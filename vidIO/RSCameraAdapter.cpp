#include "RSCameraAdapter.hpp"

namespace vidIO {
    Frame RSCameraAdapter::nextFrame() {
        const rs2::frameset fs = pipe.wait_for_frames();
        const rs2::video_frame vf = fs.get_color_frame();

        return Frame({ vf.get_width(), vf.get_height() }, CV_8UC3, const_cast<void *>(vf.get_data()), cv::Mat::AUTO_STEP);
    }
    bool RSCameraAdapter::open() {
        rs2::context ctx;
        rs2::device_list devList = ctx.query_devices();
        const bool isConnected = devList.size() == 0;
        if (!isConnected) throw std::runtime_error("No RealSense devices connected.");
        this->device = devList.front();
        this->pipe.start();

        return isConnected;
    }
    void RSCameraAdapter::close() { this->pipe.stop(); }
    RSCameraAdapter::~RSCameraAdapter() { this->close(); }
}