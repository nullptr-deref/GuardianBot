#include "RSCameraAdapter.hpp"

namespace vidIO {
    RSCameraAdapter::RSCameraAdapter() {
        rs2::context ctx;
        rs2::device_list devList = ctx.query_devices();
        if (0 == devList.size()) throw std::runtime_error("No RealSense devices connected.");
        this->device = devList.front();
        this->open();
    }
    Frame RSCameraAdapter::nextFrame() {
        const rs2::frameset fs = pipe.wait_for_frames();
        const rs2::video_frame vf = fs.get_color_frame();

        return Frame({ vf.get_width(), vf.get_height() }, CV_8UC3, const_cast<void *>(vf.get_data()), cv::Mat::AUTO_STEP);
    }
    void RSCameraAdapter::open() { this->pipe.start(); }
    void RSCameraAdapter::close() { this->pipe.stop(); }
    RSCameraAdapter::~RSCameraAdapter() { this->close(); }
}