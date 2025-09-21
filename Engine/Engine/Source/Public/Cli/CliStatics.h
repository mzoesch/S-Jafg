// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

struct LCommandArgs;
struct LCommandParams;

namespace CliStatics
{

//# Whether the user input text is a command.
FORCEINLINE bool IsCommand(const LString& InText);

ENGINE_API LString SafelyRemoveCommandPrefix(const LString& InText);
ENGINE_API void    SafelyRemoveCommandPrefixInline(LString& InText);
ENGINE_API LString SafelyAddCommandPrefix(const LString& InText);
ENGINE_API void    SafelyAddCommandPrefixInline(LString& InText);

//# @return Only the command without any prefix.
ENGINE_API LString GetCommandFromText(const LString& InText);
//# @return Only the arguments for the command stringified.
ENGINE_API LString GetArgsFromText(const LString& InText);

//# @param InCommandLine The commandline without the command to parse.
ENGINE_API LCommandArgs TokenizeCommand(LString&& InCommandLine);

} /* ~Namespace CliStatics */

bool CliStatics::IsCommand(const LString& InText)
{
    return InText.StartsWith(FMT("/"));
}

} /* ~Namespace Jafg */
