#ifndef GUID_DEVINTERFACE_USB_DEVICE
#include <initguid.h>
#include <usbiodef.h>
#endif

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <thread>
#include <mutex>
#include <tuple>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLmisc.hpp"
#include "ImGuiConstants.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <librealsense2/rs.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>

#include "Camera.hpp"
#include "ArgumentParser.hpp"
#include "ImageCompare/imgcmp.hpp"
#include "MatrixCopy/MatrixCopy.hpp"
#include "Serial/SerialPort.hpp"

using Image = cv::Mat;
void loadCVmat2GLtexture(cv::Mat& image, bool shouldFlip = false);
GLuint loadDefaultShaders();

const char *PORT_NAME = "\\\\.\\COM5";
const uint32_t COMMAND_SIZE = 16;
void clearBuffer(char *buf, const size_t bsize);

int main(int argc, char **argv)
{
    cli::ArgumentParser argParser(2);
    argParser.defineArgument("-p", "--prototxt", true);
    argParser.defineArgument("-m", "--model", true);
    Map<std::string, std::string> args;
    try
    {
        args = argParser.parseArgs(argc, argv);
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
        return 0;
    }

    Camera cam;

    Image frameToDraw;
    bool isExpired = true;
    std::mutex drawnFrameMut;
    
    const unsigned int CAPACITY = 4;
    rs2::frame_queue queue(CAPACITY);

    std::vector<cv::Mat> detectionsQueue;
    std::mutex detectionsMutex;

    size_t humansWatched = 0;
    std::mutex humansCountMut;

    const unsigned int BUF_SIZE = 256u;
    char arduinoCommandBuf[BUF_SIZE] = { 0 };

    //TODO: make port name optional (type it through cli or something like that)
    SerialPort *port = new SerialPort(PORT_NAME, SerialMode::Write);
    port->close();

    std::thread inputOutputThread([&]
    {
        std::clog << "[THREAD] Input thread created.\n";

        if (!glfwInit()) throw std::runtime_error("Could not initialize GLFW.");

        GLFWmonitor *pmonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *vmode = glfwGetVideoMode(pmonitor);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_TRUE);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        GLFWwindow *IOwindow = glfwCreateWindow(vmode->width - 100, vmode->height - 100, "Viewport", nullptr, nullptr);
        glfwSetWindowPos(IOwindow, 100, 100);

        glfwMakeContextCurrent(IOwindow);
        glfwSwapInterval(1);

        if (glewInit() != GLEW_OK) throw std::runtime_error("Could not initialize GLEW.");

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        const GLuint VERTICES_COUNT = 4;
        const float verticesData[] =
        {
            // Positions  // Texture coordinates
            -1.0f,  1.0f, 0.0f, 1.0f,
             1.0f,  1.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f
        };

        GLuint vbo;
        gl::call([&] { glGenBuffers(1, &vbo); });
        gl::call([&] { glBindBuffer(GL_ARRAY_BUFFER, vbo); });
        gl::call([&] { glBufferData(GL_ARRAY_BUFFER, VERTICES_COUNT * 4 * sizeof(float), verticesData, GL_STATIC_DRAW); });

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        // IMPORTANT: before specifying the vertex buffer layout, you need you buffer to be bound to context
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 2, GL_FLOAT, true, 4 * sizeof(float), nullptr);
        glVertexAttribPointer(1, 2, GL_FLOAT, true, 4 * sizeof(float), (const void *)(2 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        const unsigned int ELEMENTS_COUNT = 6;
        const GLuint indices[ELEMENTS_COUNT] =
        {
            0, 1, 2,
            0, 2, 3
        };
        GLuint ibo;
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ELEMENTS_COUNT * sizeof(GLuint), indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        GLuint drawnFrameTexture;
        glGenTextures(1, &drawnFrameTexture);
        glBindTexture(GL_TEXTURE_2D, drawnFrameTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        GLuint defaultProgram = loadDefaultShaders();

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize = { 1.0f, 1.0f };
        io.Fonts->AddFontDefault();
        io.Fonts->Build();
        
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(IOwindow, true);
        ImGui_ImplOpenGL3_Init("#version 430");

        bool humanCounterShown = true;
        bool controllerShown = true;

        // char eventBuf[imguic::BUFFER_SIZE] = { 0 }; // Should necessarily be filled with zeros!

        while (!glfwWindowShouldClose(IOwindow))
        {
            gl::call([] { glClear(GL_COLOR_BUFFER_BIT); });

            gl::call([&] { glBindTexture(GL_TEXTURE_2D, drawnFrameTexture); });
            {
                std::lock_guard<std::mutex> frameCopyLock(drawnFrameMut);
                if (!isExpired)
                {
                    loadCVmat2GLtexture(frameToDraw, true);
                    isExpired = true;
                }
            }
            
            gl::call([&] { glBindBuffer(GL_ARRAY_BUFFER, vbo); });
            gl::call([&] { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); });
            gl::call([&] { glBindVertexArray(vao); });
            gl::call([&] { glEnableVertexAttribArray(0); });
            gl::call([&] { glEnableVertexAttribArray(1); });
            gl::call([&] { glUseProgram(defaultProgram); });

            gl::call([&] { glDrawElements(GL_TRIANGLES, ELEMENTS_COUNT, GL_UNSIGNED_INT, nullptr); });

            gl::call([&] { glBindTexture(GL_TEXTURE_2D, 0); });
            gl::call([&] { glDisableVertexAttribArray(0); });
            gl::call([&] { glDisableVertexAttribArray(1); });
            gl::call([&] { glBindBuffer(GL_ARRAY_BUFFER, 0); });
            gl::call([&] { glBindVertexArray(0); });
            gl::call([&] { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); });
            gl::call([&] { glUseProgram(0); });

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Watcher part
            ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Always);
            ImGui::SetNextWindowSize({ imguic::watcher::w, imguic::watcher::h }, ImGuiCond_Always);
            ImGui::Begin("watcher", &humanCounterShown);
            {
                std::lock_guard<std::mutex> watchedPersonsGuard(humansCountMut);
                const std::string infoLabel = "Currently watching %u " + std::string(humansWatched == 1 ? "person" : "persons");

                ImGui::BeginChild("Output");
                ImGui::Text(infoLabel.c_str(), humansWatched);
                ImGui::EndChild();
            }
            ImGui::End();

            // Controller part
            ImGui::SetNextWindowPos({ imguic::controller::x, imguic::controller::y }, ImGuiCond_Always);
            ImGui::SetNextWindowSize({ imguic::controller::w, imguic::controller::h }, ImGuiCond_Always);
            ImGui::Begin("controller", &controllerShown);
                ImGui::InputText("Type a command", arduinoCommandBuf, BUF_SIZE);
                if (ImGui::Button("Send", { imguic::controller::btnW, imguic::controller::btnH }))
                {
                    port->open(PORT_NAME, SerialMode::Write);
                    std::clog << "[PORT INFO] Sending: ";
                    for (uint32_t i = 0; i < COMMAND_SIZE; i++) std::clog << arduinoCommandBuf[i];
                    std::clog << '\n';

                    port->write(arduinoCommandBuf, BUF_SIZE);
                    std::clog << "Wrote to port successfully.\n";
                    port->close();
                    clearBuffer(arduinoCommandBuf, BUF_SIZE);
                }
            ImGui::End();
            ImGui::EndFrame();

            int display_w, display_h;
            glfwGetFramebufferSize(IOwindow, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(IOwindow);
            glfwPollEvents();
        }

        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();

        glDeleteProgram(defaultProgram);
        glDeleteTextures(1, &drawnFrameTexture);
        glfwDestroyWindow(IOwindow);
        glfwTerminate();

        std::clog << "[THREAD] Input thread closed.\n";
    });
    inputOutputThread.detach();

    std::thread netThread([&]
    {
        std::clog << "[THREAD] Created network thread.\n";
        cv::dnn::Net nnet = cv::dnn::readNetFromCaffe(args["prototxt"], args["model"]);

        while (true)
        {
            rs2::frame frame;
            if (queue.poll_for_frame(&frame))
            {
                const int fWidth = frame.as<rs2::video_frame>().get_width();
                const int fHeight = frame.as<rs2::video_frame>().get_height();
                
                const Image im({ fWidth, fHeight }, CV_8UC3, const_cast<void *>(frame.get_data()), cv::Mat::AUTO_STEP);

                const cv::Scalar mean = cv::Scalar(104.0, 177.0, 123.0);
                const cv::Mat blob = cv::dnn::blobFromImage(im, 1.0f, cv::Size(300, 300), mean, false, false);
                nnet.setInput(blob);
                const cv::Mat detection = nnet.forward();

                std::lock_guard<std::mutex> detectionsWriteGuard(detectionsMutex);
                // As far as I understood, cv::Mat::size represents:
                // size[0] - mat rows
                // size[1] - mat columns
                // size[2] - mat depth
                // size[3] - something like data per detection (especially for detections
                // produced by cv::Net)
                
                const cv::Mat detections = cv::Mat(detection.size[2], detection.size[3], CV_32F, (void *)detection.ptr<float>());
                detectionsQueue.push_back(detections);
            }
        }
        std::clog << "[THREAD] Network thread destroyed.\n";
    });
    netThread.detach();

    const float defaultConfidence = 0.8f;
    std::vector<cv::Rect> faceRects;
    std::clog << "[THREAD] Entering main thread loop.\n";
    while (cv::waitKey(1) < 0)
    {
        const rs2::frameset fs = cam.waitForFrames();
        queue.enqueue(fs.get_color_frame());

        const rs2::video_frame frame = fs.get_color_frame();
        const Image cvFrame = Image({ frame.get_width(), frame.get_height() }, CV_8UC3, const_cast<void *>(frame.get_data()), cv::Mat::AUTO_STEP);
        if (cvFrame.dims > 2 || cvFrame.dims < 2) continue;

        std::lock_guard<std::mutex> detectionsGuard(detectionsMutex);
        if (!detectionsQueue.empty())
        {
            while (!faceRects.empty()) faceRects.pop_back();

            const cv::Mat detections = detectionsQueue.back();
            detectionsQueue.pop_back();
            for (int i = 0; i < detections.rows; i++)
            {
                const float confidence = detections.at<float>(i, 2);

                if (confidence >= defaultConfidence)
                {
                    const int xLeftBottom = static_cast<int>(detections.at<float>(i, 3) * cvFrame.cols);
                    const int yLeftBottom = static_cast<int>(detections.at<float>(i, 4) * cvFrame.rows);
                    const int xRightTop = static_cast<int>(detections.at<float>(i, 5) * cvFrame.cols);
                    const int yRightTop = static_cast<int>(detections.at<float>(i, 6) * cvFrame.rows);

                    faceRects.emplace_back
                    (
                        xLeftBottom,
                        yLeftBottom,
                        xRightTop - xLeftBottom,
                        yRightTop - yLeftBottom
                    );
                }
            }
        }
        //std::lock_guard<std::mutex> humansCountGuard(humansCountMut);
        humansCountMut.lock();
        humansWatched = faceRects.size();
        humansCountMut.unlock();

        const cv::Scalar borderColor = { 0, 0, 255 };
        const unsigned int borderThickness = 4u;
        for (const cv::Rect &r : faceRects) cv::rectangle(cvFrame, r, borderColor, borderThickness);

        {
            std::lock_guard<std::mutex> rendering(drawnFrameMut);
            cvFrame.copyTo(frameToDraw);

            isExpired = false;
        }
    }
    std::clog << "[THREAD] Main thread loop destroyed.\n";
    port->close();

    return 0;
}

void loadCVmat2GLtexture(cv::Mat& image, bool shouldFlip)
{
    if(image.empty()) std::cerr << "Image is empty.\n";
    else
    {
        cv::Mat processed;
        if (shouldFlip) cv::flip(image, processed, 0);
        else image.copyTo(processed);
        
        gl::call([&] { glTexImage2D(GL_TEXTURE_2D,
                                    0,
                                    GL_RGB,
                                    processed.cols,
                                    processed.rows,
                                    0,
                                    GL_BGR,
                                    GL_UNSIGNED_BYTE,
                                    processed.data
                                   );
        });
    }
}

std::string parseShader(const std::string &filename)
{
    std::ifstream inp(filename);
    std::string buf;
    std::stringstream ss;

    while(std::getline(inp, buf)) {
        if (buf.find("shader") != std::string::npos) continue;
        ss << buf << '\n';
    }

    return ss.str();
}

GLuint loadDefaultShaders()
{
    const std::string vertexShaderFilePath = std::filesystem::absolute("resources/VertexDefault.shader").string();
    // Here I'm using 2-step assignment to avoid undefined behaviour when c_str() of a std::string
    // will point to somewhere (I mean some temporary address which will expire till the end of the operand).
    // It also makes sense while using such a trick with fragment shader compilation.
    const std::string vShaderSource = parseShader(vertexShaderFilePath);
    const char *vShaderSourceRaw = vShaderSource.c_str();
    GLuint vertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
    gl::call([&] { glShaderSource(vertexShaderObj, 1, &vShaderSourceRaw, nullptr); });
    gl::call([&] { glCompileShader(vertexShaderObj); });
    int vShaderCompileStatus;
    glGetShaderiv(vertexShaderObj, GL_COMPILE_STATUS, &vShaderCompileStatus);

    const std::string fragmentShaderFilePath = std::filesystem::absolute("resources/FragmentDefault.shader").string();
    const std::string fShaderSource = parseShader(fragmentShaderFilePath);
    const char *fShaderSourceRaw = fShaderSource.c_str();
    GLuint fragmentShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
    gl::call([&] { glShaderSource(fragmentShaderObj, 1, &fShaderSourceRaw, nullptr); });
    gl::call([&] { glCompileShader(fragmentShaderObj); });
    int fShaderCompileStatus;
    glGetShaderiv(fragmentShaderObj, GL_COMPILE_STATUS, &fShaderCompileStatus);

    GLuint prog = glCreateProgram();
    gl::call([&] { glAttachShader(prog, vertexShaderObj); });
    gl::call([&] { glAttachShader(prog, fragmentShaderObj); });
    gl::call([&] { glLinkProgram(prog); });
    gl::call([&] { glValidateProgram(prog); });
    int progLinkStatus;
    glGetProgramiv(prog, GL_LINK_STATUS, &progLinkStatus);

    if (!(vShaderCompileStatus && fShaderCompileStatus && progLinkStatus)) throw std::runtime_error("Default shaders could not be loaded");

    glDeleteShader(vertexShaderObj);
    glDeleteShader(fragmentShaderObj);

    return prog;
}

void clearBuffer(char *buf, const size_t bsize)
{
    for (size_t i = 0; i < bsize; i++) buf[i] = 0;
}