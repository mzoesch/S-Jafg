// Copyright mzoesch. All rights reserved.

#include "System/ConfigIo.h"
#include "System/Paths.h"
#include "Async/TaskUtility.h"

namespace
{

void GoToNextLine(const Jafg::LString& InContentF, LSizeTy* Cursor);
//#
//# Given the current cursor position, find the first non whitespace / tab character in this line
//# or the next meaningful line (if this line is not meaningful). That is also not a comment
//# character or only contains whitespace / tab characters.
//#
void GoToThisLineStart(const Jafg::LString& InContentF, LSizeTy* Cursor);
auto FindSection(const Jafg::LString& InContentF, const Jafg::LStringView& InSection) -> Jafg::TOptional<LSizeTy>;
//#
//# Find the specified key in the specified section (where the cursor is currently positioned). Searches
//# until the key is found or the end of the section is reached.
//#
auto FindKey(const Jafg::LString& InContentF, const LSizeTy& InCursor, const Jafg::LStringView& InKey) -> Jafg::TOptional<LSizeTy>;
auto FindKeyValue(const Jafg::LString& InContentF, const LSizeTy& InCursor, const Jafg::LStringView& InKey) -> Jafg::TOptional<Jafg::LStringView>;
bool Serialize(Jafg::LString* ContentF, const Jafg::LStringView& InSection, const Jafg::LStringView& InKey, const Jafg::LStringView& InValue);
auto Deserialize(const Jafg::LString& InContentF, const Jafg::LStringView& InSection, const Jafg::LStringView& InKey) -> Jafg::TOptional<Jafg::LStringView>;

void GoToNextLine(const Jafg::LString& InContentF, LSizeTy* Cursor)
{
    checkSlow( Cursor )

    while (InContentF.IsValidIndex(*Cursor))
    {
        if (InContentF[*Cursor] == '\n')
        {
            ++*Cursor;
            break;
        }

        ++*Cursor;
    }


    return;
}

void GoToThisLineStart(const Jafg::LString& InContentF, LSizeTy* Cursor)
{
    checkSlow( Cursor )

    while (InContentF.IsValidIndex(*Cursor))
    {
        if (InContentF[*Cursor] == ';')
        {
            ::GoToNextLine(InContentF, Cursor);
            continue;
        }

        if (InContentF[*Cursor] == ' ' || InContentF[*Cursor] == '\t')
        {
            ++*Cursor;
            continue;
        }

        if (InContentF[*Cursor] == '\n' || InContentF[*Cursor] == '\r')
        {
            ++*Cursor;
            continue;
        }

        break;
    }

    return;
}

void GoToNextLineStart(const Jafg::LString& InContentF, LSizeTy* Cursor)
{
    ::GoToNextLine(InContentF, Cursor);
    ::GoToThisLineStart(InContentF, Cursor);

    return;
}

Jafg::TOptional<LSizeTy> FindSection(const Jafg::LString& InContentF, const Jafg::LStringView& InSection)
{
    using namespace Jafg;

    LSizeTy Cursor = 0;
    while (InContentF.IsValidIndex(Cursor))
    {
        ::GoToThisLineStart(InContentF, &Cursor);

        if (InContentF.IsValidIndex(Cursor) == false)
        {
            return { };
        }

        if (InContentF[Cursor] == '[')
        {
            ++Cursor;
            LSizeTy Start = Cursor;

            while (InContentF.IsValidIndex(Cursor) && InContentF[Cursor] != ']')
            {
                if (InContentF[Cursor] == ';' || InContentF[Cursor] == '\n' || InContentF[Cursor] == '\r')
                {
                    panicMsgf
                    (
                        "Invalid syntax in config file at line [{}] when trying to find [{}].",
                        InContentF.GetLineNumber(static_cast<LString::SizeType>(Cursor)),
                        InSection
                    )
                    return { };
                }

                ++Cursor;
                continue;
            }

            if (InContentF.IsValidIndex(Cursor) == false)
            {
                panicMsgf
                (
                    "Invalid syntax in config file at line [{}] when trying to find [{}].",
                    InContentF.GetLineNumber(static_cast<LString::SizeType>(Cursor)),
                    InSection
                )
                return { };
            }

            check( InContentF[Cursor] == ']' )
            const LStringView Section = InContentF.Sub<LStringView>(Start, Cursor - Start);
            if (Section == InSection)
            {
                return Start - 1;
            }
        }

        GoToNextLine(InContentF, &Cursor);
    }

    return { };
}

Jafg::TOptional<LSizeTy> FindKey(const Jafg::LString& InContentF, const LSizeTy& InCursor, const Jafg::LStringView& InKey)
{
    checkCode
    (
        if (InContentF.IsValidIndex(InCursor))
        {
            check( InContentF[InCursor] == '[' )
        }
    )

    using namespace Jafg;
    LSizeTy Cursor = InCursor;

    ::GoToNextLineStart(InContentF, &Cursor);
    while (true)
    {
        if (InContentF.IsValidIndex(Cursor) == false)
        {
            return { };
        }

        if (InContentF[Cursor] == '[')
        {
            return { };
        }

        if (LString::StartsWith(InContentF.GetBegin() + Cursor, InContentF.GetEnd(), InKey.GetBegin(), InKey.GetEnd()) == false)
        {
            ::GoToNextLineStart(InContentF, &Cursor);
            continue;
        }

        const LSizeTy OutCandidate = Cursor;

        Cursor += InKey.GetSize();
        if (InContentF.IsValidIndex(Cursor) == false)
        {
            return { };
        }

        if (InContentF[Cursor] != '=')
        {
            ::GoToNextLineStart(InContentF, &Cursor);
            continue;
        }

        Cursor += /* = */1;
        if (InContentF.IsValidIndex(Cursor) == false)
        {
            return { };
        }

        return OutCandidate;
    }
}

Jafg::TOptional<Jafg::LStringView> FindKeyValue(const Jafg::LString& InContentF, const LSizeTy& InCursor, const Jafg::LStringView& InKey)
{
    Jafg::TOptional<LSizeTy> Key = FindKey(InContentF, InCursor, InKey);
    if (!Key)
    {
        return { };
    }

    LSizeTy Cursor = *Key;
    Cursor += InKey.GetSize();
    check( InContentF.IsValidIndex(Cursor) && InContentF[Cursor] == '=' )
    Cursor += /* = */1;

    const Jafg::LString::T* Begin = InContentF.GetBegin() + Cursor;
    while (InContentF.IsValidIndex(Cursor))
    {
        if (InContentF[Cursor] == ';' || InContentF[Cursor] == '\n' || InContentF[Cursor] == '\r')
        {
            break;
        }

        ++Cursor;

        continue;
    }

    const Jafg::LString::T* End = InContentF.GetBegin() + Cursor;

    return Jafg::LStringView(Begin, End);
}


bool Serialize(Jafg::LString* ContentF, const Jafg::LStringView& InSection, const Jafg::LStringView& InKey, const Jafg::LStringView& InValue)
{
    using namespace Jafg;

    checkSlow( ContentF )

    if (const TOptional<LStringView> DeserializedValue = ::Deserialize(*ContentF, InSection, InKey); DeserializedValue)
    {
        const LStringView& Value = *DeserializedValue;
        if (Value.Equals(InValue.GetBegin(), InValue.GetEnd()) == false)
        {
            ContentF->Substitute(const_cast<LString::T*>(Value.GetBegin()), const_cast<LString::T*>(Value.GetEnd()), InValue.GetBegin(), InValue.GetEnd());
            return true;
        }

        return false;
    }

    TOptional<LSizeTy> SectionMaybe = ::FindSection(*ContentF, InSection);
    if (!SectionMaybe)
    {
        ContentF->Append(LString::SprintF("[{}]\n", InSection));
        SectionMaybe = ::FindSection(*ContentF, InSection);
    }
    LSizeTy Section = *SectionMaybe;

    ::GoToNextLine(*ContentF, &Section);
    ContentF->AppendAt(Section, LString::SprintF("{}={}\n", InKey, InValue));

    return true;
}

Jafg::TOptional<Jafg::LStringView> Deserialize(const Jafg::LString& InContentF, const Jafg::LStringView& InSection, const Jafg::LStringView& InKey)
{
    Jafg::TOptional<LSizeTy> Section = ::FindSection(InContentF, InSection);
    if (!Section)
    {
        return { };
    }
    LSizeTy Cursor = *Section;
    ::GoToThisLineStart(InContentF, &Cursor);
    Jafg::TOptional<LSizeTy> KeyCursor = ::FindKey(InContentF, Cursor, InKey);
    if (!KeyCursor)
    {
        return { };
    }

    Cursor = *KeyCursor;

    Cursor += InKey.GetSize();
    check( InContentF.IsValidIndex(Cursor) && InContentF[Cursor] == '=' )
    Cursor += /* = */1;

    if (InContentF.IsValidIndex(Cursor) == false)
    {
        return { };
    }

    const Jafg::LString::T* Begin = InContentF.GetBegin() + Cursor;
    while (InContentF.IsValidIndex(Cursor))
    {
        if (InContentF[Cursor] == ';' || InContentF[Cursor] == '\n' || InContentF[Cursor] == '\r')
        {
            break;
        }

        ++Cursor;

        continue;
    }

    const Jafg::LString::T* End = InContentF.GetBegin() + Cursor;

    return Jafg::LStringView(Begin, End);
}

} /* ~Namespace <Anonymous> */

bool Jafg::ConfigIo::Serialize(const LPath& InPath, const LStringView& InSection, const LStringView& InKey, const LStringView& InValue, const bool bDoBackup /* = true */)
{
    checkSlow( Tasks::IsOnMasterThread() )
    check( Paths::DoesFileExist(InPath) )

    LString ContentF = Paths::ReadFile(InPath);

    const bool bUpdated = ::Serialize(&ContentF, InSection, InKey, InValue);

    if (bUpdated)
    {
        if (bDoBackup)
        {
            Paths::MakeFileBackup(InPath);
        }

        Paths::OverrideFile(InPath, ContentF);

        LOG_VERBOSE(LogConfigIo, "Pushed field [{}::{}] with [{}].", InSection, InKey, InValue);
    }

    return bUpdated;
}

bool Jafg::ConfigIo::SerializeBulk(const LPath& InPath, const TArray<Entry>& InEntries, const bool bDoBackup /* = true */)
{
    checkSlow( Tasks::IsOnMasterThread() )
    check( Paths::DoesFileExist(InPath) )

    LString ContentF = Finder::ReadFile(InPath);

    bool bUpdated = false;
    for (const Entry& E : InEntries)
    {
        if (::Serialize(&ContentF, E.InSection, E.InKey, E.InValue))
        {
            bUpdated = true;
            LOG_VERBOSE(LogConfigIo, "Pushed field [{}::{}] with [{}].", E.InSection, E.InKey, E.InValue);
        }
    }

    if (bUpdated)
    {
        if (bDoBackup)
        {
            Paths::MakeFileBackup(InPath);
        }

        Paths::OverrideFile(InPath, ContentF);
    }

    return bUpdated;
}

Jafg::TOptional<Jafg::LString> Jafg::ConfigIo::Deserialize(const LPath& InPath, const LStringView& InSection, const LStringView& InKey)
{
    checkSlow( Tasks::IsOnMasterThread() )
    check( Paths::DoesFileExist(InPath) )

    const LString ContentF = Paths::ReadFile(InPath);
    TOptional<LStringView> Out = ::Deserialize(ContentF, InSection, InKey);

    if (Out)
    {
        LOG_VERBOSE(LogConfigIo, "Pulled field [{}::{}] with [{}].", InSection, InKey, *Out);
        return  Out->GetSize() == 0 ? LString("NULL") : LString(*Out);
    }

    return { };
}
