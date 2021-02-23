#pragma once

#include <GL/glew.h>

#include "Shader.hpp"

namespace gl {
    class Program {
    public:
        Program();
        ~Program() noexcept = default;

        void attachShader(const Shader &shader);
        void detachShader(const Shader &shader);
        bool link() const;
        bool validate() const;
        void use() const;
        void stopUse() const;
        void del() const;
        std::string getInfoLog() const;

        GLuint getID() const;

    private:
        GLuint id;
    };
}