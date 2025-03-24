// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template <typename T, typename U>
FORCEINLINE constexpr size_t OffsetOf(U T::*member)
{
#if WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnull-pointer-subtraction"
#endif /* WITH_GCC */
    return (char*)&((T*)nullptr->*member) - (char*)nullptr;
#if WITH_GCC
    #pragma GCC diagnostic pop
#endif /* WITH_GCC */
}

} /* ~Namespace Jafg. */
