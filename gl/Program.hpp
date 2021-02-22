#pragma once

#include <GL/glew.h>

#include "Shader.hpp"

namespace gl {
    class Program {
    public:
        Program();
        ~Program() noexcept;

        void attachShader(const Shader &shader);
        bool link();

    private:
        GLuint id;
    };
}