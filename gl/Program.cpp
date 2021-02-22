#include "Program.hpp"
#include "GLmisc.hpp"
namespace gl {
    Program::Program() {
        id = glCreateProgram();
    }
    Program::~Program() {
        glUseProgram(0);
        glDeleteProgram(id);
    }
    void Program::attachShader(const Shader &shader) {
        glAttachShader(id, shader.getID());
    }
    bool Program::link() {
        glLinkProgram(id);
        int res;
        glGetProgramiv(id, GL_LINK_STATUS, &res);

        return static_cast<bool>(res);
    }
    bool Program::validate() {
        glValidateProgram(id);
        int res;
        glGetProgramiv(id, GL_VALIDATE_STATUS, &res);

        return static_cast<bool>(res);
    }
    std::string Program::getInfoLog() {
        static const uint32_t bufSize = 256u;
        GLchar log[bufSize];
        int32_t length;
        glGetProgramInfoLog(id, bufSize, &length, log);

        std::string res;
        for (uint32_t i = 0; i < length; i++) res += log[i];

        return res;
    }

    void Program::use() const { glUseProgram(id); }
    void Program::stopUse() const { glUseProgram(0); }
    GLuint Program::getID() const { return id; }
}