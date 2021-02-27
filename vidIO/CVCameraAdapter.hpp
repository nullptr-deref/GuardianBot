#pragma once

#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "CameraAdapter.hpp"

namespace vidIO {
    class CVCameraAdapter : public CameraAdapter {
    public:
        CVCameraAdapter();
        ~CVCameraAdapter();
        bool open() override;
        void close() override;
        Frame nextFrame() override;

    private:
        cv::VideoCapture cap;
    };
}