#include "Camera.hpp"

Camera::Camera()
{
    const rs2::device_list l = m_ctx.query_devices();
    if (0 == l.size()) throw std::runtime_error("No devices connected.");
    m_dev = l.front();

    m_pipe.start();
}

rs2::frameset Camera::waitForFrames()
{
    return m_pipe.wait_for_frames();
}

Camera::~Camera()
{
    m_pipe.stop();
}