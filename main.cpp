#ifndef GUID_DEVINTERFACE_USB_DEVICE
#include <initguid.h>
#include <usbiodef.h>
#endif

#include <iostream>

#include <librealsense2/rs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>

#include "ArgumentParser.hpp"

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

    const std::string windowName = "Viewport";
    cv::namedWindow(windowName, cv::WindowFlags::WINDOW_FULLSCREEN);

    cv::dnn::Net nnet = cv::dnn::readNetFromCaffe(args["prototxt"], args["model"]);
    const float defaultConfidence = 0.8f;

    while (cv::waitKey(1) < 0 && cv::getWindowProperty(windowName, cv::WindowPropertyFlags::WND_PROP_AUTOSIZE) >= 0)
    {
        const rs2::frameset fs = pipe.wait_for_frames();

        const rs2::video_frame frame = fs.get_color_frame();
        const int frameWidth = frame.get_width();
        const int frameHeight = frame.get_height();

        const cv::Mat imageFromFrame(cv::Size(frameWidth, frameHeight), CV_8UC3, const_cast<void *>(frame.get_data()), cv::Mat::AUTO_STEP);

        const cv::Mat blob = cv::dnn::blobFromImage(imageFromFrame, 1.0f, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0), false, false);

        nnet.setInput(blob);
        const cv::Mat detection = nnet.forward();
        const cv::Mat detections(detection.size[2], detection.size[3], CV_32F, (void *)detection.ptr<float>());

        const float defaultConfidence = 0.7f;
        std::vector<cv::Rect> faces;

        for (int i = 0; i < detections.rows; i++)
        {
            const float confidence = detections.at<float>(i, 2);

            if (confidence >= defaultConfidence)
            {
                const int xLeftBottom = static_cast<int>(detections.at<float>(i, 3) * imageFromFrame.cols);
                const int yLeftBottom = static_cast<int>(detections.at<float>(i, 4) * imageFromFrame.rows);
                const int xRightTop = static_cast<int>(detections.at<float>(i, 5) * imageFromFrame.cols);
                const int yRightTop = static_cast<int>(detections.at<float>(i, 6) * imageFromFrame.rows);

                faces.emplace_back
                (
                    xLeftBottom,
                    yLeftBottom,
                    (xRightTop - xLeftBottom),
                    (yRightTop - yLeftBottom)
                );
            }
        }

        std::clog << "[INFO] currently watching " << faces.size() << (faces.size() == 1 ? " person" : " persons") << ".\n";

        const cv::Scalar borderColor = { 0, 0, 255 };
        const unsigned int borderThickness = 4u;
        for (const cv::Rect &r : faces)
        {
            cv::rectangle(imageFromFrame, r, borderColor, borderThickness);
        }

        cv::imshow(windowName, imageFromFrame);
    }

    return 0;
}