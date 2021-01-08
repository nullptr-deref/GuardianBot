#pragma once

#include <librealsense2/rs.hpp>

class Camera
{
    rs2::context m_ctx;
    rs2::device m_dev;
    rs2::pipeline m_pipe;

public:
    Camera();
    ~Camera();

    rs2::frameset waitForFrames();
};