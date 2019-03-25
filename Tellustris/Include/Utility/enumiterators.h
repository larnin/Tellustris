#ifndef ENUMITERATORS_H
#define ENUMITERATORS_H

#include <type_traits>

template <typename E, typename = std::enable_if<std::is_enum<E>::value>>
struct EnumIterator
{
    E value;

    EnumIterator<E>& operator++()
    {
        value = static_cast<E>(std::underlying_type_t<E>(value) + 1);
        return *this;
    }

    E operator*()
    {
        return value;
    }

    bool operator==(const EnumIterator<E> & e) const
    {
        return e.value == value;
    }

    bool operator!=(const EnumIterator<E> & e) const
    {
        return !(*this == e);
    }
};

template<typename E, typename = std::enable_if<std::is_enum<E>::value>>
EnumIterator<E> begin(E)
{
    return EnumIterator<E>{static_cast<E>(std::underlying_type_t<E>(0))};
}

template<typename E, typename = std::enable_if<std::is_enum<E>::value>>
EnumIterator<E> end(E max)
{
    return EnumIterator<E>{static_cast<E>(std::underlying_type_t<E>(max) + 1)};
}

#endif // ENUMITERATORS_H
