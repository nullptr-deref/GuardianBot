#pragma once

#include <cstddef>

template <typename T1, typename T2>
struct Pair
{
    T1 first;
    T2 second;
};

template <typename KeyType, typename ValueType>
class Map
{
public:
    Map() : m_head(nullptr), m_tail(nullptr) {}

    auto size() -> size_t { return m_size; }

    auto insert(const Pair<KeyType, ValueType> &p) -> void
    {
        Node<KeyType, ValueType> toInsert(p.first, p.second);

        if (m_head != nullptr)
        {
            if (toInsert.key < m_tail->key)
            {
                Node<KeyType, ValueType> *currentCompared = m_head;
                
                while (currentCompared != m_tail)
                {
                    if (toInsert.key > currentCompared->key && toInsert.key < currentCompared->next->key)
                    {
                        Node<KeyType, ValueType> *insertion = new Node<KeyType, ValueType>(toInsert.key, toInsert.value);

                        currentCompared->next->prev = insertion;
                        insertion->next = currentCompared->next;
                        insertion->prev = currentCompared;
                        currentCompared->next = insertion;

                        m_size++;
                        break;
                    }
                    else if (toInsert.key == currentCompared->key)
                    {
                        currentCompared->value = toInsert.value;
                        break;
                    }

                    currentCompared = currentCompared->next;
                }
            }
            else if (toInsert.key == m_tail->key)
            {
                m_tail->value = toInsert.value;
            }
            else
            {
                Node<KeyType, ValueType> *insertion = new Node<KeyType, ValueType>(toInsert.key, toInsert.value);
                m_tail->next = insertion;
                insertion->prev = m_tail;
                m_tail = insertion;

                m_size++;
            }
        }
        else
        {
            Node<KeyType, ValueType> *insertion = new Node<KeyType, ValueType>(toInsert.key, toInsert.value);
            m_head = insertion;
            m_tail = m_head;
            
            m_size++;
        }
    }

    ValueType &operator[](const KeyType &key)
    {
        Node<KeyType, ValueType> *current = m_head;

        while (current->key != key)
        {
            current = current->next;
        }

        if (current == m_tail && current->key != key)
        {
            throw std::invalid_argument("No item in list with provided key.");
        }

        return current->value;
    }

    const ValueType &operator[](const KeyType &key) const
    {
        Node<KeyType, ValueType> *current = m_head;

        while (current->key != key)
        {
            current = current->next;
        }

        if (current == m_tail && current->key != key)
        {
            throw std::invalid_argument("No item in list with provided key.");
        }

        return current->value;
    }

    void printAllNodes() const
    {
        Node<KeyType, ValueType> *curr = m_head;

        while (curr != m_tail)
        {
            std::cout << "[" << curr->key << "]{" << curr->value << "}\n";
            curr = curr->next;
        }
        std::cout << "[" << m_tail->key << "]{" << m_tail->value << "}\n";
    }

private:
    template <typename KeyT, typename ValT>
    struct Node
    {
        Node() = default;
        Node(KeyT key, ValT value) : key(key), value(value) {}
        ~Node()
        {
            if (nullptr != next)
            {
                delete next;
                next = nullptr;
            }
            if (nullptr != prev)
            {
                delete prev;
                prev = nullptr;
            }
        }

        KeyT key;
        ValT value;

        Node<KeyT, ValT> *next = nullptr;
        Node<KeyT, ValT> *prev = nullptr;
    };
    
    Node<KeyType, ValueType> *m_head = nullptr;
    Node<KeyType, ValueType> *m_tail = nullptr;

    size_t m_size = 0;
};