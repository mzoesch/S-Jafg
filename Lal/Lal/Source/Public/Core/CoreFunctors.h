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

template <bool Predicate, typename Ret = void>
class TEnableIf;
template <typename Ret> class TEnableIf<true, Ret>
{
public: using Ty = Ret;
};
template <typename Ret> class TEnableIf<false, Ret>
{
};
template <bool Predicate, typename Ret = void>
using TEnableIfTy = typename TEnableIf<Predicate, Ret>::Ty;

} /* ~Namespace Jafg */
