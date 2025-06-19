// Copyright mzoesch. All rights reserved.

#pragma once

#include "CliExtended.h"
#include "Cli/CliType.h"

namespace Jafg
{

class LCommandLineInterface;
struct LCliQuery;
template <>
struct LCliTypeFactory<LCliQuery>;

//#
//# A query. Use like this:
//# LCliType::Type<LCliQuery>(LCliQuery::Input
//# {
//#     "QueryOne", "QueryTwo", "QueryThree"
//# });
//#
struct LCliQuery
{
    typedef TArray<LString> Input;
};

template <>
struct LCliTypeFactory<LCliQuery>
{
    UTILITY_STRUCT(LCliTypeFactory)

    static constexpr bool bExists { true };

    FORCEINLINE static LCliType Dispatch(LString&& InName, LString&& InHelp, const TArray<LString>& InArgs);
};

ENGINE_API void AddExtendedPrimitivesToCli(LCommandLineInterface* Cli);

namespace Private
{

ENGINE_API bool CliQueryImpl(const LCommandArgs& Args, i32* Cursor, const TArray<LString>& Values);
ENGINE_API auto CliQuerySuggestImpl(const LCommandArgs& Args, const i32 Cursor, const i32 MaxSuggestions, const TArray<LString>& Values) -> TArray<LString>;

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

} /* ~Namespace Jafg */
