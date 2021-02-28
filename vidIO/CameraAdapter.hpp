#pragma once

#include <opencv2/imgproc.hpp>

namespace vidIO {
    using Frame = cv::Mat;
    class CameraAdapter {
    public:
        virtual bool open() = 0;
        virtual void close() = 0;
        virtual Frame nextFrame() = 0;
    };
}