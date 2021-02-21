#pragma once

#include <vector>
#include <tuple>

#include <GL/glew.h>

namespace gl {
    using Attribute = std::tuple<GLuint, unsigned int, GLenum, bool>;
    class VertexArrayLayout {
    public:
        VertexArrayLayout() : stride(0) {};

        void addAttribute(unsigned int count, GLenum type, bool normalized);

        std::vector<Attribute> attributes;
        unsigned int stride;
    private:
        unsigned int indexesCounter = 0;
    };
}