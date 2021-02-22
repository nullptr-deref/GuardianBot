#include "Program.hpp"

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
}