#ifndef GUID_DEVINTERFACE_USB_DEVICE
#include <initguid.h>
#include <usbiodef.h>
#endif

#include <iostream>
#include <thread>
#include <mutex>

#include <librealsense2/rs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>

#include "ArgumentParser.hpp"
#include "ImageCompare/imgcmp.hpp"
#include "MatrixCopy/MatrixCopy.hpp"

using Image = cv::Mat;

cv::Mat copySubMatrix(const cv::Mat &inp, int startRow, int startCol, int rowCount, int colCount);
inline int clamp(int val, int min, int max);

int main(int argc, char **argv)
{
    rs2::context ctx;

    cli::ArgumentParser argParser;
    argParser.defineArgument("-p", "--prototxt");
    argParser.defineArgument("-m", "--model");
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

    rs2::device_list list = ctx.query_devices();
    if (0 == list.size())
    {
        std::cerr << "No devices connected.\n";
        return 0;
    }
    
    rs2::device dev = list.front();
    rs2::pipeline pipe;
    pipe.start();

    const unsigned int CAPACITY = 4;
    rs2::frame_queue queue(CAPACITY);

    const std::string windowName = "Viewport";
    cv::namedWindow(windowName, cv::WindowFlags::WINDOW_FULLSCREEN);

    const cv::String faceWndName = "Detected face";
    cv::namedWindow(faceWndName, cv::WindowFlags::WINDOW_AUTOSIZE);

    const unsigned int RESIZED_IMAGE_SIZE = 256u;

    std::vector<cv::Mat> detectionsQueue;
    std::mutex detectionsMutex;
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

    const float defaultConfidence = 0.7f;

    std::vector<cv::Rect> faceRects;
    std::clog << "[THREAD] Entering main thread loop.\n";
    while (cv::waitKey(1) < 0 && cv::getWindowProperty(windowName, cv::WindowPropertyFlags::WND_PROP_AUTOSIZE) >= 0)
    {
        const rs2::frameset fs = pipe.wait_for_frames();
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

        std::clog << "[INFO] currently watching " << faceRects.size() << (faceRects.size() == 1 ? " person" : " persons") << ".\n";

        const cv::Scalar borderColor = { 0, 0, 255 };
        const unsigned int borderThickness = 4u;
        for (const cv::Rect &r : faceRects) cv::rectangle(cvFrame, r, borderColor, borderThickness);

        cv::imshow(windowName, cvFrame);
    }
    cv::destroyAllWindows();

    return 0;
}

inline int clamp(int val, int min, int max)
{
    if (val < min) return min;
    if (val > max) return max;

    return val;
}

cv::Mat copySubMatrix(const cv::Mat &inp, int startRow, int startCol, int rowCount, int colCount)
{
    const cv::Mat rr = inp.rowRange(clamp(startRow, 0, inp.rows), clamp(startRow + rowCount, 0, inp.rows));

    return rr.colRange(clamp(startCol, 0, rr.cols), clamp(startCol + rowCount, 0, rr.cols));
}