#include "CVCameraAdapter.hpp"

namespace vidIO {
    CVCameraAdapter::CVCameraAdapter() {
        this->open();
        this->fdat.width = cap_.get(cv::CAP_PROP_FRAME_WIDTH);
        this->fdat.height = cap_.get(cv::CAP_PROP_FRAME_HEIGHT);
    }

    bool CVCameraAdapter::open() {
        return cap_.open(0);
    }

    void CVCameraAdapter::close() { if (cap_.isOpened()) cap_.release(); }
    Frame CVCameraAdapter::nextFrame() {
        cv::Mat frame;
        if (!cap_.read(frame))
            throw std::runtime_error("Device could not read frame.");

        return frame;
    }

    CVCameraAdapter::~CVCameraAdapter() { this->close(); }
}