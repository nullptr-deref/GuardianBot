#include "Shader.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace gl {
    Shader::Shader(ShaderType type) {
        id = glCreateShader(static_cast<GLenum>(type));
    }
    Shader::Shader(ShaderType type, const std::string &src) {
        id = glCreateShader(static_cast<GLenum>(type));
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
    gl::ShaderID Shader::getID() const { return id; }

    std::string Shader::parseFromFile(const std::filesystem::path &filepath) {
        std::ifstream inp(filepath);
        std::string buf;
        std::stringstream ss;

        if (inp.good()) {
            std::clog << "Found shader file '" << filepath.string() << "'\n";
            while(std::getline(inp, buf)) ss << buf << '\n';
        }
        else {
            throw std::runtime_error("'" + filepath.string() + "': no such file of directory.");
        }

        return ss.str();
    }
}