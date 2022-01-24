#ifndef GUID_DEVINTERFACE_USB_DEVICE
#include <initguid.h>
#include <usbiodef.h>
#endif

#include <iostream>
#include <thread>
#include <memory>
#include <queue>

#ifdef PROFILING
#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>
#define PROFILER_CALL(x) x
#define PROFC(x) PROFILER_CALL(x)
#else
#define PROFILER_CALL(x)
#define PROFC(x) PROFILER_CALL(x)
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ImGuiConstants.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>

#include "cli/ArgumentParser.hpp"
#include "Serial/SerialPort.hpp"

#include "gl/gl.hpp"

#include "vidIO/Camera.hpp"

#include "ImGuiWindows.hpp"

using Image = cv::Mat;

int main(int argc, char **argv) {
    PROFC(EASY_PROFILER_ENABLE);
    PROFC(EASY_MAIN_THREAD);

    PROFC(EASY_BLOCK("Parsing arguments"));
    cli::ArgumentParser ap;
    cli::ArgMap am;
    try {
        ap.arg(cli::ArgType::String, { .fullName = "prototxt", .shortName = "p" });
        ap.arg(cli::ArgType::String, { .fullName = "model", .shortName = "m" });
        am = ap.parse(argc, argv);
    }
    catch (const cli::BasicException &e) {
        std::cerr << e.what() << '\n';
        std::exit(-1);
    }
    PROFC(EASY_END_BLOCK);
    // WARNING!!!
    // I check for available ports here because later usage of this function deadly
    // interrupts RealSense device work and it crashes.
    // This must be placed before Camera ctor call at all costs!
    std::vector<std::string> availablePorts = SerialPort::queryAvailable();

    PROFC(EASY_BLOCK("Camera constructor call"));
    vidIO::Camera cam;
    PROFC(EASY_END_BLOCK);
    std::queue<vidIO::Frame> frameQueue;
    std::vector<cv::Rect> faceRects;
    std::vector<cv::Rect> rectsBackup;
    const cv::Scalar borderColor = { 0, 0, 255 };
    const unsigned int borderThickness = 4u;

    std::atomic_bool shouldShutdown = false;

    std::atomic_size_t humansWatched = 0;

    const unsigned int BUF_SIZE = 256u;
    char arduinoCommandBuf[BUF_SIZE] = { 0 };

    std::unique_ptr<SerialPort> connected = nullptr;

    std::thread interfaceThread([&]
    {
        std::clog << "[THREAD] Render thread created.\n";

        if (!glfwInit()) throw std::runtime_error("Could not initialize GLFW.");

        GLFWwindow *wnd = gl::createDefaultWindow("Viewport");
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
        tex.setAttr(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        tex.setAttr(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        tex.setAttr(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        tex.setAttr(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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

        while (!glfwWindowShouldClose(wnd))
        {
            glClear(GL_COLOR_BUFFER_BIT);

            PROFC(EASY_BLOCK("Loading image into texture memory"));
            if (!frameQueue.empty()) {
                const vidIO::Frame &f = frameQueue.front();
                if (!faceRects.empty()) {
                    for (const cv::Rect &r : faceRects)
                        cv::rectangle(f, r, borderColor, borderThickness);
                    rectsBackup = faceRects;
                }
                else {
                    for (const cv::Rect &r : rectsBackup)
                        cv::rectangle(f, r, borderColor, borderThickness);
                }
                gl::loadCVmat2GLTexture(tex, f, true);
                frameQueue.pop();
            }
            PROFC(EASY_END_BLOCK);
            tex.bind();

            glDrawElements(GL_TRIANGLES, ELEMENTS_COUNT, GL_UNSIGNED_INT, nullptr);
            tex.unbind();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            wnd::showWatcherWindow(humansWatched.load());
            wnd::showControllerWindow(connected, arduinoCommandBuf, BUF_SIZE, availablePorts);
            ImGui::EndFrame();

            int displayW, displayH;
            glfwGetFramebufferSize(wnd, &displayW, &displayH);
            glViewport(0, 0, displayW, displayH);
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

        shouldShutdown = true;

        std::clog << "[THREAD] Render thread closed.\n";
    });
    interfaceThread.detach();

    std::thread netThread([&]
    {
        std::clog << "[THREAD] Created network thread.\n";
        PROFC(EASY_BLOCK("Reading model from file"));
        cv::dnn::Net nnet = cv::dnn::readNetFromCaffe(am["prototxt"].get<std::string>(), am["model"].get<std::string>());
        PROFC(EASY_END_BLOCK);

        const float defaultConfidence = 0.8f;
        while (!shouldShutdown)
        {
            if (!frameQueue.empty()) {
                PROFC(EASY_BLOCK("Reading next frame from queue"));
                const vidIO::Frame &f = frameQueue.front();

                PROFC(EASY_BLOCK("Detection", profiler::colors::Blue));
                const cv::Scalar mean = cv::Scalar(104.0, 177.0, 123.0);
                const cv::Mat blob = cv::dnn::blobFromImage(f, 1.0f, cv::Size(300, 300), mean, false, false);
                nnet.setInput(blob);
                const cv::Mat detection = nnet.forward();
                PROFC(EASY_END_BLOCK);

                // As far as I understood, cv::Mat::size represents:
                // size[0] - mat rows
                // size[1] - mat columns
                // size[2] - mat depth
                // size[3] - something like data per detection (especially for detections
                // produced by cv::Net)
                
                const cv::Mat detections = cv::Mat(detection.size[2], detection.size[3], CV_32F, (void *)detection.ptr<float>());
                if (!faceRects.empty()) faceRects.clear();

                for (int i = 0; i < detections.rows; i++) {
                    const float confidence = detections.at<float>(i, 2);

                    if (confidence >= defaultConfidence) {
                        const int xLeftBottom = static_cast<int>(detections.at<float>(i, 3) * f.cols);
                        const int yLeftBottom = static_cast<int>(detections.at<float>(i, 4) * f.rows);
                        const int xRightTop = static_cast<int>(detections.at<float>(i, 5) * f.cols);
                        const int yRightTop = static_cast<int>(detections.at<float>(i, 6) * f.rows);

                        faceRects.emplace_back
                        (
                            xLeftBottom,
                            yLeftBottom,
                            xRightTop - xLeftBottom,
                            yRightTop - yLeftBottom
                        );
                    }
                }
                humansWatched = faceRects.size();

            }
        }
        std::clog << "[THREAD] Network thread destroyed.\n";
    });
    netThread.detach();

    std::clog << "[THREAD] Entering main thread loop.\n";
    while (!shouldShutdown)
    {
        PROFC(EASY_BLOCK("Reading next frame from camera"));
        const vidIO::Frame frame = cam.nextFrame();
        frameQueue.push(frame);
        PROFC(EASY_END_BLOCK);
    }
    std::clog << "[THREAD] Main thread loop destroyed.\n";
    connected->close();

    PROFC(profiler::dumpBlocksToFile("C:/dev/GuardianBot/dumps/test.prof"));
    return 0;
}