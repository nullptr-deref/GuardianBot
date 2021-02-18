#pragma once

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <opencv2/imgproc.hpp>

#include <type_traits>

namespace meta
{
    template <typename T1, typename T2>
    struct IsSame : std::false_type {};

    template <typename T>
    struct IsSame<T, T> : std::true_type {};
}

namespace gl {
    class Texture;
    GLFWwindow * createDefaultWindow();
    void loadCVmat2GLtexture(const Texture &texture, cv::Mat &image, bool shouldFlip = false);
    std::string parseShader(const std::string &filename);
    GLuint compileShader(GLenum type, const std::string &src);
    GLuint loadDefaultShaders();
    GLuint retrieveTypeSize(GLenum type);

    template <typename T>
    class VertexBuffer {
    public:
        VertexBuffer() {
            glGenBuffers(1, &id);
        }
        explicit VertexBuffer(const T *data, unsigned int count, GLenum usage) {
            glGenBuffers(1, &id);
            glNamedBufferData(id, count * sizeof(T), data, usage);
        }
        ~VertexBuffer() {
            glDeleteBuffers(1, &id);
            this->unbind();
        }

        void bind() const {
            glBindBuffer(GL_ARRAY_BUFFER, id);
        }
        void unbind() const {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    private:
        GLuint id;
    };

    using Attribute = std::tuple<GLuint, unsigned int, GLenum, bool>;
    class VertexArrayLayout {
    public:
        VertexArrayLayout() : stride(0) {};

        void addAttribute(unsigned int count, GLenum type, bool normalized);

        std::vector<Attribute> attributes;
        unsigned int stride;
    private:
        unsigned int indexesCounter = 0;
    };

    class VertexArray {
    public:
        VertexArray();
        ~VertexArray();

        void setLayout(const VertexArrayLayout &l);
        void enableAttribute(unsigned int index);
        void disableAttribute(unsigned int index);

        void bind() const;
        void unbind() const;

    private:
        GLuint id;
    };

    template <typename T>
    class IndexBuffer {
    public:
        IndexBuffer() {
            glGenBuffers(1, &id);
            this->bind();
        }
        explicit IndexBuffer(const T *indices, unsigned int count, GLenum usage) {
            glGenBuffers(1, &id);
            this->bind();
            glBufferData(id, count * sizeof(T), (const void *)indices, usage);
        }
        
        void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); }
        void unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
    private:
        GLuint id;
        T *indices;
        unsigned int count;
    };

    class Texture {
    public:
        Texture(GLenum type) : type(type) {
            glGenTextures(1, &id);
            this->bind();
        }
        ~Texture() noexcept {
            this->unbind();
            glDeleteTextures(1, &id);
        }
        
        template <typename Attr>
        void setAttribute(GLenum attr, Attr val) {
            if constexpr (meta::IsSame<Attr, int>()) glTexParameteri(type, attr, val);
        }

        GLenum getType() const { return type; }

        void bind() const { glBindTexture(type, id); }
        void unbind() const { glBindTexture(type, 0); }
    private:
        GLuint id;
        GLenum type;
    };
}