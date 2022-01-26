#include "ArgumentParser.hpp"

namespace cli {
    auto ArgumentParser::arg(ArgType &&type, ArgName &&name) -> void {
        this->registerArg({ .type = std::move(type), .name = std::move(name) });
    }

    auto ArgumentParser::parse(int argc, char **argv) -> ArgMap {
        ArgMap parsed;
        const auto tokens = this->extractArguments(argc, argv);
        for (int i = 0; i < argc; i++) {
            if (0 != i) {
                if (isArgName(tokens[i])) {
                    const auto record = searchRegisteredArgRecord(tokens[i]);
                    if (!isDuplicate(record, parsed)) {
                        switch(record.type) {
                            case ArgType::Flag:
                                parsed.insert_or_assign(record.name.fullName, ArgData(ArgType::Flag, ""));
                                break;
                            default:
                                parsed.insert_or_assign(record.name.fullName, ArgData(record.type, tokens[i + 1]));
                        }
                    }
                    else throw DuplicateException("Argument '" + record.name.fullName + "' provided more than once.");
                }
            }
            else {
                parsed.insert_or_assign("appName", ArgData(ArgType::AppName, tokens[i]));
            }
        }

        return parsed;
    }

    auto ArgumentParser::extractArguments(int argCount, char **argv) -> std::vector<std::string> {
        std::vector<std::string> args;
        for (int i = 0; i < argCount; i++)
            args.emplace_back(argv[i]);

        return args;
    }

    auto ArgumentParser::isArgName(const std::string_view &token) -> bool {
        return token.starts_with("-");
    }

    auto ArgumentParser::searchRegisteredArgRecord(const std::string &token) -> ArgRecord {
        const std::string trimmedToken = token.front() == '-' ? token.substr(token.find_first_not_of("-")) : token;
        const auto searchedArg = std::find_if(registered_.cbegin(), registered_.cend(),
            [&trimmedToken](const ArgRecord &rec) {
                return rec.name.fullName == trimmedToken ||
                    rec.name.shortName == trimmedToken;
            });
        if (searchedArg == registered_.cend())
            throw UnregisteredArgumentException("Argument with name '" + trimmedToken + "' is not registered.");

        return *searchedArg;
    }

    auto ArgumentParser::isDuplicate(const ArgRecord &rec, const std::map<std::string, ArgData> &prevArgs) -> bool {
        return prevArgs.contains(rec.name.fullName);
    }


    auto ArgumentParser::registerArg(const ArgRecord &rec) -> void {
        if (isEntryAvailable(rec)) {
            registered_.push_back(rec);
        }
        else throw MultipleRegistry("Argument with the same name registered more than once.");
    }

    auto ArgumentParser::isEntryAvailable(const ArgRecord &rec) -> bool {
        return std::find(registered_.cbegin(), registered_.cend(), rec) == registered_.cend();
    }
}

namespace cli {
    auto operator==(const ArgRecord &lhs, const ArgRecord &rhs) -> bool {
        return lhs.type == rhs.type &&
            lhs.name.fullName == rhs.name.fullName;
    }
}
