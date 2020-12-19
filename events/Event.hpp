#pragma once

#include <string>

namespace events
{
    class Event
    {
        std::string m_type;
    public:
        Event(const std::string &type) : m_type(type) {}
        Event(std::string &&type) : m_type(std::move(type)) {}

        auto is() const -> const std::string { return m_type; }
    };
}