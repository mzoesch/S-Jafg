// Copyright mzoesch. All rights reserved.

#if !WITH_VIRTUAL_FILESYSTEM

#include "System/Paths.h"
#include <filesystem>
#include <fstream>

#if PLATFORM_WINDOWS /* Just some windows nonsense... */
    #pragma push_macro( "CreateFile" )
    #undef CreateFile
#endif /* PLATFORM_WINDOWS */

namespace Fs = std::filesystem;

namespace Jafg
{

namespace Private
{

bool TPathBase_DoesExist(const LPath& InPath)
{
    return Paths::DoesPathExist(InPath);
}

bool TPathBase_IsFile(const LPath& InPath)
{
    return Paths::DoesFileExist(InPath);
}

bool TPathBase_IsDir(const LPath& InPath)
{
    return Paths::DoesDirExist(InPath);
}

bool TPathBase_IsAbsolute(const LPath& InPath)
{
    return Paths::IsAbsolute(InPath);
}

bool TPathBase_IsRelative(const LPath& InPath)
{
    return Paths::IsRelative(InPath);
}

} /* ~Namespace Private */

LPath Paths::GetMemoryDumpFilePath()
{
    return "Saved/Dumps/proc.dmp";
}

LString Paths::ReadFile(const LPath& InFilePath)
{
    LString Error;
    const TOptional<LString> FileContent = Paths::TryReadFile(InFilePath, &Error);
    if (!FileContent)
    {
        panicMsgf( "{}", Error )
        return { };
    }

    return FileContent.GetValue();
}

TArray<u8> Paths::ReadFileAsBinary(const LPath& InFilePath)
{
    std::ifstream File
    (
        InFilePath.ToPtr(),
        std::ios::binary | std::ios::ate
    );

    if (File.fail())
    {
        panicMsgf("Failed to open file at [{}].", InFilePath)
        return { };
    }


    TArray<u8> Buffer; Buffer.AddUninitialized(File.tellg());

    File.seekg(0, std::ios::beg);

    if (File.read(reinterpret_cast<char*>(Buffer.GetDataPointer()), Buffer.GetSize()).fail())
    {
        File.close();
        panicMsgf("Failed to read file at [{}].", InFilePath)
        return { };
    }

    File.close();

    return Buffer;
}

TOptional<LString> Paths::TryReadFile(const LPath& InFilePath, LString* OutHumanReadableError /* = nullptr */)
{
    const std::ifstream File(InFilePath.ToPtr(), std::ios::in | std::ios::binary);

    if (!File)
    {
        if (OutHumanReadableError)
        {
            *OutHumanReadableError = LString::SprintF("Failed to open file: [{}].", InFilePath.ToPtr());
        }

        return { };
    }

    std::ostringstream Buffer;
    Buffer << File.rdbuf();
    return LString(Buffer.str().c_str());
}

LString Paths::GetStem(const LString& InFileName)
{
    const i32 LastDot = InFileName.FindLast('.');
    if (LastDot == INDEX_NONE)
    {
        return InFileName;
    }

    return InFileName.Cut(LastDot);
}

void Paths::GetStemInline(LString& InOutFileName)
{
    const i32 LastDot = InOutFileName.FindLast('.');
    if (LastDot == INDEX_NONE)
    {
        return;
    }

    InOutFileName.InlineCut(LastDot);

    return;
}

bool Paths::DoesPathExist(const LPath& InPath)
{
    return std::filesystem::exists(InPath.ToPtr());
}

bool Paths::DoesFileExist(const LPath& InFilePath)
{
    return std::filesystem::is_regular_file(InFilePath.ToPtr());
}

bool Paths::DoesDirExist(const LPath& InDirPath)
{
    return std::filesystem::is_directory(InDirPath.ToPtr());
}

bool Paths::IsAbsolute(const LPath& InPath)
{
    return std::filesystem::path(InPath.ToPtr()).is_absolute();
}

bool Paths::IsRelative(const LPath& InPath)
{
    return std::filesystem::path(InPath.ToPtr()).is_relative();
}

void Paths::CreateFileSlow(const LPath& InFilePath, const bool bMakeParents /* = false */)
{
    if (DoesFileExist(InFilePath))
    {
        LOG_WARNING(LogPlatform, "Trying to create the file [{}] but it already exists.", InFilePath )
        return;
    }

    if (bMakeParents)
    {
        if (const LPath& Parent = InFilePath.GetParent(); DoesDirExist(Parent) == false)
        {
            LOG_TRACE(LogPlatform, "Creating directory [{}].", Parent)
            std::filesystem::create_directories(Parent.ToPtr());
        }
    }

    LOG_TRACE(LogPlatform, "Creating file [{}].", InFilePath )
    std::ofstream Out { InFilePath.ToPtr() };
    Out.close();

    if (Out.fail())
    {
        panicMsgf( "Failed to create file: {}.", InFilePath )
    }

    return;
}

void Paths::CreateDirectories(const LPath& InDirPath)
{
    std::filesystem::create_directories(InDirPath.ToPtr());
}

void Paths::OverrideFile(const LPath& InFileName, const LStringView& InContent, const bool bUseNativeLineEndings /* = false */)
{
    Paths::EnsureFile(InFileName);

    std::ofstream Out
    {
        InFileName.ToPtr(),
        std::ios::out | std::ios::trunc | (bUseNativeLineEndings ? static_cast<std::ios::openmode>(0) : std::ios::binary)
    };
    Out.write(InContent.GetBegin(), InContent.GetSize());
    Out.close();

    LOG_TRACE(LogSystem, "File [{}] overridden.", InFileName )

    return;
}

void Paths::MakeFileBackup(const LPath& InFileName, const bool bMakeIfSame /* = false */, i32 InBackupCount /* = 5 */, const LStringView& InBackupExtension /* = ".old" */)
{
    LOG_VERBOSE(LogSystem, "Making backup of file [{}].", InFileName )

    check( InBackupCount > 0 )

    Paths::DoesFileExistAsserted(InFileName);

    if (bMakeIfSame == false)
    {
        LPath BackupPath = InFileName;
        BackupPath.Append(InBackupExtension.GetBegin(), InBackupExtension.GetSize());
        BackupPath.Append("1");
        Paths::EnsureFile(BackupPath);
        if (Paths::AreFilesIdentical(InFileName, BackupPath))
        {
            return;
        }
    }

    if (InBackupCount < 1)
    {
        InBackupCount = 1;
    }

    for (int i = InBackupCount; i > 0; --i)
    {
        LPath BackupPath = InFileName;
        BackupPath.Append(InBackupExtension.GetBegin(), InBackupExtension.GetSize());
        BackupPath.Append(std::to_string(i).c_str());
        LPath PreviousBackupPath = InFileName;
        PreviousBackupPath.Append(InBackupExtension.GetBegin(), InBackupExtension.GetSize());
        PreviousBackupPath.Append(std::to_string(i - 1).c_str());

        if (i == 1)
        {
            PreviousBackupPath = InFileName;
        }

        if (Fs::exists(PreviousBackupPath.ToPtr()))
        {
            Paths::EnsureFile(BackupPath);
            Fs::copy_file(PreviousBackupPath.ToPtr(), BackupPath.ToPtr(), Fs::copy_options::overwrite_existing);
        }
    }

    return;
}

void Paths::EnsureFile(const LPath& InFileName)
{
    if (DoesFileExist(InFileName) == false)
    {
        CreateFileSlow(InFileName);
    }

    return;
}

void Paths::CheckFile(const LPath& InFileName)
{
    jassert( DoesFileExist(InFileName) )
}

bool Paths::AreFilesIdentical(const LPath& InFirst, const LPath& InSecond)
{

    std::ifstream f1(InFirst.ToPtr(), std::ios::binary);
    std::ifstream f2(InSecond.ToPtr(), std::ios::binary);

    if (!f1 || !f2)
    {
        panicMsgf( "Error opening files: [{}] || [{}].", InFirst, InSecond )
        return false;
    }

    char c1;
    char c2;
    while (true)
    {
        if (f1.get(c1).fail())
        {
            f2.get(c2);
            break;
        }
        if (f2.get(c2).fail())
        {
            break;
        }

        if (c1 != c2)
        {
            return false;
        }

        continue;
    }

    return f1.eof() && f2.eof();
}

} /* ~Namespace Jafg::Paths */

#if PLATFORM_WINDOWS
    #pragma pop_macro( "CreateFile" )
#endif /* PLATFORM_WINDOWS */

#endif /* !WITH_VIRTUAL_FILESYSTEM */
