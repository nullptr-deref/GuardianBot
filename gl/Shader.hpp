#pragma once

#include <filesystem>
#include <string>

#include <GL/glew.h>

namespace gl {
    using ShaderID = GLuint;
    enum class ShaderType : GLenum
    {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER
    };

    class Shader {
        ShaderID id;
    public:
        Shader(ShaderType type);
        explicit Shader(ShaderType type, const std::string &src);

        static std::string parseFromFile(const std::filesystem::path &filepath);

        void setSource(const std::string &src);
        bool compile();
        ShaderID getID() const;
    };
}