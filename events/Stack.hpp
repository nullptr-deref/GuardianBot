#pragma once

#include <memory>

#include "Event.hpp"

namespace events
{
    class Stack
    {
        struct Node
        {
            Node(const Event &e) : ev(e), prev(nullptr) {}

            Event ev;
            std::unique_ptr<Node> prev;
        };

        std::unique_ptr<Node> top;
    public:
        Stack() = default;

        void push(const Event &e)
        {
            std::unique_ptr<Node> temp = std::make_unique<Node>(e);
            if (top) temp->prev = std::move(top);
            top = std::move(temp);
        }
        Event pull()
        {
            if (top)
            {
                const Event dequeuedEvent = top->ev;
                top = std::move(top->prev);

                return dequeuedEvent;
            }
        }

        // void listAllElements()
        // {
        //     Node *current = top.get();

        //     while (current->prev != nullptr)
        //     {
        //         std::cout << current->ev.is() << '\n';
        //         current = current->prev.get();
        //     }
        //     std::cout << current->ev.is() << '\n';
        // }
    };
}