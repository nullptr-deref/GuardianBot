#pragma once

#include <memory>

#include "Queue.hpp"

namespace events
{
    class Emitter
    {
        std::shared_ptr<Queue> m_qPtr;
    public:
        Emitter(Queue &q) : m_qPtr(std::make_shared<Queue>(q)) {}
    
        void emit(const Event &e) { m_qPtr->enqueue(e); }
        std::shared_ptr<Queue> provideQueueLink() const { return m_qPtr; }
    };
}