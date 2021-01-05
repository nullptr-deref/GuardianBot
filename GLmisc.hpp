#pragma once

#include <iostream>

#include "GL/glew.h"

namespace gl
{
    static auto clearErrors() -> void
    {
        while (GL_NO_ERROR != glGetError());
    }

    static auto logCall() -> bool
    {
        while (GLenum error = glGetError())
        {
            std::cerr << std::hex << "OpenGL returned error code [" << error << ']' << std::endl;

            return false;
        }

        return true;
    }

    template <typename T>
    void assert(T val)
    {
        if (!val) __debugbreak();
    }

    template <typename T>
    void call(T op)
    {
        clearErrors();
        op();
        gl::assert(logCall());
    }
}