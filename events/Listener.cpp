#include "Listener.hpp"

namespace events
{
    void Listener::listenTo(const Emitter &em)
    {
        this->m_queue = em.provideQueueLink();
    }

    Event &Listener::peekFirstEvent() const
    {
        return m_queue->peekFirst();
    }

    Event &Listener::peekLatestEvent() const
    {
        return m_queue->peekLatest();
    }

    void Listener::subscribeForEvent(const Event &ev, const std::function<void()> &callback)
    {
        m_callbackRouter.insert({ ev.is(), callback });
    }

    bool Listener::processEventIfSubscribed()
    {
        Event & ev = m_queue->peekFirst();
        if (auto it = m_callbackRouter.find(ev.is()); it != m_callbackRouter.end())
        {
            it->second();
            m_queue->dequeue();

            return true;
        }

        return false;
    }
}