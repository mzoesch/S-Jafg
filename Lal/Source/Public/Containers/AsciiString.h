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

    inline static uint8 StringTerminator     = '\0';
    inline static char  StringTerminatorChar = '\0';

public:

    FORCEINLINE LAsciiString()                           noexcept;

    FORCEINLINE LAsciiString(      LAsciiString&  Other) noexcept;
    FORCEINLINE LAsciiString(const LAsciiString&  Other) noexcept;
    FORCEINLINE LAsciiString(      LAsciiString&& Other) noexcept;
    FORCEINLINE LAsciiString(const LAsciiString&& Other) noexcept = delete;
    FORCEINLINE LAsciiString(const LRune* Other)         noexcept;
    FORCEINLINE LAsciiString(const char* Other)          noexcept;

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
    FORCEINLINE auto Reset  (const SizeType Size)     -> void ;
    FORCEINLINE auto Empty  (void)                    -> void ;
    FORCEINLINE auto SwapStrings(LAsciiString& Other) -> void { this->Data.SwapBuffers(Other.Data); return; }

    /** Copy the string into another string. */
    FORCEINLINE auto CopyInto(LAsciiString& Other) const -> void { Other = *this;            return; }
    FORCEINLINE auto CopyFrom(const LAsciiString& Other) -> void { *this = Other;            return; }
    /** Move the string into another string. */
    FORCEINLINE auto MoveInto(LAsciiString& Other)       -> void { Other = std::move(*this); return; }
    FORCEINLINE auto MoveFrom(LAsciiString& Other)       -> void { *this = std::move(Other); return; }

    /** Get the size of the string in bytes. */
    FORCEINLINE auto GetSize(void) const -> SizeType { return this->Data.GetSize(); }
    /** Get the number of runes in the string (that excludes the null-terminator). */
    FORCEINLINE auto GetRuneCount(void) const -> SizeType { return this->Data.GetSize() - 1; }

    /** Interpret the string as a C-style string. */
    FORCEINLINE auto ToC()         ->       char * { return reinterpret_cast<      char*>(this->Data.GetData()); }
    FORCEINLINE auto ToC()   const -> const char * { return reinterpret_cast<const char*>(this->Data.GetData()); }
    FORCEINLINE auto ToPtr()       ->       LRune* { return this->Data.GetData(); }
    FORCEINLINE auto ToPtr() const -> const LRune* { return this->Data.GetData(); }

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
    FORCEINLINE auto Equals(const char* Other) const -> bool { return *this == Other; }
    FORCEINLINE auto Equals(const LRune* Other) const -> bool { return *this == Other; }

    /** Private iterator functions for range-based loops. Do not use these directly. */
    FORCEINLINE auto begin()       noexcept -> Iterator<LRune>       { return Iterator<LRune>      (this->Data.GetData());      }
    FORCEINLINE auto begin() const noexcept -> Iterator<const LRune> { return Iterator<const LRune>(this->Data.GetData());      }
    FORCEINLINE auto end()         noexcept -> Iterator<LRune>       { return Iterator<LRune>      (this->Data.GetSlack() - 1); }
    FORCEINLINE auto end()   const noexcept -> Iterator<const LRune> { return Iterator<const LRune>(this->Data.GetSlack() - 1); }

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
    /*
     * We reserve one here. As if we only have empty strings, we still need to have the default null-terminator
     * allocated to ensure backwards compatibility with C-style strings.
     * If we just added the terminator, the default-dynamic-array allocator would instantly allocate not one
     * but five new elements. This would cost us a lot of memory.
     *
     * For later: Maybe we can just have a global string terminator pointer and use that instead? We currently do not
     * randomly reorganize runes inside a string inside c-style functions.
     * Maybe we add an explicit way of doing that when the time comes, as: #GetGrowableMemoryPointer.
     */
    this->Data.Reserve(1);

    this->Data.Add(LAsciiString::StringTerminator);

    return;
}

FORCEINLINE Jafg::LAsciiString::LAsciiString(LAsciiString& Other) noexcept
{
    this->Data = Other.Data;

#if !IN_SHIPPING
    if (*this->Data.Peek() != LAsciiString::StringTerminator)
    {
        panic( "Got copy of illformed string (not null-terminated)." )
    }
#endif /* !IN_SHIPPING */

    return;
}

FORCEINLINE Jafg::LAsciiString::LAsciiString(const LAsciiString& Other) noexcept
{
    this->Data = Other.Data;

#if !IN_SHIPPING
    if (*this->Data.Peek() != LAsciiString::StringTerminator)
    {
        panic( "Got copy of illformed string (not null-terminated)." )
    }
#endif /* !IN_SHIPPING */

    return;
}

FORCEINLINE Jafg::LAsciiString::LAsciiString(LAsciiString&& Other) noexcept
{
    this->Data = std::forward<TdhArray<LRune>>(Other.Data);

#if !IN_SHIPPING
    if (*this->Data.Peek() != LAsciiString::StringTerminator)
    {
        panic( "Got forwarded memory of illformed string (not null-terminated)." )
    }
#endif /* !IN_SHIPPING */

    /*
     * The other one is a string we have to reset to ensure that it is in a valid state.
     * As a valid forwarded state of a TArray is not valid for a LAsciiString.
     */
    Other.EnsureValidState();

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

        if (*Other++ == LAsciiString::StringTerminator)
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

#if !IN_SHIPPING
    if (*this->Data.Peek() != LAsciiString::StringTerminator)
    {
        panic( "Got copy of illformed string (not null-terminated)." )
    }
#endif /* !IN_SHIPPING */

    return *this;
}

FORCEINLINE Jafg::LAsciiString& Jafg::LAsciiString::operator=(LAsciiString&& Other) noexcept
{
    this->Data = std::forward<TdhArray<LRune>>(Other.Data);

#if !IN_SHIPPING
    if (*this->Data.Peek() != LAsciiString::StringTerminator)
    {
        panic( "Got forwarded memory of illformed string (not null-terminated)." )
    }
#endif /* !IN_SHIPPING */

    /*
     * The other one is a string we have to reset to ensure that it is in a valid state.
     * As a valid forwarded state of a TArray is not valid for a LAsciiString.
     */
    Other.EnsureValidState();

    return *this;
}

FORCEINLINE void Jafg::LAsciiString::Append(const char* Other)
{
    this->Data.Pop();

    while (true)
    {
        this->Data.Add(*Other);

        if (*Other++ == LAsciiString::StringTerminatorChar)
        {
            break;
        }

        continue;
    }

    return;
}

FORCEINLINE void Jafg::LAsciiString::Append(const LAsciiString& Other)
{
    this->Data.Pop();
    this->Data.Append(Other.Data);

#if !IN_SHIPPING
    if (*this->Data.Peek() != LAsciiString::StringTerminator)
    {
        panic( "Got copy of illformed string (not null-terminated)." )
    }
#endif /* !IN_SHIPPING */

    return;
}

FORCEINLINE void Jafg::LAsciiString::Pop()
{
    if (this->GetRuneCount() > 0)
    {
        this->Data.Pop();
        this->Data.Pop();
        this->Data.Add(LAsciiString::StringTerminator);
    }

    return;
}

FORCEINLINE const Jafg::LAsciiString::LRune* Jafg::LAsciiString::Peek() const
{
    if (this->GetRuneCount() == 0)
    {
        return this->Data.Peek();
    }

    return &this->Data[this->GetSize() - 2];
}

FORCEINLINE void Jafg::LAsciiString::Reset(const SizeType Size)
{
    this->Data.Reset(Size < 1 ? 1 : Size);
    this->Data.Add(LAsciiString::StringTerminator);

    return;
}

FORCEINLINE void Jafg::LAsciiString::Empty()
{
    this->Data.Empty();
    this->Data.Add(LAsciiString::StringTerminator);
    return;
}

FORCEINLINE Jafg::LAsciiString::LAsciiString(const LRune* Other) noexcept
{
    while (true)
    {
        this->Data.Add(*Other);

        if (*Other == LAsciiString::StringTerminator)
        {
            break;
        }

        continue;
    }

    return;
}

FORCEINLINE Jafg::LAsciiString& Jafg::LAsciiString::operator+=(const LAsciiString& Other)
{
    this->Data.Pop();
    this->Data.Append(Other.Data);

#if !IN_SHIPPING
    if (*this->Data.Peek() != LAsciiString::StringTerminator)
    {
        panic( "Got copy of illformed string (not null-terminated)." )
    }
#endif /* !IN_SHIPPING */

    return *this;
}

FORCEINLINE bool Jafg::LAsciiString::operator==(const char* Other) const
{
    check( Other != nullptr )

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

FORCEINLINE bool Jafg::LAsciiString::operator==(const LRune* Other) const
{
    check( Other != nullptr )

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

FORCEINLINE bool Jafg::LAsciiString::operator<(const char* Other) const
{
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
    for (const LRune Rune : this->Data)
    {
        if (*Other == LAsciiString::StringTerminator)
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
    for (const LRune Rune : this->Data)
    {
        if (*Other == LAsciiString::StringTerminatorChar)
        {
            return Rune != LAsciiString::StringTerminator;
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
    for (const LRune Rune : this->Data)
    {
        if (*Other == LAsciiString::StringTerminator)
        {
            return Rune != LAsciiString::StringTerminator;
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

FORCEINLINE void Jafg::LAsciiString::EnsureValidState()
{
    if (this->GetSize() == 0)
    {
        this->Data.Add(LAsciiString::StringTerminator);
        return;
    }

    if (*this->Data.Peek() != LAsciiString::StringTerminator)
    {
        this->Data.Add(LAsciiString::StringTerminator);
    }

    return;
}
