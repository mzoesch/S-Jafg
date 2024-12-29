// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/Cli/CliStatics.h"

#include "Engine/Cli/CliCommand.h"
#include "Engine/Cli/CliObject.h"

Jafg::LString Jafg::CliStatics::SafelyRemoveCommandPrefix(const LString& InText)
{
    LString Out;

    if (InText.StartsWith("/"))
    {
        Out = InText.RightChop(1);
    }
    else
    {
        Out = InText;
    }

    return Out;
}

void Jafg::CliStatics::SafelyRemoveCommandPrefixInline(LString& InText)
{
    if (InText.StartsWith("/"))
    {
        InText.InlineRightChop(1);
    }

    check( InText.StartsWith("/") == false )

    return;
}

Jafg::LString Jafg::CliStatics::SafelyAddCommandPrefix(const LString& InText)
{
    LString Out;

    if (InText.StartsWith("/"))
    {
        Out = InText;
    }
    else
    {
        Out = "/";
        Out += InText;
    }

    return Out;
}

void Jafg::CliStatics::SafelyAddCommandPrefixInline(LString& InText)
{
    if (InText.StartsWith("/") == false)
    {
        InText.AppendAt(0, "/");
    }

    check( InText.StartsWith("/") )

    return;
}

Jafg::LString Jafg::CliStatics::GetCommandFromText(const LString& InText)
{
    const int32 Space = InText.FindFirst(" ");

    if (Space == INDEX_NONE)
    {
        LString Temp = InText;
        SafelyRemoveCommandPrefixInline(Temp);
        return Temp;
    }

    LString Temp = InText.LeftChop(Space);
    SafelyRemoveCommandPrefixInline(Temp);
    return Temp;
}

Jafg::LString Jafg::CliStatics::GetArgsFromText(const LString& InText)
{
    const int32 Space = InText.FindFirst(" ");

    if (Space == INDEX_NONE)
    {
        return "";
    }

    return InText.RightChop(Space);
}

void Jafg::CliStatics::TokenizeCommand(LString&& InCommandLine, LCommandArgs* OutTokens)
{
    check(OutTokens->GetArgCount() == 0 )

    LString CommandLine = std::move(InCommandLine);

    while (true)
    {
        const int32 Idx = CommandLine.FindFirst(" ");
        if (Idx == INDEX_NONE)
        {
            if (CommandLine.IsEmpty() == false)
            {
                OutTokens->Args.Add(CliStatics::TokenizeArg(CommandLine));
            }
            return;
        }

        LString Arg = CommandLine.LeftChop(Idx);
        if (Arg.IsEmpty())
        {
            CommandLine.InlineRightChop(Idx + 1);
            continue;
        }

        OutTokens->Args.Add(CliStatics::TokenizeArg(Arg));

        CommandLine.InlineRightChop(Idx + 1);
        continue;
    }

    return;
}

Jafg::LCliToken* Jafg::CliStatics::TokenizeArg(const LString& InArg)
{
    LCliToken_String* MyToken = new LCliToken_String();
    MyToken->Value = InArg;
    return MyToken;
}
