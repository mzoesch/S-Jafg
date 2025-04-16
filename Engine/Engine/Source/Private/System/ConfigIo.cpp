// Copyright mzoesch. All rights reserved.

#include "System/ConfigIo.h"
#include "Async/TaskUtility.h"
#include "System/Paths.h"

using namespace Jafg;

namespace
{

void CopyUntilChar(const char* InSource, char* OutDestination, const char InCustomTerminator, const size_t InMaxSize) {
    size_t i = 0;
    while (InSource[i] && InSource[i] != InCustomTerminator && i < InMaxSize - 1)
    {
        OutDestination[i] = InSource[i];
        ++i;
    }
    OutDestination[i] = '\0';

    if (i == InMaxSize)
    {
        panic( "Buffer overflow." )
    }

    return;
}

// @note Remember to free the destination.
void CopyUntilChar(const char* InSource, char** OutDestinationPtr, const char InCustomTerminator)
{
    constexpr size_t BufferSize { 512 };
    checkSlow( *OutDestinationPtr == nullptr )
    *OutDestinationPtr =  static_cast<char*>(::malloc(BufferSize));
    checkSlow( *OutDestinationPtr != nullptr )
    ::memset(*OutDestinationPtr, 0, BufferSize);

    ::CopyUntilChar(InSource, *OutDestinationPtr, InCustomTerminator, BufferSize);

    return;
}

bool StartsWith(const char* InString, const char* InPrefix)
{
    return ::strncmp(InString, InPrefix, strlen(InPrefix)) == 0;
}

// @return True if valid.
bool GoToValue(const char* InString, i32* Cursor)
{
    for (i32 i = *Cursor; i < static_cast<i32>(strlen(InString)); ++i)
    {
        if (InString[i] == '\n')
        {
            panic( "Could not find value of value." )
            return false;
        }

        if (InString[i] == '=')
        {
            *Cursor = ++i;
            return true;
        }

        continue;
    }

    return false;
}

// @return True if valid.
bool GoToKey(const char* InString, const char* InKey, i32* Cursor)
{
    for (i32 i = *Cursor; i < static_cast<i32>(strlen(InString)); ++i)
    {
        if (InString[i] == '[')
        {
            return false;
        }

        if (::StartsWith(&InString[i], InKey) && InString[i+strlen(InKey)] == '=')
        {
            *Cursor = i;
            return true;
        }

        continue;
    }

    return false;
}

// @return True, if end of file was found.
bool GoToNextLine(const LString& InFileContent, i32* Cursor)
{
    for (i32 i = *Cursor; i < InFileContent.GetSize(); ++i)
    {
        if (InFileContent[i] == '\n')
        {
            *Cursor = ++i;
            return false;
        }
    }

    return true;
}

// @return True, if end of file was found.
bool GoToNextMeaningfulLine(const LString& InFileContent, i32* Cursor)
{
    while (true)
    {
        if (::GoToNextLine(InFileContent, Cursor))
        {
            return true;
        }

        if (InFileContent.IsValidIndex(*Cursor) == false)
        {
            return true;
        }

        if (InFileContent[*Cursor] == '\n')
        {
            *Cursor += 1;
            continue;
        }

        break;
    }

    return false;
}

i32 FindSection(const LString& InFileContent, const LStringView& InSection)
{
    for (i32 i = 0; i < InFileContent.GetRuneCount(); ++i)
    {
        const LString::T Char = InFileContent[i];
        if (Char != '[')
        {
            continue;
        }
        if (InFileContent[i+static_cast<i32>(InSection.GetSize())+1] != ']')
        {
            continue;
        }

        const LString::T* Section = &InFileContent.GetUnderlyingDataStructure()[i+1];

        if (::strncmp(Section, InSection.GetBegin(), ::strlen(InSection.GetBegin())) == 0)
        {
            return i;
        }

        continue;
    }

    return INDEX_NONE;
}

void AppendNewSection(LString* InFileContent, const LStringView& InSection)
{
    checkSlow( FindSection(*InFileContent, InSection) == INDEX_NONE )
    InFileContent->Append("[");
    InFileContent->Append(InSection.GetBegin());
    InFileContent->Append("]\n");
    return;
}

} /* ~Namespace <Anonymous> */

bool ConfigIo::Serialize(const LPath& InPath, const LStringView& InSection, const LStringView& InKey, const LStringView& InValue, const bool bDoBackup /* = ture */)
{
    checkSlow( Tasks::IsOnMasterThread() )
    check( Paths::DoesFileExist(InPath) )

    LOG_VERBOSE(LogConfigIo, "Pushing field [{}::{}] with [{}] to [{}].", InSection, InKey, InValue, InPath)

    LString FileContent = Finder::ReadFile(InPath);

    if (const TOptional<LString> StoredValue = Deserialize(InPath, InSection, InKey); StoredValue)
    {
        if (::strcmp(StoredValue.GetValue().ToPtr(), InValue.GetBegin()) == 0)
        {
            return false;
        }
    }

    i32 Cursor = ::FindSection(FileContent, InSection);
    if (Cursor == INDEX_NONE)
    {
        ::AppendNewSection(&FileContent, InSection);
        Cursor = ::FindSection(FileContent, InSection);
        checkSlow( Cursor != INDEX_NONE )
    }
    ::GoToNextMeaningfulLine(FileContent, &Cursor);

    if (::GoToKey(FileContent.ToPtr(), InKey.GetBegin(), &Cursor) == false)
    {
        GoToNextLine(FileContent, &Cursor);
        LString NewFileContent; NewFileContent.Reserve(FileContent.GetSize());
        for (i32 i = 0; i < Cursor; ++i)
        {
            NewFileContent.Add(FileContent[i]);
        }
        NewFileContent += InKey.GetBegin();
        NewFileContent.Add('=');
        NewFileContent += InValue.GetBegin();
        NewFileContent.Add('\n');
        for (i32 i = Cursor; i < FileContent.GetRuneCount(); ++i)
        {
            NewFileContent.Add(FileContent[i]);
        }

        if (bDoBackup)
        {
            Paths::MakeFileBackup(InPath);
        }
        Paths::OverrideFile(InPath, NewFileContent.ToPtr());

        return true;
    }

    if (::GoToValue(FileContent.ToPtr(), &Cursor) == false)
    {
        panic( "Could not find value." )
        return false;
    }

    const i32 ValueSize = static_cast<i32>(::strlen(InValue.GetBegin()));
    i32 ValueCursor = 0;
    bool  bUpdateNew = true;
    for (i32 i = Cursor; i < FileContent.GetRuneCount(); ++i)
    {
        if (ValueSize == ValueCursor)
        {
            if (FileContent[i] == '\n')
            {
                return false;
            }
            bUpdateNew = false;
            break;
        }

        if (InValue[ValueCursor] == '\0')
        {
            break;
        }

        if (FileContent[i] == '\n')
        {
            break;
        }

        if (FileContent[i] != InValue[ValueCursor])
        {
            break;
        }

        ++ValueCursor;
        continue;
    }
    if (bUpdateNew == false)
    {
        return false;
    }

    LString NewFileContent; NewFileContent.Reserve(FileContent.GetSize());
    for (i32 i = 0; i < Cursor; ++i)
    {
        NewFileContent.Add(FileContent[i]);
    }
    if (*NewFileContent.Peek() != '=')
    {
        panic( "Invalid state" )
        return false;
    }
    NewFileContent += InValue.GetBegin();
    NewFileContent.Add('\n');
    if (::GoToNextMeaningfulLine(FileContent, &Cursor) == false)
    {
        for (i32 i = Cursor; i < FileContent.GetRuneCount(); ++i)
        {
            NewFileContent.Add(FileContent[i]);
        }
    }

    if (bDoBackup)
    {
        Paths::MakeFileBackup(InPath);
    }
    Paths::OverrideFile(InPath, NewFileContent.ToPtr());

    return true;
}

TOptional<LString> ConfigIo::Deserialize(const LPath& InPath, const LStringView& InSection, const LStringView& InKey)
{
    checkSlow( Tasks::IsOnMasterThread() )
    check( Finder::DoesFileExists(InPath) )

    LOG_VERBOSE(LogConfigIo, "Pulling field [{}::{}] from [{}].", InSection, InKey, InPath)

    const LString FileContent = Finder::ReadFile(InPath);

    i32 Cursor = ::FindSection(FileContent, InSection);
    if (Cursor == INDEX_NONE)
    {
        return { };
    }
    if (::GoToNextMeaningfulLine(FileContent, &Cursor))
    {
        return { };
    }

    while (true)
    {
        if (::StartsWith(&FileContent.GetUnderlyingDataStructure()[Cursor], InKey.GetBegin()))
        {
            break;
        }
        if (FileContent.GetUnderlyingDataStructure()[Cursor] == '[')
        {
            return { };
        }
        if (::GoToNextMeaningfulLine(FileContent, &Cursor))
        {
            return { };
        }

        continue;
    }

    if (::GoToValue(FileContent.ToPtr(), &Cursor) == false)
    {
        return { };
    }

    LString Out;
    while (FileContent.GetUnderlyingDataStructure()[Cursor] != '\n')
    {
        Out.Append(&FileContent.GetUnderlyingDataStructure()[Cursor], 1);
        ++Cursor;
    }

    return Out;
}
