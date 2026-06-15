// Copyright mzoesch. All rights reserved.

#pragma once

#include "Cli/CliObject.h"

namespace Jafg
{

class LCliType;
struct LCommandArgs;

//# @return True if the argument can be parsed as a valid type.
typedef TFunction<bool(LCommandArgs const& Args, std::size_t* Cursor)> LOnParseType;

//#
//# @param OutValue Is always valid.
//# @return True if the #OutValue was set changed.
//#
typedef TFunction<bool(const LCommandArgs& InValue, LString* OutValue)> LOnValueSet;

//# @return A list of suggestions for the given arguments. The first index is the most common suggestion.
typedef TFunction<TArray<LString>(LCommandArgs const& Args, std::size_t Cursor, u32 MaxSuggestions)> LOnSuggestType;

//#
//# Utility struct for a type T that may accept an arbitrary number of arguments which fit for said type T.
//#
template <typename T>
struct LCliTypeFactory final
{
    UTILITY_STRUCT(LCliTypeFactory)
    static constexpr bool bExists { false };
};

//#
//# A variable type inside the cli of the engine. May be used for type checking within commands and variables.
//#
class LCliType final : public LCliObject
{
public:

    FORCEINLINE LCliType() = delete;

    FORCEINLINE LCliType
    (
          const LString& InIdentifier
        , const LString& InDefault
        , LOnParseType&& InOnParseType
        , LOnValueSet&& InOnValueSet = nullptr
        , LOnSuggestType&& InOnSuggest = nullptr
    )
        : LCliObject(InIdentifier)
        , Default(InDefault)
        , OnParseType(std::move(InOnParseType))
        , OnValueSet(std::move(InOnValueSet))
        , OnSuggest(std::move(InOnSuggest))
    {
    }

    FORCEINLINE LCliType
    (
          const LString& InIdentifier, const LString& InHelp
        , const LString& InDefault
        , LOnParseType&& InOnParseType
        , LOnValueSet&& InOnValueSet = nullptr
        , LOnSuggestType&& InOnSuggest = nullptr
    )
        : LCliObject(InIdentifier, InHelp)
        , Default(InDefault)
        , OnParseType(std::move(InOnParseType))
        , OnValueSet(std::move(InOnValueSet))
        , OnSuggest(std::move(InOnSuggest))
    {
    }

    PROHIBIT_COPY(LCliType)

    FORCEINLINE LCliType(LCliType&& InOther) noexcept;
    FORCEINLINE LCliType& operator=(LCliType&& InOther) noexcept;

    //#
    //# Use this for referring to an already registered type.
    //#
    FORCEINLINE static LCliType Type(const LString& InIdentifier) { return LCliType(InIdentifier); }
    FORCEINLINE static LCliType Type(const LString& InIdentifier, const LString& InHelp) { return LCliType(InIdentifier, InHelp); }
    template<typename T>
    FORCEINLINE static LCliType Type() UNSUPPORTED_TEMPLATED_SPECIALIZATION(T, return LCliType::Type(""))
    template<typename T, typename... TArgs> requires LCliTypeFactory<T>::bExists
    FORCEINLINE static LCliType Type(TArgs&&... Args);

    FORCEINLINE LCliType* GetRegisteredType() const { return static_cast<LCliType*>(this->GetRegisteredObjectByUuid()); }

    //#
    //# Set a variable typesafe through the engine with this method.
    //#
    ENGINE_API bool SetVariable(const LCommandArgs& InValue, LString* OutValue) const;

    ENGINE_API bool CanParse(const LCommandArgs& Args, std::size_t* Cursor) const;
    ENGINE_API TArray<LString> Suggest(const LCommandArgs& Args, std::size_t Cursor, u32 MaxSuggestions) const;

    FORCEINLINE auto GetDefault() const -> const LString& { return this->Default; }
    FORCEINLINE bool IsTypeDelegateValid() const noexcept { return this->OnParseType.IsValid(); }
    FORCEINLINE bool IsValueSetDelegateValid() const noexcept { return this->OnValueSet.IsValid(); }

private:

    FORCEINLINE explicit LCliType(const LString& InIdentifier) : LCliObject(InIdentifier) { this->ExpandToUuid(); }
    FORCEINLINE explicit LCliType(const LString& InIdentifier, const LString& InHelp) : LCliObject(InIdentifier, InHelp) { this->ExpandToUuid(); }
    LString Default;

    LOnParseType OnParseType;
    LOnValueSet OnValueSet;
    LOnSuggestType OnSuggest;
};

FORCEINLINE LCliType::LCliType(LCliType&& InOther) noexcept
{
    this->Default = std::move(InOther.Default);
    this->OnParseType = std::move(InOther.OnParseType);
    this->OnValueSet = std::move(InOther.OnValueSet);
    this->OnSuggest = std::move(InOther.OnSuggest);
    this->LCliObject::operator=(std::move(InOther));
    return;
}

FORCEINLINE LCliType& LCliType::operator=(LCliType&& InOther) noexcept
{
    this->Default = std::move(InOther.Default);
    this->OnParseType = std::move(InOther.OnParseType);
    this->OnValueSet = std::move(InOther.OnValueSet);
    this->OnSuggest = std::move(InOther.OnSuggest);
    this->LCliObject::operator=(std::move(InOther));
    return *this;
}

template <typename T, typename... TArgs> requires LCliTypeFactory<T>::bExists
FORCEINLINE LCliType LCliType::Type(TArgs&&... Args)
{
    return LCliTypeFactory<T>::Dispatch(std::forward<TArgs>(Args)...);
}

} /* ~Namespace Jafg */
