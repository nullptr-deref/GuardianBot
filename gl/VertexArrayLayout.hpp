#pragma once

#include <vector>
#include <tuple>

#include <GL/glew.h>

namespace gl {
    // Attribute is a storage for neccessary vertex data:
    //                         | index | count       | type  | is item normalized
    using Attribute = std::tuple<GLuint, unsigned int, GLenum, bool>;
    class VertexArrayLayout {
    public:
        VertexArrayLayout() = default;

        void addAttribute(unsigned int count, GLenum type, bool normalized);

        std::vector<Attribute> attributes;
        unsigned int stride = 0;
    private:
        unsigned int indexesCounter = 0;
    };
}