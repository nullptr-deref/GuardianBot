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

    class ArgumentParser
    {
    public:
        ArgumentParser() = default;

        void defineArgument(const std::string &shortArgName, const std::string &fullArgName);

        auto parseArgs(int argc, char **argv) -> Map<std::string, std::string>;

    private:
        int definedArgsCount = 0;

        struct ArgumentNames
        {
            std::string fullName;
            std::string shortName;
        };

        std::vector<ArgumentNames> argsNames;
    };
}