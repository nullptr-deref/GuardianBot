#ifndef GUID_DEVINTERFACE_USB_DEVICE
#include <initguid.h>
#include <usbiodef.h>
#endif

#include <iostream>
#include <thread>
#include <mutex>
#include <tuple>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
#include "events/Emitter.hpp"
#include "events/Listener.hpp"

using Image = cv::Mat;

inline int clamp(int val, int min, int max);

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
    
    const unsigned int CAPACITY = 4;
    rs2::frame_queue queue(CAPACITY);

    const cv::String windowName = "Viewport";
    cv::namedWindow(windowName, cv::WindowFlags::WINDOW_FULLSCREEN);

    const cv::String faceWndName = "Detected face";
    cv::namedWindow(faceWndName, cv::WindowFlags::WINDOW_AUTOSIZE);

    const unsigned int RESIZED_IMAGE_SIZE = 256u;

    std::vector<cv::Mat> detectionsQueue;
    std::mutex detectionsMutex;

    size_t humansWatched = 0;
    std::mutex humansCountMut;

    using namespace events;
    std::mutex eventsMutex;
    Queue q;
    Emitter globalEm(q);


    std::thread inputOutputThread([&]
    {
        std::clog << "[THREAD] Input thread created.\n";

        if (!glfwInit()) throw std::runtime_error("Could not initialize GLFW.");

        unsigned int outWndWidth = 300;
        unsigned int outWndHeight = 100;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        GLFWwindow *IOwindow = glfwCreateWindow(outWndWidth, outWndHeight, "InputOutputWindow", nullptr, nullptr);
        glfwSetWindowPos(IOwindow, 100, 100);

        glfwMakeContextCurrent(IOwindow);
        glfwSwapInterval(1);

        if (glewInit() != GLEW_OK) throw std::runtime_error("Could not initialize GLEW.");

        std::string event;

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize = { 1.0f, 1.0f };
        io.Fonts->AddFontDefault();
        io.Fonts->Build();
        
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(IOwindow, true);
        ImGui_ImplOpenGL3_Init("#version 150");

        ImVec4 alphaClear = { 0, 0, 0, 0 };

        bool isIOwindowShown = true;

        while (!glfwWindowShouldClose(IOwindow))
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Always);
            ImGui::SetNextWindowSize({ static_cast<float>(outWndWidth), static_cast<float>(outWndHeight) }, ImGuiCond_Always);
            ImGui::Begin("Controller", &isIOwindowShown);
            {
                std::lock_guard<std::mutex> watchedPersonsGuard(humansCountMut);
                const std::string infoLabel = "Currently watching %u " + std::string(humansWatched == 1 ? "person" : "persons");

                ImGui::BeginChild("Output");
                ImGui::Text(infoLabel.c_str(), humansWatched);
                ImGui::EndChild();
            }
            ImGui::End();
            ImGui::EndFrame();

            int display_w, display_h;
            glfwGetFramebufferSize(IOwindow, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(alphaClear.x, alphaClear.y, alphaClear.z, alphaClear.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(IOwindow);
            glfwPollEvents();

            std::lock_guard<std::mutex> evGuard(eventsMutex);
            globalEm.emit(Event(event));
        }

        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(IOwindow);
        glfwTerminate();
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

                const cv::Mat blob = cv::dnn::blobFromImage(im, 1.0f, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0), false, false);
                nnet.setInput(blob);
                const cv::Mat detection = nnet.forward();

                std::lock_guard<std::mutex> detectionsWriteGuard(detectionsMutex);
                const cv::Mat detections = cv::Mat(detection.size[2], detection.size[3], CV_32F, (void *)detection.ptr<float>());
                detectionsQueue.push_back(detections);
            }
        }
        std::clog << "[THREAD] Destroyed network thread.\n";
    });
    netThread.detach();

    const float defaultConfidence = 0.8f;
    std::vector<cv::Rect> faceRects;
    std::clog << "[THREAD] Entering main thread loop.\n";
    while (cv::waitKey(1) < 0 && cv::getWindowProperty(windowName, cv::WindowPropertyFlags::WND_PROP_AUTOSIZE) >= 0)
    {
        const rs2::frameset fs = cam.waitForFrames();
        queue.enqueue(fs.get_color_frame());

        const rs2::video_frame frame = fs.get_color_frame();

        const Image cvFrame = Image({ frame.get_width(), frame.get_height() }, CV_8UC3, const_cast<void *>(frame.get_data()), cv::Mat::AUTO_STEP);

        cv::Mat prevFace = cv::Mat::zeros(RESIZED_IMAGE_SIZE, RESIZED_IMAGE_SIZE, CV_8UC3);

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

                    const cv::Mat faceImage = copySubMatrix(cvFrame, yLeftBottom, xLeftBottom, yRightTop - yLeftBottom, xRightTop - xLeftBottom);
                    cv::Mat faceResized;
                    cv::resize(faceImage, faceResized, { RESIZED_IMAGE_SIZE, RESIZED_IMAGE_SIZE }, 0, 0, cv::InterpolationFlags::INTER_LINEAR);
                    std::cout << "Current face is similar to previous with " << imgcmp::compareImagesByRegions(faceResized, prevFace, 16, 25000) << " confidence\n";
                    faceResized.copyTo(prevFace);
                    cv::imshow(faceWndName, faceResized);

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
        // I'm using anonymous scopes in some places to shrink mutex affection range
        {
            std::lock_guard<std::mutex> humansCountGuard(humansCountMut);
            humansWatched = faceRects.size();
        }

        const cv::Scalar borderColor = { 0, 0, 255 };
        const unsigned int borderThickness = 4u;
        for (const cv::Rect &r : faceRects) cv::rectangle(cvFrame, r, borderColor, borderThickness);

        // TODO: maybe I should wrap everything within single window, but I need to convert OpenCV Mats
        // into OpenGL texture somehow to make this possible
        cv::imshow(windowName, cvFrame);
    }
    std::clog << "[THREAD] Exiting main thread loop.\n";
    cv::destroyAllWindows();

    return 0;
}

inline int clamp(int val, int min, int max)
{
    if (val < min) return min;
    if (val > max) return max;

    return val;
}