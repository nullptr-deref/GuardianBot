#include "glstuff.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "Texture.hpp"
#include "Shader.hpp"
#include "Program.hpp"

namespace gl {
    GLFWwindow * createDefaultWindow(const std::string &windowName) {
        GLFWmonitor *pmonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *vmode = glfwGetVideoMode(pmonitor);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_TRUE);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        GLFWwindow *wnd = glfwCreateWindow(vmode->width - 100, vmode->height - 100, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowPos(wnd, 50, 50);

        return wnd;
    }

    void loadCVmat2GLTexture(const Texture &tex, const cv::Mat& image, bool shouldFlip)
    {
        const cv::Mat copied = image.clone();
        if(copied.empty()) std::cerr << "Image is empty.\n";
        else
        {
            tex.bind();

            cv::Mat processed;
            if (shouldFlip) cv::flip(copied, processed, 0);
            else copied.copyTo(processed);
            
            glTexImage2D(tex.getType(),
                        0,
                        GL_RGB,
                        processed.cols,
                        processed.rows,
                        0,
                        GL_BGR,
                        GL_UNSIGNED_BYTE,
                        processed.data
            );
            tex.unbind();
        }
    }

    Program loadDefaultShaders() {
        namespace fs = std::filesystem;
        const fs::path vertfp = fs::path("resources/VertexDefault.shader");
        Shader vertex(ShaderType::Vertex, Shader::parseFromFile(vertfp.string()));
        const bool isVertexReady = vertex.compile();

        const fs::path fragfp = fs::path("resources/FragmentDefault.shader");
        Shader frag(ShaderType::Fragment, Shader::parseFromFile(fragfp.string()));
        const bool isFragReady = frag.compile();

        Program p;
        p.attachShader(vertex);
        p.attachShader(frag);
        const bool li = p.link();
        const bool v = p.validate();
        p.detachShader(vertex);
        p.detachShader(frag);
        std::clog << p.getInfoLog() << '\n';
        const bool isProgReady = v && li;

        if (!(isVertexReady && isFragReady && isProgReady)) throw std::runtime_error("Default shaders could not be loaded.");

        return p;
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