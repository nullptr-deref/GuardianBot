#pragma once

#include <GL/glew.h>

#include "VertexArrayLayout.hpp"

namespace gl {
    class VertexArray {
    public:
        VertexArray();
        ~VertexArray();

        void setLayout(const VertexArrayLayout &l);
        void enableAttribute(unsigned int index);
        void disableAttribute(unsigned int index);

        void bind() const;
        void unbind() const;

    private:
        GLuint id;
    };
}