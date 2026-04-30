// Copyright mzoesch. All rights reserved.

#include "System/ConfigIo.h"
#include "Async/TaskUtility.h"

#if 0

namespace
{

void GoToNextLine(const LString& InContentF, std::size_t* Cursor);
//#
//# Given the current cursor position, find the first non whitespace / tab character in this line
//# or the next meaningful line (if this line is not meaningful). That is also not a comment
//# character or only contains whitespace / tab characters.
//#
void GoToThisLineStart(const LString& InContentF, std::size_t* Cursor);
auto FindSection(const LString& InContentF, const LStringView& InSection) -> std::optional<std::size_t>;
//#
//# Find the specified key in the specified section (where the cursor is currently positioned). Searches
//# until the key is found or the end of the section is reached.
//#
auto FindKey(const LString& InContentF, const std::size_t& InCursor, const LStringView& InKey) -> std::optional<std::size_t>;
auto FindKeyValue(LString& InContentF, const std::size_t& InCursor, const LStringView& InKey) -> std::optional<LStringView>;
bool Serialize(LString* ContentF, const LStringView& InSection, const LStringView& InKey, const LStringView& InValue);
auto Deserialize(LString& InContentF, const LStringView& InSection, const LStringView& InKey) -> std::optional<LStringView>;

void GoToNextLine(const LString& InContentF, std::size_t* Cursor)
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

void GoToThisLineStart(const LString& InContentF, std::size_t* Cursor)
{
    checkSlow( Cursor )

    while (algo::is_valid_index(InContentF, *Cursor))
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

void GoToNextLineStart(const LString& InContentF, std::size_t* Cursor)
{
    ::GoToNextLine(InContentF, Cursor);
    ::GoToThisLineStart(InContentF, Cursor);

    return;
}

std::optional<std::size_t> FindSection(const LString& InContentF, const LStringView& InSection)
{
    using namespace Jafg;

    std::size_t Cursor { 0 };
    while (algo::is_valid_index(InContentF, Cursor))
    {
        ::GoToThisLineStart(InContentF, &Cursor);

        if (algo::is_valid_index(InContentF, Cursor) == false)
        {
            return { };
        }

        if (InContentF[Cursor] == '[')
        {
            ++Cursor;
            std::size_t Start = Cursor;

            while (algo::is_valid_index(InContentF, Cursor) && InContentF[Cursor] != ']')
            {
                if (InContentF[Cursor] == ';' || InContentF[Cursor] == '\n' || InContentF[Cursor] == '\r')
                {
                    panicMsgf
                    (
                        "Invalid syntax in config file at line [{}] when trying to find [{}].",
                        InContentF.GetLineNumber(static_cast<LString::size_type>(Cursor)),
                        InSection
                    )
                    return { };
                }

                ++Cursor;
                continue;
            }

            if (algo::is_valid_index(InContentF, Cursor) == false)
            {
                panicMsgf
                (
                    "Invalid syntax in config file at line [{}] when trying to find [{}].",
                    InContentF.GetLineNumber(static_cast<LString::size_type>(Cursor)),
                    InSection
                )
                return { };
            }

            check( InContentF[Cursor] == ']' )
            if (LStringView{InContentF.begin() + Start, InContentF.begin() + Cursor} == InSection)
            {
                return Start - 1;
            }
        }

        GoToNextLine(InContentF, &Cursor);
    }

    return { };
}

std::optional<std::size_t> FindKey(const LString& InContentF, const std::size_t& InCursor, const LStringView& InKey)
{
    checkCode
    (
        if (InContentF.IsValidIndex(InCursor))
        {
            check( InContentF[InCursor] == '[' )
        }
    )

    using namespace Jafg;
    std::size_t Cursor = InCursor;

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

        if (LString::StartsWith(InContentF.begin() + Cursor, InContentF.end(), InKey.begin(), InKey.end()) == false)
        {
            ::GoToNextLineStart(InContentF, &Cursor);
            continue;
        }

        const std::size_t OutCandidate = Cursor;

        Cursor += InKey.GetRuneCount();
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

Jafg::std::optional<LMutableStringView> FindKeyValue(LString& InContentF, const std::size_t& InCursor, const LStringView& InKey)
{
    Jafg::std::optional<std::size_t> Key = FindKey(InContentF, InCursor, InKey);
    if (!Key)
    {
        return { };
    }

    std::size_t Cursor = *Key;
    Cursor += InKey.GetRuneCount();
    check( InContentF.IsValidIndex(Cursor) && InContentF[Cursor] == '=' )
    Cursor += /* = */1;

    const auto Begin = InContentF.begin() + Cursor;
    while (InContentF.IsValidIndex(Cursor))
    {
        if (InContentF[Cursor] == ';' || InContentF[Cursor] == '\n' || InContentF[Cursor] == '\r')
        {
            break;
        }

        ++Cursor;

        continue;
    }

    const auto End = InContentF.begin() + Cursor;

    return LStringView{Begin, End};
}

bool Serialize(LString* ContentF, const LStringView& InSection, const LStringView& InKey, const LStringView& InValue)
{
    using namespace Jafg;

    checkSlow( ContentF )

    if (std::optional<LStringView> DeserializedValue = ::Deserialize(*ContentF, InSection, InKey); DeserializedValue.IsValid())
    {
        LStringView& Value = *DeserializedValue;
        if (Value.Equals(InValue.begin(), InValue.end()) == false)
        {
            ContentF->Substitute(Value.begin(), Value.end(), InValue.begin(), InValue.end());
            return true;
        }

        return false;
    }

    std::optional<std::size_t> SectionMaybe = ::FindSection(*ContentF, InSection);
    if (!SectionMaybe)
    {
        ContentF->append(Jafg::SprintF("[{}]\n", InSection));
        SectionMaybe = ::FindSection(*ContentF, InSection);
    }
    std::size_t Section = *SectionMaybe;

    ::GoToNextLine(*ContentF, &Section);
    ContentF->insert(Section, Jafg::SprintF("{}={}\n", InKey, InValue));

    return true;
}

std::optional<LStringView> Deserialize(LString& InContentF, const LStringView& InSection, const LStringView& InKey)
{
    std::optional<std::size_t> Section = ::FindSection(InContentF, InSection);
    if (!Section)
    {
        return { };
    }
    std::size_t Cursor = *Section;
    ::GoToThisLineStart(InContentF, &Cursor);
    std::optional<std::size_t> KeyCursor = ::FindKey(InContentF, Cursor, InKey);
    if (!KeyCursor)
    {
        return { };
    }

    Cursor = *KeyCursor;

    Cursor += InKey.size();
    check( algo::is_valid_index(InContentF, Cursor) && InContentF[Cursor] == '=' )
    Cursor += /* = */1;

    if (algo::is_valid_index(InContentF, Cursor) == false)
    {
        return { };
    }

    const auto Begin = InContentF.begin() + Cursor;
    while (algo::is_valid_index(InContentF, Cursor))
    {
        if (InContentF[Cursor] == ';' || InContentF[Cursor] == '\n' || InContentF[Cursor] == '\r')
        {
            break;
        }

        ++Cursor;

        continue;
    }

    const auto End = InContentF.begin() + Cursor;

    return LStringView{Begin, End};
}

} /* ~Namespace <Anonymous> */

bool Jafg::ConfigIo::Serialize(const LPath& InPath, const LStringView& InSection, const LStringView& InKey, const LStringView& InValue, const bool bDoBackup /* = true */)
{
    checkSlow( Tasks::IsOnMasterThread() )
    check( Finder::DoesFileExist(InPath) )

    LString ContentF = Finder::ReadFile(InPath);

    const bool bUpdated = ::Serialize(&ContentF, InSection, InKey, InValue);

    if (bUpdated)
    {
        if (bDoBackup)
        {
            Finder::MakeFileBackup(InPath);
        }

        Finder::OverrideFile(InPath, ContentF);

        LOG_VERBOSE(LogConfigIo, "Pushed field [{}::{}] with [{}].", InSection, InKey, InValue);
    }

    return bUpdated;
}

bool Jafg::ConfigIo::SerializeBulk(const LPath& InPath, const TArray<Entry>& InEntries, const bool bDoBackup /* = true */)
{
    checkSlow( Tasks::IsOnMasterThread() )
    check( Finder::DoesFileExist(InPath) )

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
            Finder::MakeFileBackup(InPath);
        }

        Finder::OverrideFile(InPath, ContentF);
    }

    return bUpdated;
}

std::optional<LString> Jafg::ConfigIo::Deserialize(const LPath& InPath, const LStringView& InSection, const LStringView& InKey)
{
    checkSlow( Tasks::IsOnMasterThread() )
    check( Finder::DoesFileExist(InPath) )

    LString ContentF = Finder::ReadFile(InPath);
    std::optional<LStringView> Out = ::Deserialize(ContentF, InSection, InKey);

    if (Out.has_value())
    {
        LOG_VERBOSE(LogConfigIo, "Pulled field [{}::{}] with [{}].", InSection, InKey, *Out);
        return  Out->size() == 0 ? LString{"NULL"} : LString{*Out};
    }

    return { };
}

#endif /* 0 */
