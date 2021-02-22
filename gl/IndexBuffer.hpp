#pragma once

#include <GL/glew.h>
namespace gl {
    template <typename T>
    class IndexBuffer {
    public:
        IndexBuffer() {
            glGenBuffers(1, &id);
            this->bind();
        }
        explicit IndexBuffer(const T *indices, unsigned int count, GLenum usage) {
            glGenBuffers(1, &id);
            this->bind();
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(T), (const void *)indices, usage);
        }
        
        void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); }
        void unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
    private:
        GLuint id;
        T *indices;
        unsigned int count;
    };

    template <typename T>
    using ElementArrayBuffer = IndexBuffer<T>;
}