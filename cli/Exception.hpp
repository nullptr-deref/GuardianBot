#pragma once

#include <exception>
#include <string>

namespace cli {
    class BasicException : public std::exception {
    public:
        explicit BasicException(const std::string &explanation) {
            this->expl_ = explanation;
        }
        explicit BasicException(std::string &&explanation) {
            this->expl_ = std::move(explanation);
        }
        const char *what() const noexcept override {
            return this->expl_.c_str();
        }

        ~BasicException() {}
    protected:
        std::string expl_;
    };

    using AmbiguousArgument = BasicException;
    using MultipleRegistry = BasicException;
    using DuplicateException = BasicException;
    using UnregisteredArgumentException = BasicException;
}
