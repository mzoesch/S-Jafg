
// Copyright mzoesch. All rights reserved.

#pragma once

///////////////////////////////////////////////////////////////////////////////
// String Traits.
///////////////////////////////////////////////////////////////////////////////
namespace Lal
{

/*----------------------------------------------------------------------------
    Concepts.
----------------------------------------------------------------------------*/

template <typename T>
concept TStringBaseEncodingConcept = requires
{
    typename T::T;
    typename T::SizeType;

    T::Terminator;

    { T::GetStringLength(std::declval<const typename T::T*>()) } -> std::convertible_to<typename T::SizeType>;

    { T::GetCharacterCount(std::declval<const typename T::T*>()) }  -> std::convertible_to<typename T::SizeType>;
    { T::GetCharacterCount(std::declval<const typename T::T*>(), std::declval<const typename T::T*>()) } -> std::convertible_to<typename T::SizeType>;

    { T::GetCharacterSize(std::declval<const typename T::T*>()) } -> std::convertible_to<typename T::SizeType>;

    { T::RecoverToMostRecentValidCharacter(std::declval<typename T::T*>(), std::declval<const typename T::T*>()) } -> std::same_as<typename T::T*>;
    { T::RecoverToMostRecentValidCharacter(std::declval<const typename T::T*>(), std::declval<const typename T::T*>()) } -> std::same_as<const typename T::T*>;

    { T::GetUppercaseRune(std::declval<typename T::T>()) } -> std::same_as<typename T::T>;
    { T::GetLowercaseRune(std::declval<typename T::T>()) } -> std::same_as<typename T::T>;
};


/*----------------------------------------------------------------------------
    Implementation.
----------------------------------------------------------------------------*/

//#
//# The base encoding for strings.
//#
template <typename TIn, std::integral TSizeType>
struct TStringBaseEncodingBase
{
    typedef TIn       T;
    typedef TSizeType SizeType;
};

//#
//# Common traits. Define your own if you need to. These are the most basic UTF-8 traits.
//#
template <typename TIn, std::integral TSizeType>
struct TStringBaseDefaultUtf8Traits : public TStringBaseEncodingBase<TIn, TSizeType>
{
    typedef TStringBaseEncodingBase<TIn, TSizeType> _Super;

    typedef typename _Super::T        T;
    typedef typename _Super::SizeType SizeType;

    inline static constexpr T Terminator { '\0' };

    UTILITY_STRUCT(TStringBaseDefaultUtf8Traits)

    //#
    //# @return The length of the string, excluding the null terminator in runes.
    //#
    template <typename T>
    FORCEINLINE static SizeType GetStringLength(const T* String) noexcept;

    //#
    //# @return The length of the string, excluding the null terminator in characters.
    //#
    //# @note This just includes basic UTF-8 characters, not any extended characters or compound shit.
    //#
    template <typename T>
    static SizeType GetCharacterCount(const T* String) noexcept;
    template <typename T>
    static SizeType GetCharacterCount(const T* Begin, const T* End) noexcept;

    //#
    //# The length of one character in runes.
    //#
    static SizeType GetCharacterSize(const T* Character) noexcept;

    //#
    //# Recover the given pointer #Character to the most recent valid UTF-8 Character.
    //# @param Guard The max the Character may go back. If this is not given, it will go back until one valid character
    //#              is found. The #Terminator also qualifies as a valid character.
    //#
    static T* RecoverToMostRecentValidCharacter(T* Character, const T* Guard = nullptr);
    static const T* RecoverToMostRecentValidCharacter(const T* Character, const T* Guard = nullptr);

    static T GetUppercaseRune(const T Rune);
    static T GetLowercaseRune(const T Rune);
};

static_assert(TStringBaseEncodingConcept<TStringBaseDefaultUtf8Traits<LChar, LSize>>);

} /* ~Namespace Lal */
