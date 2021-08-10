#include "VertexArrayLayout.hpp"

#include "glstuff.hpp"

namespace gl {
    void VertexArrayLayout::addAttribute(unsigned int count, Type type, Normalization normalized) {
        attributes.push_back({ indexesCounter_++, count, type, normalized });
        stride += count * retrieveTypeSize(type);
    }
}