#include "CVCameraAdapter.hpp"

namespace vidIO {
    CVCameraAdapter::CVCameraAdapter() { this->open(); }
    void CVCameraAdapter::open() { if (!cap.isOpened()) cap.open(0); }
    void CVCameraAdapter::close() { if (cap.isOpened()) cap.release(); }
    Frame CVCameraAdapter::nextFrame() {
        cv::Mat frame;
        cap.read(frame);

        return frame;
    }
    CVCameraAdapter::~CVCameraAdapter() { this->close(); }
}