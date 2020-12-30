#include "Listener.hpp"

namespace events
{
    void Listener::listenTo(const Emitter &em)
    {
        this->m_qPtr = em.provideQueueLink();
    }

    Event &Listener::peekFirstEvent() const
    {
        return m_qPtr->peekFirst();
    }

    Event &Listener::peekLatestEvent() const
    {
        return m_qPtr->peekLatest();
    }
}