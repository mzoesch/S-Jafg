// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template <typename T, typename U>
FORCEINLINE constexpr size_t OffsetOf(U T::*member)
{
#if PLATFORM_WASM
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnull-pointer-subtraction"
#endif /* PLATFORM_WASM */
    return (char*)&((T*)nullptr->*member) - (char*)nullptr;
#if PLATFORM_WASM
    #pragma GCC diagnostic pop
#endif /* PLATFORM_WASM */
}

} /* ~Namespace Jafg. */
