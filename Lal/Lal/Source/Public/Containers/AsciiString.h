// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/**
 * Jafg interpretation of a simple american standard code for information interchange string.
 * A string is mutable and can therefore be modified in place.
 * Strings are null-terminated and can therefore be used in C-style functions.
 */
class LAsciiString
{
    using SizeType = int32;
    /**
     * A rune is a single grapheme represented by a single byte.
     * Said grapheme has to be a valid ascii character.
     */
    using LRune = uint8;

    inline static LRune StringTerminatorRune = '\0';
    inline static char  StringTerminatorChar = '\0';

public:

    FORCEINLINE LAsciiString()                           noexcept;
    FORCEINLINE LAsciiString(LNullptrTy)                 noexcept;

    FORCEINLINE LAsciiString(      LAsciiString&  Other) noexcept;
    FORCEINLINE LAsciiString(const LAsciiString&  Other) noexcept;
    FORCEINLINE LAsciiString(      LAsciiString&& Other) noexcept;
    FORCEINLINE LAsciiString(const LAsciiString&& Other) noexcept = delete;
    FORCEINLINE LAsciiString(const LRune* Other)         noexcept;
    FORCEINLINE LAsciiString(const char* Other)          noexcept;
    FORCEINLINE explicit LAsciiString(const int32  Number)    noexcept;
    FORCEINLINE explicit LAsciiString(const int64  Number)    noexcept;
    FORCEINLINE explicit LAsciiString(const uint32 Number)    noexcept;
    FORCEINLINE explicit LAsciiString(const uint64 Number)    noexcept;

    FORCEINLINE auto operator=(const char*  Other)         noexcept -> LAsciiString&;
    FORCEINLINE auto operator=(const uint8* Other)         noexcept -> LAsciiString&;
    FORCEINLINE auto operator=(const LAsciiString&  Other) noexcept -> LAsciiString&;
    FORCEINLINE auto operator=(      LAsciiString&& Other) noexcept -> LAsciiString&;
    FORCEINLINE auto operator=(const LAsciiString&& Other) noexcept -> LAsciiString& = delete;

    FORCEINLINE operator       char *()       noexcept = delete;
    FORCEINLINE operator const char *() const noexcept = delete;
    FORCEINLINE operator       LRune*()       noexcept = delete;
    FORCEINLINE operator const LRune*() const noexcept = delete;

    FORCEINLINE ~LAsciiString()                          noexcept = default;

    FORCEINLINE auto Append(const char* Other)         -> void ;
    FORCEINLINE auto Append(const LAsciiString& Other) -> void ;

    /** Pop the last rune. */
    FORCEINLINE auto Pop(void) -> void;
    /** Peek at the lass rune. If the string is empty, the last entry will be returned. */
    FORCEINLINE auto Peek(void) const -> const LRune*;

    FORCEINLINE auto Reserve(const SizeType Size)     -> void { this->Data.Reserve(Size);           return; }
    /**
     * Empties the string and sets its size to zero. The memory buffer will not be deallocated or reallocated unless
     * the Size parameter is greater than the current capacity and growing the current memory buffer is not possible.
     * A null-terminator will be written to the first byte of the buffer.
     */
    FORCEINLINE auto Reset  (const SizeType Size)     -> void ;
    /**
     * Empties the string and sets its size to zero. The memory buffer will be deallocated.
     */
    FORCEINLINE auto Empty  (void)                    -> void ;
    FORCEINLINE auto IsEmpty(void) const              -> bool { return this->GetRuneCount() == 0;           }
    FORCEINLINE auto SwapStrings(LAsciiString& Other) -> void { this->Data.SwapBuffers(Other.Data); return; }

    /** Copy a string into another string. */
    FORCEINLINE auto CopyInto(LAsciiString& Other) const -> void { Other = *this;            return; }
    FORCEINLINE auto CopyFrom(const LAsciiString& Other) -> void { *this = Other;            return; }
    /** Move a string into another string. */
    FORCEINLINE auto MoveInto(LAsciiString& Other)       -> void { Other = std::move(*this); return; }
    FORCEINLINE auto MoveFrom(LAsciiString& Other)       -> void { *this = std::move(Other); return; }

    /** Get the size of the string in bytes. */
    FORCEINLINE auto GetSize(void) const -> SizeType { return this->Data.GetSize(); }
    /** Get the number of runes in the string (that excludes the null-terminator). */
    FORCEINLINE auto GetRuneCount(void) const -> SizeType { return Maths::Max(this->Data.GetSize() - 0x01, 0x00); }

    /** Interpret the string as a C-style string. */
    FORCEINLINE auto ToC()   const -> const char * ;
    FORCEINLINE auto ToPtr() const -> const LRune* ;

    FORCEINLINE auto GetRuneAt(const SizeType Index) const -> LRune { return this->Data[Index];                    }
    FORCEINLINE auto GetCharAt(const SizeType Index) const -> char  { return static_cast<char>(this->Data[Index]); }
    FORCEINLINE auto operator[](const SizeType Index)       ->       LRune& { return this->Data[Index]; }
    FORCEINLINE auto operator[](const SizeType Index) const -> const LRune& { return this->Data[Index]; }

    FORCEINLINE auto operator==(const LAsciiString& Other) const -> bool { return this->Data.IsDataEqual(Other.Data);   }
    FORCEINLINE auto operator!=(const LAsciiString& Other) const -> bool { return this->Data.IsDataUnequal(Other.Data); }
    FORCEINLINE auto operator <(const LAsciiString& Other) const -> bool { return this->Data  < Other.Data;             }
    FORCEINLINE auto operator >(const LAsciiString& Other) const -> bool { return this->Data  > Other.Data;             }
    FORCEINLINE auto operator<=(const LAsciiString& Other) const -> bool { return this->Data <= Other.Data;             }
    FORCEINLINE auto operator>=(const LAsciiString& Other) const -> bool { return this->Data >= Other.Data;             }
    FORCEINLINE auto operator+=(const LAsciiString& Other)       -> LAsciiString&;
    FORCEINLINE auto operator-=(const LAsciiString& Other)       -> LAsciiString& = delete;

    FORCEINLINE auto operator==(LNullptrTy)         const -> bool;
    FORCEINLINE auto operator==(const char * Other) const -> bool;
    FORCEINLINE auto operator==(const LRune* Other) const -> bool;
    FORCEINLINE auto operator!=(const char * Other) const -> bool { return !(*this == Other); }
    FORCEINLINE auto operator!=(const LRune* Other) const -> bool { return !(*this == Other); }
    FORCEINLINE auto operator <(const char * Other) const -> bool;
    FORCEINLINE auto operator <(const LRune* Other) const -> bool;
    FORCEINLINE auto operator >(const char * Other) const -> bool;
    FORCEINLINE auto operator >(const LRune* Other) const -> bool;
    FORCEINLINE auto operator<=(const char * Other) const -> bool { return !(*this > Other); }
    FORCEINLINE auto operator<=(const LRune* Other) const -> bool { return !(*this > Other); }
    FORCEINLINE auto operator>=(const char * Other) const -> bool { return !(*this < Other); }
    FORCEINLINE auto operator>=(const LRune* Other) const -> bool { return !(*this < Other); }
    FORCEINLINE auto operator+=(const char * Other)       -> LAsciiString& { this->Append(Other); return *this; }
    FORCEINLINE auto operator+=(const LRune* Other)       -> LAsciiString& { this->Append(Other); return *this; }

    FORCEINLINE auto Equals(const LAsciiString& Other) const -> bool { return *this == Other; }
    FORCEINLINE auto Equals(const char* Other)         const -> bool { return *this == Other; }
    FORCEINLINE auto Equals(const LRune* Other)        const -> bool { return *this == Other; }

    /** Private iterator functions for range-based loops. Do not use these directly. */
    FORCEINLINE auto begin()       noexcept -> Iterator<LRune>       ;
    FORCEINLINE auto begin() const noexcept -> Iterator<const LRune> ;
    FORCEINLINE auto end()         noexcept -> Iterator<LRune>       ;
    FORCEINLINE auto end()   const noexcept -> Iterator<const LRune> ;

    template <typename ... ArgyTy>
    static auto SprintF(const char* Format, const ArgyTy& ... Args) -> LAsciiString;

private:

    /**
     * Brings this string into a valid state after e.g., a complex operation (like moving or forwarding).
     * Ensures that a valid c-str pointer is available and the string is null-terminated.
     */
    FORCEINLINE auto EnsureValidState() -> void;

    TdhArray<LRune> Data;
};

} /* ~Namespace Jafg */

template <>
struct std::formatter<Jafg::LAsciiString> : std::formatter<const char*>
{
    FORCEINLINE auto format(
        const Jafg::LAsciiString& String,
        std::format_context&      Context
    ) const -> std::format_context::iterator
    {
        return std::formatter<const char*>::format(String.ToC(), Context);
    }
};

FORCEINLINE Jafg::LAsciiString::LAsciiString() noexcept
{
    check( this->Data.GetData() == nullptr )
}

FORCEINLINE Jafg::LAsciiString::LAsciiString(LNullptrTy) noexcept
{
    this->Data.Empty();
    check( this->Data.GetData() == nullptr )
    return;
}

FORCEINLINE Jafg::LAsciiString::LAsciiString(LAsciiString& Other) noexcept
{
    this->Data = Other.Data;

#if CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */

    return;
}

FORCEINLINE Jafg::LAsciiString::LAsciiString(const LAsciiString& Other) noexcept
{
    this->Data = Other.Data;

#if CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */

    return;
}

FORCEINLINE Jafg::LAsciiString::LAsciiString(LAsciiString&& Other) noexcept
{
    this->Data = std::forward<TdhArray<LRune>>(Other.Data);

#if CHECK_STRING_VALIDITY
    this->EnsureValidState();
    Other.EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */

    return;
}

FORCEINLINE Jafg::LAsciiString::LAsciiString(const LRune* Other) noexcept
{
    this->Data.Reset(1);

    while (true)
    {
        this->Data.Add(*Other);

        if (*Other++ == LAsciiString::StringTerminatorRune)
        {
            break;
        }

        continue;
    }

#if CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */

    return;
}

FORCEINLINE Jafg::LAsciiString::LAsciiString(const char* Other) noexcept
{
    this->Data.Reset(1);

    while (true)
    {
        this->Data.Add(*Other);

        if (*Other++ == LAsciiString::StringTerminatorChar)
        {
            break;
        }

        continue;
    }

#if CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */

    return;
}

FORCEINLINE Jafg::LAsciiString::LAsciiString(const int32 Number) noexcept
{
    /* Super sketchy, we will have to implement this somehow on our own. */
    const std::string NumberString = std::to_string(Number);

    this->Data.Reset(static_cast<SizeType>(NumberString.size()) + 1);
    this->Data.Add(LAsciiString::StringTerminatorRune);
    this->Append(NumberString.c_str());

    return;
}

FORCEINLINE Jafg::LAsciiString::LAsciiString(const int64 Number) noexcept
{
    const std::string NumberString = std::to_string(Number);

    this->Data.Reset(static_cast<SizeType>(NumberString.size()) + 1);
    this->Data.Add(LAsciiString::StringTerminatorRune);
    this->Append(NumberString.c_str());

    return;
}

FORCEINLINE Jafg::LAsciiString::LAsciiString(const uint32 Number) noexcept
{
    const std::string NumberString = std::to_string(Number);

    this->Data.Reset(static_cast<SizeType>(NumberString.size()) + 1);
    this->Data.Add(LAsciiString::StringTerminatorRune);
    this->Append(NumberString.c_str());

    return;
}

FORCEINLINE Jafg::LAsciiString::LAsciiString(const uint64 Number) noexcept
{
    const std::string NumberString = std::to_string(Number);

    this->Data.Reset(static_cast<SizeType>(NumberString.size()) + 1);
    this->Data.Add(LAsciiString::StringTerminatorRune);
    this->Append(NumberString.c_str());

    return;
}

FORCEINLINE Jafg::LAsciiString& Jafg::LAsciiString::operator=(const char* Other) noexcept
{
    this->Data.Reset(1);

    while (true)
    {
        this->Data.Add(*Other);

        if (*Other++ == LAsciiString::StringTerminatorChar)
        {
            break;
        }

        continue;
    }

    return *this;
}

FORCEINLINE Jafg::LAsciiString& Jafg::LAsciiString::operator=(const uint8* Other) noexcept
{
    this->Data.Reset(1);

    while (true)
    {
        this->Data.Add(*Other);

        if (*Other++ == LAsciiString::StringTerminatorRune)
        {
            break;
        }

        continue;
    }

    return *this;
}

FORCEINLINE Jafg::LAsciiString& Jafg::LAsciiString::operator=(const LAsciiString& Other) noexcept
{
    this->Data = Other.Data;

#if CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */

    return *this;
}

FORCEINLINE Jafg::LAsciiString& Jafg::LAsciiString::operator=(LAsciiString&& Other) noexcept
{
    this->Data = std::forward<TdhArray<LRune>>(Other.Data);

#if CHECK_STRING_VALIDITY
    this->EnsureValidState();
    Other.EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */

    return *this;
}

FORCEINLINE void Jafg::LAsciiString::Append(const char* Other)
{
#if CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */

    if (this->Data.IsData())
    {
        /* Null-terminator. */
        this->Data.Pop();
    }

    while (true)
    {
        this->Data.Add(*Other);

        if (*Other++ == LAsciiString::StringTerminatorChar)
        {
            break;
        }

        continue;
    }

#if CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */

    return;
}

FORCEINLINE void Jafg::LAsciiString::Append(const LAsciiString& Other)
{
#if CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */

    if (this->Data.IsData())
    {
        /* Null-terminator. */
        this->Data.Pop();
    }

    this->Data.Append(Other.Data);

#if CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */

    return;
}

FORCEINLINE void Jafg::LAsciiString::Pop()
{
    if (this->GetRuneCount() > 0)
    {
        this->Data.Pop();
        this->Data.Pop();

        if (this->Data.IsEmpty())
        {
            /* Orphan memory. */
            this->Empty();
        }
        else
        {
            this->Data.Add(LAsciiString::StringTerminatorRune);
        }
    }

#if CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */

    return;
}

FORCEINLINE const Jafg::LAsciiString::LRune* Jafg::LAsciiString::Peek() const
{
    if (this->GetRuneCount() == 0)
    {
        return &LAsciiString::StringTerminatorRune;
    }

    return &this->Data[this->GetSize() - 2];
}

FORCEINLINE void Jafg::LAsciiString::Reset(const SizeType Size)
{
    check( Size >= 0 )

    this->Data.Reset(Size);
    this->Data.Add(LAsciiString::StringTerminatorRune);

    return;
}

FORCEINLINE void Jafg::LAsciiString::Empty()
{
    this->Data.Empty();

#if CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */

    return;
}

FORCEINLINE const char* Jafg::LAsciiString::ToC() const
{
    if (this->Data.IsData())
    {
        return reinterpret_cast<const char*>(this->Data.GetData());
    }

    return &LAsciiString::StringTerminatorChar;
}

FORCEINLINE const Jafg::LAsciiString::LRune* Jafg::LAsciiString::ToPtr() const
{
    if (this->Data.IsData())
    {
        return this->Data.GetData();
    }

    return &LAsciiString::StringTerminatorRune;
}

FORCEINLINE Jafg::LAsciiString& Jafg::LAsciiString::operator+=(const LAsciiString& Other)
{
    if (this->Data.IsData())
    {
        this->Data.Pop();
    }

    this->Data.Append(Other.Data);

#if CHECK_STRING_VALIDITY
    this->EnsureValidState();
#endif /* CHECK_STRING_VALIDITY */

    return *this;
}

FORCEINLINE bool Jafg::LAsciiString::operator==(LNullptrTy) const
{
    return this->Data.IsData();
}

FORCEINLINE bool Jafg::LAsciiString::operator==(const char* Other) const
{
    checkSlow( Other != nullptr )

    if (this->Data.IsData())
    {
        for (const LRune Rune : this->Data)
        {
            if (Rune != *Other++)
            {
                return false;
            }

            continue;
        }

        return true;
    }

    return *Other == LAsciiString::StringTerminatorChar;
}

FORCEINLINE bool Jafg::LAsciiString::operator==(const LRune* Other) const
{
    checkSlow( Other != nullptr )

    if (this->Data.IsData())
    {
        for (const LRune Rune : this->Data)
        {
            if (Rune != *Other++)
            {
                return false;
            }

            continue;
        }

        return true;
    }

    return *Other == LAsciiString::StringTerminatorRune;
}

FORCEINLINE bool Jafg::LAsciiString::operator<(const char* Other) const
{
    checkSlow( Other != nullptr )

    if (this->Data.IsData() == false)
    {
        return *Other != LAsciiString::StringTerminatorChar;
    }

    for (const LRune Rune : this->Data)
    {
        if (*Other == LAsciiString::StringTerminatorChar)
        {
            return false;
        }

        if (Rune < *Other)
        {
            return true;
        }

        if (Rune > *Other)
        {
            return false;
        }

        ++Other;

        continue;
    }

    return true;
}

FORCEINLINE bool Jafg::LAsciiString::operator<(const LRune* Other) const
{
    checkSlow( Other != nullptr )

    if (this->Data.IsData() == false)
    {
        return *Other != LAsciiString::StringTerminatorRune;
    }

    for (const LRune Rune : this->Data)
    {
        if (*Other == LAsciiString::StringTerminatorRune)
        {
            return false;
        }

        if (Rune < *Other)
        {
            return true;
        }

        if (Rune > *Other)
        {
            return false;
        }

        ++Other;

        continue;
    }

    return true;
}

FORCEINLINE bool Jafg::LAsciiString::operator>(const char* Other) const
{
    if (this->Data.IsData() == false)
    {
        return false;
    }

    for (const LRune Rune : this->Data)
    {
        if (*Other == LAsciiString::StringTerminatorChar)
        {
            return Rune != LAsciiString::StringTerminatorRune;
        }

        if (Rune > *Other)
        {
            return true;
        }

        if (Rune < *Other)
        {
            return false;
        }

        ++Other;

        continue;
    }

    return false;
}

FORCEINLINE bool Jafg::LAsciiString::operator>(const LRune* Other) const
{
    if (this->Data.IsData() == false)
    {
        return false;
    }

    for (const LRune Rune : this->Data)
    {
        if (*Other == LAsciiString::StringTerminatorRune)
        {
            return Rune != LAsciiString::StringTerminatorRune;
        }

        if (Rune > *Other)
        {
            return true;
        }

        if (Rune < *Other)
        {
            return false;
        }

        ++Other;

        continue;
    }

    return false;
}

FORCEINLINE auto Jafg::LAsciiString::begin() noexcept -> Iterator<LRune>
{
    if (this->Data.IsData() == false)
    {
        return { nullptr };
    }

    return { this->Data.GetData() };
}

FORCEINLINE auto Jafg::LAsciiString::begin() const noexcept -> Iterator<const LRune>
{
    if (this->Data.IsData() == false)
    {
        return { nullptr };
    }

    return { this->Data.GetData() };
}

FORCEINLINE auto Jafg::LAsciiString::end() noexcept -> Iterator<LRune>
{
    if (this->Data.IsData() == false)
    {
        return { nullptr };
    }

    return { this->Data.GetSlack() - 1 };
}

FORCEINLINE auto Jafg::LAsciiString::end() const noexcept -> Iterator<const LRune>
{
    if (this->Data.IsData() == false)
    {
        return { nullptr };
    }

    return { this->Data.GetSlack() - 1 };
}

FORCEINLINE void Jafg::LAsciiString::EnsureValidState()
{
    if (this->Data.IsData())
    {
        if (*this->Data.Peek() != LAsciiString::StringTerminatorRune)
        {
            this->Data.Add(LAsciiString::StringTerminatorRune);
            LOG_ERROR(
                LogJafgInternal,
                "Ensured that string is null-terminated. This may not happen in shipping configurations."
            )
        }

        return;
    }

    check( this->GetSize()          == 0       )
    check( this->GetRuneCount()     == 0       )
    check( this->Data.GetCapacity() == 0       )
    check( this->Data.GetData()     == nullptr )

    return;
}

template <typename ... ArgyTy>
Jafg::LAsciiString Jafg::LAsciiString::SprintF(const char* Format, const ArgyTy& ... Args)
{
    /**
     * Super supid solution. But who cares right now.
     * Later we write our own implementation with type safety etc.
     *
     * S will be stackallocated if in str is small enough. So its not that bad.
     * But still one unnecessary heap allocation by this class...
     */
    const LStringLegacy S = std::vformat(Format, std::make_format_args(Args...));
    return { S.c_str() };
}
