#include "glstuff.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace gl {
    GLFWwindow * createDefaultWindow() {
        GLFWmonitor *pmonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *vmode = glfwGetVideoMode(pmonitor);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_TRUE);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        GLFWwindow *wnd = glfwCreateWindow(vmode->width - 100, vmode->height - 100, "Viewport", nullptr, nullptr);
        glfwSetWindowPos(wnd, 100, 100);

        return wnd;
    }

    void loadCVmat2GLtexture(GLuint &texture, cv::Mat& image, bool shouldFlip)
    {
        if(image.empty()) std::cerr << "Image is empty.\n";
        else
        {
            glBindTexture(GL_TEXTURE_2D, texture);

            cv::Mat processed;
            if (shouldFlip) cv::flip(image, processed, 0);
            else image.copyTo(processed);
            
            glTexImage2D(GL_TEXTURE_2D,
                        0,
                        GL_RGB,
                        processed.cols,
                        processed.rows,
                        0,
                        GL_BGR,
                        GL_UNSIGNED_BYTE,
                        processed.data
            );
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    std::string parseSahder(const std::string &filename) {
        std::ifstream inp(filename);
        std::string buf;
        std::stringstream ss;

        while(std::getline(inp, buf)) {
            if (buf.find("shader") != std::string::npos) continue;
            ss << buf << '\n';
        }

        return ss.str();
    }

    GLuint compileShader(GLenum type, const std::string &src) {
        // Here I'm using 2-step assignment to avoid undefined behaviour when c_str() of a std::string
        // will point to somewhere (I mean some temporary address which will expire till the end of the operand).
        // It also makes sense while using such a trick with fragment shader compilation.
        const char *srcRaw = src.c_str();
        GLuint s = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(s, 1, &srcRaw, nullptr);
        glCompileShader(s);

        return s;
    }

    GLuint loadDefaultShaders() {
        const std::string vertexShaderFilePath = std::filesystem::absolute("resources/VertexDefault.shader").string();
        const std::string vShaderSource = parseShader(vertexShaderFilePath);
        GLuint vertexShaderObj = compileShader(GL_VERTEX_SHADER, vShaderSource);
        int vShaderCompileStatus;
        glGetShaderiv(vertexShaderObj, GL_COMPILE_STATUS, &vShaderCompileStatus);

        const std::string fragmentShaderFilePath = std::filesystem::absolute("resources/FragmentDefault.shader").string();
        const std::string fShaderSource = parseShader(fragmentShaderFilePath);
        GLuint fragmentShaderObj = compileShader(GL_FRAGMENT_SHADER, fShaderSource);
        int fShaderCompileStatus;
        glGetShaderiv(fragmentShaderObj, GL_COMPILE_STATUS, &fShaderCompileStatus);

        GLuint prog = glCreateProgram();
        glAttachShader(prog, vertexShaderObj);
        glAttachShader(prog, fragmentShaderObj);
        glLinkProgram(prog);
        glValidateProgram(prog);
        int progLinkStatus;
        glGetProgramiv(prog, GL_LINK_STATUS, &progLinkStatus);

        if (!(vShaderCompileStatus && fShaderCompileStatus && progLinkStatus)) throw std::runtime_error("Default shaders could not be loaded");

        glDeleteShader(vertexShaderObj);
        glDeleteShader(fragmentShaderObj);

        return prog;
    }

    GLuint retrieveTypeSize(GLenum type) {
        GLuint ret = 0;
        switch(type) {
            case GL_UNSIGNED_BYTE: {
                ret = sizeof(GLbyte);
            } break;
            case GL_FLOAT: {
                ret = sizeof(GLfloat);
            } break;
            case GL_INT: {
                ret = sizeof(GLint);
            } break;
        }

        return ret;
    }
}