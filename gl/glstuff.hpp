#pragma once

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <opencv2/imgproc.hpp>

namespace gl {
    class Texture;
    GLFWwindow * createDefaultWindow();
    void loadCVmat2GLTexture(const Texture &texture, cv::Mat &image, bool shouldFlip = false);
    std::string parseShader(const std::string &filename);
    GLuint compileShader(GLenum type, const std::string &src);
    GLuint loadDefaultShaders();
    GLuint retrieveTypeSize(GLenum type);
}