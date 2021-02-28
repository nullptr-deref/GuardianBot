#pragma once

#include <librealsense2/rs.hpp>

#include "CameraAdapter.hpp"

namespace vidIO {
    class RSCameraAdapter : public CameraAdapter {
    public:
        RSCameraAdapter() = default;
        ~RSCameraAdapter();
        bool open() override;
        void close() override;
        Frame nextFrame() override;

    private:
        rs2::context context;
        rs2::device device;
        rs2::pipeline pipe;
    };
}