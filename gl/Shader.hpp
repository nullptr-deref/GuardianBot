#pragma once

#include <string>

#include <GL/glew.h>

namespace gl {
    class Shader {
    public:
        Shader(GLenum type);
        explicit Shader(GLenum type, const std::string &src);

        void setSource(const std::string &src);
        bool compile();
        GLuint getID() const;
    private:
        GLuint id;
    };
}