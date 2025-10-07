// Copyright mzoesch. All rights reserved.

#include "Cli/CliStatics.h"
#include "Cli/CliCommand.h"

namespace
{

bool IsValidArgs(const Jafg::LCommandArgs& InArgs)
{
    if (InArgs.Name.empty() == false)
    {
        return InArgs.SubArgs.empty();
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

LString Jafg::CliStatics::SafelyRemoveCommandPrefix(const LString& InText)
{
    LString Out;

    if (InText.starts_with('/'))
    {

        Out = algo::right_chop(InText, 1);
    }
    else
    {
        Out = InText;
    }

    return Out;
}

void Jafg::CliStatics::SafelyRemoveCommandPrefixInline(LString& InText)
{
    if (InText.starts_with('/'))
    {
        algo::inline_right_chop(&InText, 1);
    }

    check( InText.starts_with('/') == false )

    return;
}

LString Jafg::CliStatics::SafelyAddCommandPrefix(const LString& InText)
{
    LString Out;

    if (InText.starts_with('/'))
    {
        Out = InText;
    }
    else
    {
        Out = '/';
        Out += InText;
    }

    return Out;
}

void Jafg::CliStatics::SafelyAddCommandPrefixInline(LString& InText)
{
    if (InText.starts_with('/') == false)
    {
        InText.insert(0, "/");
    }

    check( InText.starts_with('/') )

    return;
}

LString Jafg::CliStatics::GetCommandFromText(const LString& InText)
{
    const auto Space{ InText.find(' ') };
    if (Space == InText.npos)
    {
        LString Temp = InText;
        SafelyRemoveCommandPrefixInline(Temp);
        return Temp;
    }

    LString Temp = algo::left_chop(InText, Space);
    SafelyRemoveCommandPrefixInline(Temp);
    return Temp;
}

LString Jafg::CliStatics::GetArgsFromText(const LString& InText)
{
    const auto Space{ InText.find(' ') };

    if (Space == InText.npos)
    {
        return { };
    }

    if (InText.size() - 1 <= static_cast<u64>(Space + 1))
    {
        return { };
    }

    return algo::right_chop(InText, Space + 1);
}

Jafg::LCommandArgs Jafg::CliStatics::TokenizeCommand(LString&& InCommandLine)
{
    LCommandArgs Out;

    LString Cur; Cur.reserve(InCommandLine.size());
    i64 Cursor = INDEX_NONE;
    bool bInString = false;
    char LastChar = 0;
    while (static_cast<u64>(++Cursor) < InCommandLine.size())
    {
        const char CurChar = InCommandLine[Cursor];
        if (bInString)
        {
            if (CurChar == '"' && LastChar != '\\')
            {
                if (Cur.empty() == false)
                {
                    Out.SubArgs.emplace_back(std::move(Cur));
                }
                check( Cur.empty() )
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
            if (Cur.empty() == false)
            {
                Out.SubArgs.emplace_back(std::move(Cur));
            }
            check( Cur.empty() )
            bInString = true;
            LastChar = CurChar;
            continue;
        }

        if (CurChar == ' ')
        {
            if (Cur.empty() == false)
            {
                Out.SubArgs.emplace_back(std::move(Cur));
            }
            check( Cur.empty() )
            LastChar = CurChar;
            continue;
        }

        Cur += CurChar;
        LastChar = CurChar;
        continue;
    }

    if (Cur.empty() == false)
    {
        Out.SubArgs.emplace_back(std::move(Cur));
    }

    if (bInString)
    {
        return LCommandArgs{};
    }

    check( ::IsValidArgs(Out) )
    return Out;
}
