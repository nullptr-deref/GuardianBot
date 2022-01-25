#pragma once

#include <type_traits>

#include <GL/glew.h>


namespace gl {
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
        void setAttr(GLenum attr, Attr val) {
            if constexpr (std::is_same<Attr, int>()) glTexParameteri(type, attr, val);
        }

        GLenum getType() const { return type; }

        void bind() const { glBindTexture(type, id); }
        void unbind() const { glBindTexture(type, 0); }
    private:
        GLuint id;
        GLenum type;
    };
}