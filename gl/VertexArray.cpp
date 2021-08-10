#include "VertexArray.hpp"

#include "glstuff.hpp"

namespace gl {
    VertexArray::VertexArray() {
        glGenVertexArrays(1, &id);
        this->bind();
    }
    VertexArray::~VertexArray() {
        this->unbind();
        glDeleteVertexArrays(1, &id);
    }

    void VertexArray::setLayout(const VertexArrayLayout &l) {
        ptrdiff_t pointer = 0;
        for (auto [index, count, type, norm] : l.attributes) {
            glVertexAttribPointer(index, count, type, static_cast<bool>(norm), l.stride, (const void *)pointer);
            pointer += count * retrieveTypeSize(type);
            this->enableAttribute(index);
        }
    }
    void VertexArray::enableAttribute(unsigned int index) {
        glEnableVertexArrayAttrib(id, index);
    }
    void VertexArray::disableAttribute(unsigned int index) {
        glDisableVertexArrayAttrib(id, index);
    }

    void VertexArray::bind() const { glBindVertexArray(id); }
    void VertexArray::unbind() const { glBindVertexArray(0); }
}