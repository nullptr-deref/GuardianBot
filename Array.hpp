#include <iostream>

template <typename T>
class Array
{
public:
    Array() = default;
    Array(size_t s) : size(s)
    {
        data = new T[size];
    }
    explicit Array(T *dat, size_t s) : size(s)
    {
        data = new T[size];
        for (unsigned int i = 0; i < size; i++)
        {
            data[i] = dat[i];
        }
    }

    Array(const Array &other)
    {
        this->size = other.size;
        this->data = new T[size];

        for (unsigned int i = 0; i < size; i++)
        {
            data[i] = other[i];
        }
    }

    T &operator[](size_t index)
    {
        if (index >= size || index < 0) throw std::invalid_argument("Attempt to access out-of-range array element.");

        return data[index];
    }

    const T &operator[](size_t index) const
    {
        if (index >= size || index < 0) throw std::invalid_argument("Attempt to access out-of-range array element.");

        return data[index];
    }

    Array &operator=(const Array &other)
    {
        if (nullptr != data)
        {
            delete[] data;
            data = nullptr;
        }

        this->size = other.size;
        this->data = new T[size];

        for (unsigned int i = 0; i < size; i++)
        {
            data[i] = other[i];
        }

        return *this;
    }

    ~Array()
    {
        delete[] data;
        data = nullptr;
    }

    unsigned int size = 0;
    T *data = nullptr;
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