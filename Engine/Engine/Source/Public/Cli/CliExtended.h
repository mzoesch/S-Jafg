// Copyright mzoesch. All rights reserved.

#pragma once

#include "CliExtended.h"
#include "Cli/CliType.h"

namespace Jafg
{

class LCommandLineInterface;
struct LCliQuery;
struct LCliVariable;
template <>
struct LCliTypeFactory<LCliQuery>;

//#
//# A query. Use like this:
//# LCliType::Type<LCliQuery>(LCliQuery::Input
//# {
//#     "QueryOne", "QueryTwo", "QueryThree"
//# });
//#
struct LCliQuery final
{
    typedef TArray<LString> Input;
};

//#
//# A string value that only allows exactly one value.
//# This is useful for overloading when #LCliQuery is not enough as different queries require different
//# command signatures.
//# Use like this:
//# LCliType::Type<LCliString>("MyString");
//#
struct LCliString final
{
};

//#
//# Any variable registered to the CLI. Note that this will not work with intermediate variables.
//# For them use LCliType::Type("Any") instead.
//#
template<> FORCEINLINE LCliType LCliType::Type<LCliVariable>() { return LCliType::Type("Var"); }


///////////////////////////////////////////////////////////////////////////////
// Implementations

template <>
struct LCliTypeFactory<LCliQuery> final
{
    UTILITY_STRUCT(LCliTypeFactory)

    static constexpr bool bExists { true };

    FORCEINLINE static LCliType Dispatch(LString&& InName, LString&& InHelp, const TArray<LString>& InArgs);
};

template <>
struct LCliTypeFactory<LCliString> final
{
    UTILITY_STRUCT(LCliTypeFactory)

    static constexpr bool bExists { true };

    FORCEINLINE static LCliType Dispatch(const LString& InValue);
};

namespace Private
{

ENGINE_API void AddExtendedPrimitivesToCli(LCommandLineInterface* Cli);

ENGINE_API bool CliQueryImpl(const LCommandArgs& Args, i32* Cursor, const TArray<LString>& Values);
ENGINE_API auto CliQuerySuggestImpl(const LCommandArgs& Args, const i32 Cursor, const i32 MaxSuggestions, const TArray<LString>& Values) -> TArray<LString>;
ENGINE_API bool CliStringImpl(const LCommandArgs& Args, i32* Cursor, const LString& Value);
ENGINE_API auto CliStringSuggestImpl(const LCommandArgs& Args, const i32 Cursor, const i32 MaxSuggestions, const LString& Value) -> TArray<LString>;

} /* ~Namespace Private */


///////////////////////////////////////////////////////////////////////////////
// Definitions

FORCEINLINE LCliType LCliTypeFactory<LCliQuery>::Dispatch(LString&& InName, LString&& InHelp, const TArray<LString>& InArgs)
{
    return LCliType
    {
        std::move(InName),
        {},
        std::move(InHelp),
        [Values = InArgs](const LCommandArgs& Args, i32* Cursor) -> bool
        {
            return Private::CliQueryImpl(Args, Cursor, Values);
        },
        nullptr,
        [Values = InArgs](const LCommandArgs& Args, const i32 Cursor, const i32 MaxSuggestions) -> TArray<LString>
        {
            return Private::CliQuerySuggestImpl(Args, Cursor, MaxSuggestions, Values);
        },
    };
}

FORCEINLINE LCliType LCliTypeFactory<LCliString>::Dispatch(const LString& InValue)
{
    return LCliType
    {
        InValue,
        {},
        {},
        [Value = InValue](const LCommandArgs& Args, i32* Cursor) -> bool
        {
            return Private::CliStringImpl(Args, Cursor, Value);
        },
        nullptr,
        [Value = InValue](const LCommandArgs& Args, const i32 Cursor, const i32 MaxSuggestions) -> TArray<LString>
        {
            return Private::CliStringSuggestImpl(Args, Cursor, MaxSuggestions, Value);
        },
    };
}

} /* ~Namespace Jafg */
