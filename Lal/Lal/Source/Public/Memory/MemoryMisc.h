// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template <typename T, typename U>
FORCEINLINE constexpr size_t OffsetOf(U T::*member)
{
#if LAL_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wnull-pointer-subtraction"
#endif /* LAL_WITH_CLANG */
    return (char*)&((T*)nullptr->*member) - (char*)nullptr;
#if LAL_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* LAL_WITH_CLANG */
}


} /* ~Namespace Jafg */
