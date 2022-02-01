#pragma once

#include <opencv2/imgproc.hpp>

namespace vidIO {
    using Frame = cv::Mat;
    struct FrameData {
        uint64_t width;
        uint64_t height;
    };

    class CameraAdapter {
    public:
        virtual bool open() = 0;
        virtual void close() = 0;
        virtual Frame nextFrame() = 0;
        auto frameData() const -> const FrameData &;
    protected:
        FrameData fdat;
    };
}