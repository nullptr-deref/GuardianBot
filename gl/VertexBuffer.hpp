#pragma once

#include <GL/glew.h>

namespace gl {
    template <typename T>
    class VertexBuffer {
    public:
        VertexBuffer() {
            glGenBuffers(1, &id);
        }
        explicit VertexBuffer(const T *data, unsigned int count, GLenum usage) {
            glGenBuffers(1, &id);
            this->bind();
            glBufferData(GL_ARRAY_BUFFER, count * sizeof(T), data, usage);
        }
        ~VertexBuffer() {
            glDeleteBuffers(1, &id);
            this->unbind();
        }

        void bind() const {
            glBindBuffer(GL_ARRAY_BUFFER, id);
        }
        void unbind() const {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    private:
        GLuint id;
    };
}