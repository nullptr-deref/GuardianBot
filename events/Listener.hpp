#pragma once

#include "Emitter.hpp"

namespace events
{
    class Listener
    {
        std::shared_ptr<Queue> m_qPtr = nullptr;
    public:
        Listener() = default;

        Event &peekFirstEvent() const;
        Event &peekLatestEvent() const;
        void listenTo(const Emitter &em);
    };
}