#pragma once

#include <concepts>
#include <map>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <unordered_map>

#include "Exception.hpp"

template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;
template <typename T>
concept Extractable = Numeric || std::same_as<T, std::string>;

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

        template <Extractable ExtractedType>
        auto get() -> ExtractedType {
            switch(this->type_) {
                case ArgType::Number:
                    if constexpr (std::is_floating_point<ExtractedType>())
                        return std::atof(raw_);
                    if constexpr (std::is_integral<ExtractedType>())
                        return std::atoi(raw_);
                    break;
                case ArgType::List:
                    // TODO: just create some implementation already
                    return raw_;
                    break;
                case ArgType::String:
                    if constexpr (std::is_same<ExtractedType, std::string>())
                        return raw_;
                    break;
                case ArgType::Flag:
                    if constexpr (std::is_integral<ExtractedType>())
                        return true;
            }

            return raw_;
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
    auto operator==(const ArgRecord &lhs, const ArgRecord &rhs) -> bool;

    using ArgMap = std::map<std::string, ArgData>;
    class ArgumentParser {
    public:
        ArgumentParser() = default;
        auto arg(ArgType &&type, ArgName &&name) -> void;

        auto parse(int argc, char **argv) -> ArgMap;

    private:
        auto extractArguments(int argCount, char **argv) -> std::vector<std::string>;
        auto isArgName(const std::string_view &token) -> bool;
        auto searchRegisteredArgRecord(const std::string &token) -> ArgRecord;
        auto isDuplicate(const ArgRecord &rec, const std::map<std::string, ArgData> &prevArgs) -> bool;

        auto registerArg(const ArgRecord &rec) -> void;
        auto isEntryAvailable(const ArgRecord &rec) -> bool;

        std::vector<ArgRecord> registered_;
    };
}