#include "Camera.hpp"

namespace vidIO {
    Camera::Camera() {
        adapter = std::make_unique<CVCameraAdapter>();
        adapter->open();
    }
    Frame Camera::nextFrame() { return adapter->nextFrame(); }
    bool Camera::open() { return adapter->open(); }
    void Camera::close() { adapter->close(); }
    Camera::~Camera() { adapter->close(); }
    auto Camera::frameData() const -> const FrameData & {
        return adapter->frameData();
    }
}