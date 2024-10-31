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

} /* ~Namespace Jafg */