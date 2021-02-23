#include "Program.hpp"
#include "GLmisc.hpp"
namespace gl {
    Program::Program() {
        id = glCreateProgram();
    }
    void Program::attachShader(const Shader &shader) {
        glAttachShader(id, shader.getID());
    }
    void Program::detachShader(const Shader &shader) {
        glDetachShader(id, shader.getID());
    }
    bool Program::link() const {
        glLinkProgram(id);
        int res;
        glGetProgramiv(id, GL_LINK_STATUS, &res);

        return static_cast<bool>(res);
    }
    bool Program::validate() const {
        glValidateProgram(id);
        int res;
        glGetProgramiv(id, GL_VALIDATE_STATUS, &res);

        return static_cast<bool>(res);
    }
    std::string Program::getInfoLog() const {
        static const uint32_t bufSize = 256u;
        GLchar log[bufSize];
        int32_t length;
        glGetProgramInfoLog(id, bufSize, &length, log);

        std::string res;
        for (int32_t i = 0; i < length; i++) res += log[i];

        return res;
    }

    void Program::use() const { glUseProgram(id); }
    void Program::stopUse() const { glUseProgram(0); }
    GLuint Program::getID() const { return id; }
}