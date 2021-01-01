#pragma once

#include <memory>

#include "Queue.hpp"

namespace events
{
    class Emitter
    {
        std::shared_ptr<Queue> m_queue;
    public:
        Emitter(Queue &q) : m_queue(std::make_shared<Queue>(q)) {}
    
        void emit(const Event &e) { m_queue->enqueue(e); }
        std::shared_ptr<Queue> provideQueueLink() const { return m_queue; }
    };
}