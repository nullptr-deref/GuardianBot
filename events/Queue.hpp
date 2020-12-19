#pragma once

#include <memory>

#include "Event.hpp"

namespace events
{
    class Queue
    {
        struct Node
        {
            Node(const Event &e) : ev(e), later(nullptr), earlier(nullptr) {}

            Event ev;
            std::shared_ptr<Node> earlier;
            std::shared_ptr<Node> later;
        };

        std::shared_ptr<Node> latest;
        std::shared_ptr<Node> first;

    public:
        Queue() : first(nullptr), latest(nullptr) {}

        void enqueue(const Event &e)
        {
            std::shared_ptr<Node> temp = std::make_shared<Node>(e);
            if (!first)
            {
                first = std::move(temp);
                latest = first;
            }
            else
            {
                temp->earlier = std::move(latest);
                latest = std::move(temp);
                latest->earlier->later = latest;
            }
        }

        Event dequeue()
        {
            const Event earliest = first->ev;
            first = first->later;

            return earliest;
        }

        // void listAllElements()
        // {
        //     Node *current = first.get();

        //     while (current->later != nullptr)
        //     {
        //         std::cout << current->ev.is() << '\n';
        //         current = current->later.get();
        //     }
        //     std::cout << current->ev.is() << '\n';
        // }
    };
}