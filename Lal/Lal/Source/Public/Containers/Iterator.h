// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template <typename T>
struct Iterator
{
    using Self          = Iterator<T>;
    using ItCat         = std::forward_iterator_tag;
    using DiffTy        = std::ptrdiff_t;
    using ValTy         = T;
    using ValPtr        = ValTy*;
    using ValRef        = ValTy&;

    Iterator(ValPtr InPtr) : Ptr(InPtr)
    {
    }

    FORCEINLINE auto operator *() const -> ValRef { return *this->Ptr;                             }
    FORCEINLINE auto operator->() const -> ValPtr { return this->Ptr;                              }
    FORCEINLINE auto operator++()       -> Self&  { ++this->Ptr; return *this;                     }
    FORCEINLINE auto operator++(int32)  -> Self   { const Self Temp = *this; ++*this; return Temp; }
    friend FORCEINLINE auto operator==(const Self& Lhs, const Self& Rhs) -> bool { return Lhs.Ptr == Rhs.Ptr; }
    friend FORCEINLINE auto operator!=(const Self& Lhs, const Self& Rhs) -> bool { return !(Lhs == Rhs);      }

private:

    ValPtr Ptr = nullptr;
};

} /* ~Namespace Jafg. */
