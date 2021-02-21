#include "VertexArrayLayout.hpp"

#include "glstuff.hpp"

namespace gl {
    void VertexArrayLayout::addAttribute(unsigned int count, GLenum type, bool normalized) {
        attributes.push_back({ indexesCounter++, count, type, normalized });
        stride += count * retrieveTypeSize(type);
    }
}