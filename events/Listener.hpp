#pragma once

#include <functional>
#include <map>

#include "Emitter.hpp"

namespace events
{
    class Listener
    {
        std::shared_ptr<Queue> m_queue = nullptr;
        std::map<std::string, std::function<void(void)>> m_callbackRouter;
    public:
        Listener() = default;

        void listenTo(const Emitter &em);
        void subscribeForEvent(const Event &ev, const std::function<void()> &callback);
        bool processEventIfSubscribed();
        Event &peekFirstEvent() const;
        Event &peekLatestEvent() const;
    };
}