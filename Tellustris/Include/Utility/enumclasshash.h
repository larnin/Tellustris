#ifndef ENUMCLASSHASH_H
#define ENUMCLASSHASH_H

#include <cstddef> //size_t

struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

#endif // ENUMCLASSHASH_H
