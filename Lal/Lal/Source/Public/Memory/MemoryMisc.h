// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template <typename T, typename U>
FORCEINLINE constexpr size_t OffsetOf(U T::*member)
{
    return (char*)&((T*)nullptr->*member) - (char*)nullptr;
}

} /* ~Namespace Jafg. */
