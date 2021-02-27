#include "Camera.hpp"

namespace vidIO {
    Camera::Camera() {
        adapter = std::make_unique<RSCameraAdapter>();
        if (!adapter->open()) {
            adapter.release();
            adapter = std::make_unique<CVCameraAdapter>();
        }
        this->open();
    }
    Frame Camera::nextFrame() { return adapter->nextFrame(); }
    bool Camera::open() { return adapter->open(); }
    void Camera::close() { adapter->close(); }
    Camera::~Camera() { adapter->close(); }
}