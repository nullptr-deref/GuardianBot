#include "Shader.hpp"

#include <string>
#include <fstream>
#include <sstream>

namespace gl {
    Shader::Shader(GLenum type) {
        id = glCreateShader(type);
    }
    Shader::Shader(GLenum type, const std::string &src) {
        id = glCreateShader(type);
        this->setSource(src);
    }
    void Shader::setSource(const std::string &src) {
        const char *raw = src.c_str();
        glShaderSource(id, 1, &raw, nullptr);
    }
    bool Shader::compile() {
        glCompileShader(id);
        int res;
        glGetShaderiv(id, GL_COMPILE_STATUS, &res);

        return static_cast<bool>(res);
    }
    GLuint Shader::getID() const { return id; }

    std::string Shader::parseFromFile(const std::string &filepath) {
        std::ifstream inp(filepath);
        std::string buf;
        std::stringstream ss;

        while(std::getline(inp, buf)) ss << buf << '\n';

        return ss.str();
    }
}