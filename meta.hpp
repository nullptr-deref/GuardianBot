#pragma once

#include <type_traits>

namespace meta {
    template <typename T1, typename T2>
    struct IsSame : std::false_type {};

    template <typename T>
    struct IsSame<T, T> : std::true_type {};
}