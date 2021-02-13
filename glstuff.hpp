#pragma once

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <opencv2/imgproc.hpp>

namespace gl {
    GLFWwindow * createDefaultWindow();
    void loadCVmat2GLtexture(GLuint &texture, cv::Mat &image, bool shouldFlip = false);
    std::string parseShader(const std::string &filename);
    GLuint compileShader(GLenum type, const std::string &src);
    GLuint loadDefaultShaders();
}