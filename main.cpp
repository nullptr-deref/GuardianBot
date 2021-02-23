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
#include "Serial/SerialPort.hpp"
#include "gl/glstuff.hpp"
#include "gl/VertexBuffer.hpp"
#include "gl/VertexArray.hpp"
#include "gl/Texture.hpp"
#include "gl/IndexBuffer.hpp"
#include "gl/Program.hpp"

using Image = cv::Mat;
using StdGuard = std::lock_guard<std::mutex>;

const uint32_t COMMAND_SIZE = 16;
void clearBuffer(char *buf, const size_t bsize);

int main(int argc, char **argv)
{
    cli::ArgumentParser argParser(2);
    argParser.defineArgument("-p", "--prototxt", true);
    argParser.defineArgument("-m", "--model", true);
    Map<std::string, std::string> args;
    try { args = argParser.parseArgs(argc, argv); }
    catch(const std::runtime_error &e)
    {
        std::cerr << e.what() << '\n';
        return 0;
    }

    Camera cam;

    std::mutex frameMutex;
    Image frameToDraw;
    std::atomic_bool isExpired = true;
    
    const unsigned int CAPACITY = 4;
    rs2::frame_queue queue(CAPACITY);

    std::vector<cv::Mat> detectionsQueue;
    std::mutex detectionsMutex;

    std::atomic_size_t humansWatched = 0;

    const unsigned int BUF_SIZE = 256u;
    char arduinoCommandBuf[BUF_SIZE] = { 0 };

    SerialPort *connected;
    const auto serialMode = SerialMode::Write;
    std::string currentConnectedName;

    std::thread interfaceThread([&]
    {
        std::clog << "[THREAD] Interface thread created.\n";

        if (!glfwInit()) throw std::runtime_error("Could not initialize GLFW.");

        GLFWwindow *wnd = gl::createDefaultWindow();
        glfwMakeContextCurrent(wnd);
        glfwSwapInterval(1);

        if (glewInit() != GLEW_OK) throw std::runtime_error("Could not initialize GLEW.");

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        const GLuint VERTICES_COUNT = 4;
        const float verticesData[16] =
        {
            // Positions  // Texture coordinates
            -1.0f,  1.0f, 0.0f, 1.0f,
             1.0f,  1.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f
        };

        gl::VertexBuffer vb(verticesData, VERTICES_COUNT * 4, GL_STATIC_DRAW);
        gl::VertexArray va;
        gl::VertexArrayLayout layout;
        layout.addAttribute(2, GL_FLOAT, true);
        layout.addAttribute(2, GL_FLOAT, true);
        va.setLayout(layout);

        const unsigned int ELEMENTS_COUNT = 6;
        const GLuint indices[ELEMENTS_COUNT] =
        {
            0, 1, 2,
            0, 2, 3
        };
        gl::IndexBuffer ib(indices, ELEMENTS_COUNT, GL_STATIC_DRAW);
        gl::Texture tex(GL_TEXTURE_2D);
        tex.setAttribute(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        tex.setAttribute(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        tex.setAttribute(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        tex.setAttribute(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        const gl::Program prog = gl::loadDefaultShaders();
        prog.use();

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize = { 1.0f, 1.0f };
        io.Fonts->AddFontDefault();
        io.Fonts->Build();
        
        ImGui::StyleColorsDark();
        ImGuiStyle &style = ImGui::GetStyle();
        style.FrameBorderSize = 1.0f;

        ImGui_ImplGlfw_InitForOpenGL(wnd, true);
        ImGui_ImplOpenGL3_Init("#version 430");

        bool humanCounterShown = true;
        bool controllerShown = true;
        bool serialShown = true;

        while (!glfwWindowShouldClose(wnd))
        {
            glClear(GL_COLOR_BUFFER_BIT);

            {
                StdGuard g(frameMutex);
                if (!isExpired.load())
                {
                    gl::loadCVmat2GLTexture(tex, frameToDraw, true);
                    isExpired = true;
                }
            }
            tex.bind();

            glDrawElements(GL_TRIANGLES, ELEMENTS_COUNT, GL_UNSIGNED_INT, nullptr);
            tex.unbind();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Watcher part
            ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Always);
            ImGui::SetNextWindowSize({ imguic::watcher::w, imguic::watcher::h }, ImGuiCond_Always);
            ImGui::Begin("watcher", &humanCounterShown);
            {
                const std::string infoLabel = "Currently watching %u " + std::string(humansWatched.load() == 1 ? "person" : "persons");

                ImGui::BeginChild("Output");
                ImGui::Text(infoLabel.c_str(), humansWatched.load());
                ImGui::EndChild();
            }
            ImGui::End();

            // Controller part
            ImGui::SetNextWindowPos({ imguic::controller::x, imguic::controller::y }, ImGuiCond_Always);
            ImGui::SetNextWindowSize({ imguic::controller::w, imguic::controller::h }, ImGuiCond_Always);
            ImGui::Begin("controller", &controllerShown);
                ImGui::BeginChild("port", ImVec2(0, 64), true);
                if (ImGui::BeginMenu("COM ports"))
                {
                    const std::vector<std::string> ports = SerialPort::queryAvailable();
                    for (const auto &port : ports)
                    {
                        if (ImGui::MenuItem(port.c_str()))
                        {
                            connected = new SerialPort(port.c_str(), SerialMode::Write, serialMode);
                            currentConnectedName = port;
                        }
                    }

                    ImGui::EndMenu();
                }
                const std::string connectionLabel = currentConnectedName.size() > 0 ?
                    "Currently connected to " + currentConnectedName :
                    "Do not connected to any port now.";
                ImGui::Text(connectionLabel.c_str());
                ImGui::EndChild();

                ImGui::InputText("Type a command", arduinoCommandBuf, BUF_SIZE);
                if (ImGui::Button("Send", { imguic::controller::btnW, imguic::controller::btnH }))
                {
                    connected->open();
                    std::clog << "[PORT INFO] Sending: ";
                    for (uint32_t i = 0; i < COMMAND_SIZE; i++) std::clog << arduinoCommandBuf[i];
                    std::clog << '\n';

                    connected->write(arduinoCommandBuf, BUF_SIZE);
                    std::clog << "Wrote to port successfully.\n";
                    connected->close();
                    clearBuffer(arduinoCommandBuf, BUF_SIZE);
                }
            ImGui::End();
            ImGui::EndFrame();

            int display_w, display_h;
            glfwGetFramebufferSize(wnd, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(wnd);
            glfwPollEvents();
        }

        prog.del();
        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(wnd);
        glfwTerminate();

        std::clog << "[THREAD] Interface thread closed.\n";
    });
    interfaceThread.detach();

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
        humansWatched = faceRects.size();

        const cv::Scalar borderColor = { 0, 0, 255 };
        const unsigned int borderThickness = 4u;
        for (const cv::Rect &r : faceRects) cv::rectangle(cvFrame, r, borderColor, borderThickness);

        StdGuard g(frameMutex);
        frameToDraw = cvFrame;
        isExpired = false;
    }
    std::clog << "[THREAD] Main thread loop destroyed.\n";
    connected->close();

    return 0;
}

void clearBuffer(char *buf, const size_t bsize)
{
    for (size_t i = 0; i < bsize; i++) buf[i] = 0;
}