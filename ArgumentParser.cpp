#include "ArgumentParser.hpp"

#include <iostream>

void cli::ArgumentParser::defineArgument(const std::string &shortName, const std::string &fullName, bool required)
{
    const std::string shortNameTrimmed = shortName[0] == '-' ? shortName.substr(1) : shortName;
    const std::string fullNameTrimmed = fullName.substr(0, 2) == "--" ? fullName.substr(2) : fullName;

    if (required)
    {
        m_args[m_reqArgc++] = { shortNameTrimmed, fullNameTrimmed };
    }
    else m_notReqArgs.push_back({ shortNameTrimmed, fullNameTrimmed });
}

auto cli::ArgumentParser::parseArgs(int argc, char **argv) -> Map<std::string, std::string>
{
    Map<std::string, std::string> parsedArgs;

    Array<std::string> providedArgs(argc);
    for (size_t i = 0; i < argc; i++)
    {
        providedArgs[i] = argv[i];
    }

    if (m_reqArgc*2 + 1 != argc)
    {
        throw std::runtime_error("Required command line arguments were not provided.");
    }

    parsedArgs.insert({ "applicationName", providedArgs[0] });

    for (size_t i = 1; i < argc; i++)
    {
        for (size_t j = 0; j < m_args.size; j++)
        {
            if (providedArgs[i] == "--" + m_args[j].fullName || providedArgs[i] == "-" + m_args[j].shortName)
            {
                parsedArgs.insert({ m_args[j].fullName, providedArgs[i + 1] });
                i += 1;
            }
        }
    }
    
    return parsedArgs;
}