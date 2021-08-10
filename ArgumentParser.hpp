#pragma once

#include <string>
#include <vector>

#include "Array.hpp"
#include "Map.hpp"

namespace cli
{
    struct Argument
    {
        std::string name;
        std::string value;
    };
    enum class Requirement : bool
    {
        Required = true,
        NotRequired = false
    };

    class ArgumentParser
    {
    public:
        explicit ArgumentParser(size_t argc) : m_args(argc) {}

        void arg(const std::string &shortName, const std::string &fullName, Requirement required = Requirement::Required);

        auto parseArgs(int argc, char **argv) -> Map<std::string, std::string>;

    private:
        struct ArgumentMeta
        {
            std::string shortName;
            std::string fullName;
        };

        size_t m_reqArgc = 0;
        Array<ArgumentMeta> m_args;
        std::vector<ArgumentMeta> m_notReqArgs;
    };
}