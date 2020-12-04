#include "ArgumentParser.hpp"

#include <iostream>

void cli::ArgumentParser::defineArgument(const std::string &shortArgName, const std::string &fullArgName)
{
    definedArgsCount++;

    argsNames.push_back({
        fullArgName.substr(0, 2) == "--" ? fullArgName.substr(2) : fullArgName,
        shortArgName[0] == '-' ? shortArgName.substr(1) : shortArgName
    });
}

auto cli::ArgumentParser::parseArgs(int argc, char **argv) -> Map<std::string, std::string>
{
    Map<std::string, std::string> parsedArgs;

    Array<std::string> providedArgs(argc);
    for (size_t i = 0; i < argc; i++)
    {
        providedArgs[i] = argv[i];
    }

    if (definedArgsCount*2 + 1 != argc)
    {
        throw std::runtime_error("Required command line arguments were not provided.");
    }

    parsedArgs.insert({ "applicationName", providedArgs[0] });

    for (size_t i = 1; i < argc; i++)
    {
        for (size_t j = 0; j < argsNames.size(); j++)
        {
            if (providedArgs[i] == "--" + argsNames[j].fullName || providedArgs[i] == "-" + argsNames[j].shortName)
            {
                parsedArgs.insert({ argsNames[j].fullName, providedArgs[i + 1] });
                i += 1;
            }
        }
    }
    
    return parsedArgs;
}