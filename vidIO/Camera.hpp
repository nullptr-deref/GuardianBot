#pragma once

#include <memory>

#include "CVCameraAdapter.hpp"

namespace vidIO {
    class Camera {
    public:
        Camera();
        ~Camera();
        bool open();
        void close();
        Frame nextFrame();
    private:
        std::unique_ptr<CameraAdapter> adapter = nullptr;
    };
}