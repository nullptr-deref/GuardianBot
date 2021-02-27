#include "CVCameraAdapter.hpp"

namespace vidIO {
    CVCameraAdapter::CVCameraAdapter() { this->open(); }
    bool CVCameraAdapter::open() {
        if (!cap.isOpened()) return cap.open(0);

        return false;
    }
    void CVCameraAdapter::close() { if (cap.isOpened()) cap.release(); }
    Frame CVCameraAdapter::nextFrame() {
        cv::Mat frame;
        cap.read(frame);

        return frame;
    }
    CVCameraAdapter::~CVCameraAdapter() { this->close(); }
}