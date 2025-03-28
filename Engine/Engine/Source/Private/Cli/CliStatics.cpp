// Copyright mzoesch. All rights reserved.

#include "Cli/CliStatics.h"
#include "Cli/CliCommand.h"

namespace
{

bool IsValidArgs(const Jafg::LCommandArgs& InArgs)
{
    if (InArgs.Name.IsEmpty() == false)
    {
        return InArgs.SubArgs.IsEmpty();
    }

    for (const Jafg::LCommandArgs& SubArg : InArgs.SubArgs)
    {
        if (::IsValidArgs(SubArg) == false)
        {
            return false;
        }
    }

    return true;
}

} /* ~Namespace <Anonymous> */

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
    const i32 Space = InText.FindFirst(" ");

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
    const i32 Space = InText.FindFirst(" ");

    if (Space == INDEX_NONE)
    {
        return "";
    }

    if (InText.GetSize()-1 <= Space + 1)
    {
        return "";
    }

    return InText.RightChop(Space + 1);
}

Jafg::LCommandArgs Jafg::CliStatics::TokenizeCommand(LString&& InCommandLine)
{
    LCommandArgs Out;

    LString Cur; Cur.Reserve(InCommandLine.GetSize());
    i32 Cursor = INDEX_NONE;
    bool bInString = false;
    char LastChar = 0;
    while (++Cursor < InCommandLine.GetRuneCount())
    {
        const char CurChar = *InCommandLine[Cursor];
        if (bInString)
        {
            if (CurChar == '"' && LastChar != '\\')
            {
                if (Cur.IsEmpty() == false)
                {
                    Out.SubArgs.Emplace(LCommandArgs(std::move(Cur)));
                }
                check( Cur.IsEmpty() )
                LastChar = CurChar;
                continue;
            }

            if (CurChar != '\\')
            {
                Cur += CurChar;
            }

            LastChar = CurChar;
            continue;
        }

        if (CurChar == '"')
        {
            if (Cur.IsEmpty() == false)
            {
                Out.SubArgs.Emplace(LCommandArgs(std::move(Cur)));
            }
            check( Cur.IsEmpty() )
            bInString = true;
            LastChar = CurChar;
            continue;
        }

        if (CurChar == ' ')
        {
            if (Cur.IsEmpty() == false)
            {
                Out.SubArgs.Emplace(LCommandArgs(std::move(Cur)));
            }
            check( Cur.IsEmpty() )
            LastChar = CurChar;
            continue;
        }

        Cur += CurChar;
        LastChar = CurChar;
        continue;
    }

    if (Cur.IsEmpty() == false)
    {
        Out.SubArgs.Emplace(LCommandArgs(std::move(Cur)));
    }

    if (bInString)
    {
        return LCommandArgs();
    }

    check( ::IsValidArgs(Out) )
    return Out;
}
