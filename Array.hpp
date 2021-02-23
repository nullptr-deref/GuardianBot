#include <iostream>
#include <iterator>

template <typename T>
class Array
{
public:
    Array() = delete;
    explicit Array(size_t s) : size(s)
    {
        m_data = new T[size];
    }

    explicit Array(T *dat, size_t s) : size(s)
    {
        m_data = new T[size];
        for (unsigned int i = 0; i < size; i++)
        {
            m_data[i] = dat[i];
        }
    }

    Array(const Array &other)
    {
        this->size = other.size;
        this->m_data = new T[size];

        for (unsigned int i = 0; i < size; i++)
        {
            m_data[i] = other[i];
        }
    }

    T &operator[](size_t index)
    {
        if (index >= size || index < 0) throw std::invalid_argument("Attempt to access out-of-range array element.");

        return m_data[index];
    }

    const T &operator[](size_t index) const
    {
        if (index >= size || index < 0) throw std::invalid_argument("Attempt to access out-of-range array element.");

        return m_data[index];
    }

    Array &operator=(const Array &other)
    {
        if (nullptr != m_data)
        {
            delete[] m_data;
            m_data = nullptr;
        }

        this->size = other.size;
        this->m_data = new T[size];

        for (unsigned int i = 0; i < size; i++) m_data[i] = other[i];

        return *this;
    }

    ~Array()
    {
        delete[] m_data;
        m_data = nullptr;
    }

    size_t size = 0;
    
    template <typename U>
    class ArrayIterator
    {
        size_t m_idx;
        size_t m_size;
        U *m_data;
    public:
        explicit ArrayIterator(size_t idx, U *data, size_t cSize) : m_idx(idx), m_data(data), m_size(cSize) {}
        ArrayIterator &operator++() { m_idx = m_idx >= m_size ? m_size : m_idx + 1; return *this; }
        ArrayIterator &operator--() { m_idx = m_idx <= 0 ? 0 : m_idx - 1; return *this; }
        
        bool operator==(ArrayIterator &other) { return m_idx == other.m_idx; }
        bool operator!=(ArrayIterator &other) { return !(m_idx == other.m_idx); }
        U operator*() { return m_data[m_idx]; }
    
        using difference_type = size_t;
        using value_type = U;
        using pointer = const U*;
        using reference = const U&;
        using iterator_category = std::random_access_iterator_tag;
    };

    ArrayIterator<T> begin() { return ArrayIterator<T>(0, m_data, size); }
    ArrayIterator<T> end() { return ArrayIterator<T>(size, m_data, size); }

private:
    T *m_data = nullptr;
};

template <typename T>
std::ostream &operator<<(std::ostream &out, const Array<T> &arr)
{
    out << "[ ";
    for (unsigned int i = 0; i < arr.size - 1; i++)
    {
        out << arr[i] << ", ";
    }
    out << arr[arr.size - 1] << " ]";

    return out;
}