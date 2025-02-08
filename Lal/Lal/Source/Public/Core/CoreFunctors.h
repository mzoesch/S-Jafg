// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template <typename T>
FORCEINLINE void Swap(T& A, T& B)
{
    T Temp = std::move(A);
      A    = std::move(B);
      B    = std::move(Temp);

    return;
}

/**
 * @return The arg count of a function.
 */
template <typename F, typename ... ArgTy>
constexpr LSizeTy ArgTyCount(F(*f)(ArgTy ...))
{
    return sizeof ... (ArgTy);
}

FORCEINLINE bool Xor(const bool A, const bool B)
{
    return (A + B) % 2;
}

} /* ~Namespace Jafg */
