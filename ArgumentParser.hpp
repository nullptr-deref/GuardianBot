#pragma once

#include <exception>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <unordered_map>

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

namespace cli {
    enum class ArgType {
        AppName,
        Flag,
        String,
        Number,
        List
    };

    class ArgData {
    public:
        ArgData() = default;
        ArgData(ArgType type, const std::string &rawToken)
            : type_(type), raw_(rawToken) {}
        ArgData(ArgType type, std::string &&rawToken)
            : type_(type), raw_(std::move(rawToken)) {}

        template <typename ExtractedType>
        auto get() -> ExtractedType {
            switch(this->type_) {
                case ArgType::String:
                    if constexpr (std::is_same<ExtractedType, std::string>())
                        return raw_;
                    break;
                case ArgType::Number:
                    if constexpr (std::is_integral<ExtractedType>())
                        return std::atoi(raw_);
                    if constexpr (std::is_floating_point<ExtractedType>())
                        return std::atof(raw_);
                    break;
                case ArgType::List:
                    // TODO: somehow handle this mess, I think it shouldn't work like this
                    return raw_;
                    break;
                case ArgType::Flag:
                    if constexpr (std::is_integral<ExtractedType>())
                        return true;
            }
        }
    private:
        ArgType type_;
        std::string raw_;
    };

    struct ArgName {
        std::string fullName;
        std::string shortName;
    };

    struct ArgRecord {
        ArgType type;
        ArgName name;
    };

    auto operator==(const ArgRecord &lhs, const ArgRecord &rhs) -> bool {
        return lhs.type == rhs.type &&
            lhs.name.fullName == rhs.name.fullName;
    }

    using ArgMap = std::map<std::string, ArgData>;
    class ArgumentParser {
    public:
        ArgumentParser() = default;
        auto arg(ArgType &&type, ArgName &&name) -> void {
            this->registerArg({ .type = std::move(type), .name = std::move(name) });
        }

        auto parse(int argc, char **argv) -> ArgMap {
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

        auto isDuplicate(const ArgRecord &rec, const std::map<std::string, ArgData> &prevArgs) -> bool {
            return prevArgs.contains(rec.name.fullName);
        }
    private:
        auto extractArguments(int argCount, char **argv) -> std::vector<std::string> {
            std::vector<std::string> args;
            for (int i = 0; i < argCount; i++)
                args.emplace_back(argv[i]);

            return args;
        }

        auto isArgName(const std::string_view &token) -> bool {
            return token.starts_with("-");
        }

        auto searchRegisteredArgRecord(const std::string &token) -> ArgRecord {
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

        // FIXME: possibly should be removed
        auto isArgRegistered(const ArgRecord &rec) -> bool {
            return !isEntryAvailable(rec);
        }

        auto registerArg(ArgRecord rec) -> void {
            if (isEntryAvailable(rec)) {
                registered_.push_back(rec);
            }
            else throw MultipleRegistry("Argument with the same name registered more than once.");
        }

        auto isEntryAvailable(const ArgRecord &rec) -> bool {
            return std::find(registered_.cbegin(), registered_.cend(), rec) == registered_.cend();
        }

        std::vector<ArgRecord> registered_;
    };
}