#pragma once

#include <GL/glew.h>

#include "Shader.hpp"

namespace gl {
    class Program {
    public:
        Program();
        ~Program() noexcept;

        void attachShader(const Shader &shader);
        void attachShader(GLuint shaderID) { glAttachShader(id, shaderID); }
        bool link();
        bool validate();
        void use() const;
        void stopUse() const;
        std::string getInfoLog();

        GLuint getID() const;

    private:
        GLuint id;
    };
}