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

        struct ArgumentName
        {
            std::string fullName;
            std::string shortName;
        };

        std::vector<ArgumentName> argsNames;
    };
}

namespace cli::experimental
{
    template <uint32_t ReqArgC>
    class ArgumentParser
    {
    public:
        ArgumentParser() = default;

        void defineArgument(const std::string &shortName, const std::string &fullName, bool required = true)
        {
            std::string shortNameTrimmed = shortName[0] == '-' ? shortName.substr(1) : shortName;
            std::string fullNameTrimmed = fullName.substr(0, 2) == "--" ? fullName.substr(2) : fullName;

            if (required)
            {
                m_args[m_reqArgc] = { shortNameTrimmed, fullNameTrimmed };
                m_reqArgc++;
            }
            else
            {
                m_notReqArgs.push_back({ shortNameTrimmed, fullNameTrimmed });
            }
        }

        auto parseArgs(int argc, char **argv) -> Map<std::string, std::string>
        {
            if (m_reqArgc == ReqArgC && argc - 1 >= ReqArgC * 2)
            {
                Array<std::string> providedArgs(argc);
                for (size_t i = 0; i < argc; i++)
                {
                    providedArgs[i] = argv[i];
                }

                Map<std::string, std::string> parsedArgs;
                for (size_t i = 0; i < providedArgs.size; i++)
                {
                    for (const auto &argMeta : m_args)
                    {
                        if ('-' + argMeta.shortName == providedArgs[i] || "--" + argMeta.fullName == providedArgs[i])
                        {
                            parsedArgs.insert({ argMeta.fullName,  providedArgs[i + 1]});
                            i++;
                        }
                    }
                }

                return parsedArgs;
            }
            else
            {
                std::cerr << "Required command line arguments were not provided.\n";
                std::terminate();
            }
        }
    
    private:
        struct ArgumentMeta
        {
            std::string shortName;
            std::string fullName;
        };

        uint32_t m_reqArgc = 0;
        Array<ArgumentMeta, ReqArgC> m_args;
        std::vector<ArgumentMeta> m_notReqArgs;
    };
}