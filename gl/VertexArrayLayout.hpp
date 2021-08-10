#pragma once

#include <vector>
#include <tuple>

#include <GL/glew.h>

namespace gl {
    using Index = GLuint;
    using Type = GLenum;
    enum class Normalization : bool
    {
        Normalized = true,
        NotNormalized = false
    };
    // Attribute is a storage for neccessary vertex data:
    //                         | index | count       | type  | is item normalized
    using Attribute = std::tuple<Index, unsigned int, Type, Normalization>;
    class VertexArrayLayout {
    public:
        VertexArrayLayout() = default;

        void addAttribute(unsigned int count, Type type, Normalization normalized);

        std::vector<Attribute> attributes;
        unsigned int stride = 0;
    private:
        unsigned int indexesCounter_ = 0;
    };
}