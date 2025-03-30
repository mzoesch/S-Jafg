// Copyright mzoesch. All rights reserved.

#include "System/Paths.h"
#include <filesystem>
#if !PLATFORM_WASM
    #include <fstream>
#endif /* !PLATFORM_WASM */

#if PLATFORM_WINDOWS /* Just some windows nonsense... */
    #pragma push_macro( "CreateFile" )
    #undef CreateFile
#endif /* PLATFORM_WINDOWS */

namespace Jafg
{
LStringLegacy Paths::ReadFileLegacy(const LPath& InAbsolutePath)
{
#if WITH_VIRTUAL_FILESYSTEM
    panicMsgf( "Access to the filesystem is denied on this platform. Tried to access: {}.", InAbsolutePath.GetPath() )
    return { };
#else /* WITH_VIRTUAL_FILESYSTEM */
    const std::ifstream File(InAbsolutePath.GetPath().ToC(), std::ios::in | std::ios::binary);

    if (!File)
    {
        panicMsgf( "Failed to open file: {}.", InAbsolutePath.GetPath() )
        return { };
    }

    std::ostringstream Buffer;
    Buffer << File.rdbuf();
    return Buffer.str();
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

LString Paths::ReadFile(const LPath& InAbsolutePath)
{
#if WITH_VIRTUAL_FILESYSTEM
    panicMsgf( "Access to the filesystem is denied on this platform. Tried to access: {}.", InAbsolutePath.GetPath() )
    return { };
#else /* WITH_VIRTUAL_FILESYSTEM */
    const std::ifstream File(InAbsolutePath.GetPath().ToC(), std::ios::in | std::ios::binary);

    if (!File)
    {
        panicMsgf( "Failed to open file: {}.", InAbsolutePath.GetPath() )
        return { };
    }

    std::ostringstream Buffer;
    Buffer << File.rdbuf();
    return Buffer.str().c_str();
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

LSimpleString Paths::GetStem(const LSimpleString& InFileName)
{
    const i32 LastDot = InFileName.FindLast('.');
    if (LastDot == INDEX_NONE)
    {
        return InFileName;
    }

    return InFileName.Cut(LastDot);
}

void Paths::GetStemInline(LSimpleString& InOutFileName)
{
    const i32 LastDot = InOutFileName.FindLast('.');
    if (LastDot == INDEX_NONE)
    {
        return;
    }

    InOutFileName.InlineCut(LastDot);

    return;
}

bool Paths::DoesFileExist(const LPath& InAbsolutePath)
{
#if WITH_VIRTUAL_FILESYSTEM
    LOG_WARNING(LogSystem, "Access to the filesystem is denied on this platform. Tried to access: {}.", InAbsolutePath.GetPath())
    return false;
#else /* WITH_VIRTUAL_FILESYSTEM */
    return std::filesystem::exists(InAbsolutePath.GetPath().ToC());
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

void Paths::CreateFileSlow(const LPath& InAbsolutePath)
{
#if WITH_VIRTUAL_FILESYSTEM
    LOG_WARNING(LogSystem, "Access to the filesystem is denied on this platform. Tried to access: {}.", InAbsolutePath.GetPath())
    return;
#else /* WITH_VIRTUAL_FILESYSTEM */

    if (DoesFileExist(InAbsolutePath))
    {
        LOG_WARNING(LogPlatform, "Trying to create the file [{}] but it already exists.", InAbsolutePath.GetPath())
        return;
    }

    LOG_TRACE(LogPlatform, "Creating file [{}].", InAbsolutePath.GetPath())
    std::ofstream Out { InAbsolutePath.GetPath().ToC() };
    Out.close();

    if (Out.fail())
    {
        panicMsgf( "Failed to create file: {}.", InAbsolutePath.GetPath() )
    }

    return;
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

void Paths::OverrideFile(const LPath& InAbsolutePath, const LStringView& InContent, const bool bUseNativeLineEndings /* = false */)
{
#if WITH_VIRTUAL_FILESYSTEM
    LOG_WARNING(LogSystem, "Access to the filesystem is denied on this platform. Tried to access: {}.", InAbsolutePath.GetPath())
    return;
#else /* WITH_VIRTUAL_FILESYSTEM */
    Paths::CheckFile(InAbsolutePath);
    std::ofstream Out { InAbsolutePath.GetPath().ToC(), std::ios::out | std::ios::trunc | (bUseNativeLineEndings ? static_cast<std::ios::openmode>(0) : std::ios::binary) };
    Out << InContent.data();
    Out.close();
    return;
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

void Paths::MakeFileBackup(const LPath& InAbsolutePath, const bool bMakeIfSame /* = false */, i32 InBackupCount /* = 5 */, const LStringView& InBackupExtension /* = ".old" */)
{
#if WITH_VIRTUAL_FILESYSTEM
    LOG_WARNING(LogSystem, "Access to the filesystem is denied on this platform. Tried to access: {}.", InAbsolutePath.GetPath())
    return;
#else /* WITH_VIRTUAL_FILESYSTEM */
    namespace Fs = std::filesystem;
    Paths::DoesFileExistPanicked(InAbsolutePath);

    if (bMakeIfSame == false)
    {
        LPath BackupPath = InAbsolutePath;
        BackupPath.GetMutablePath().Append(InBackupExtension.data());
        BackupPath.GetMutablePath().Append("1");
        Paths::CheckFile(BackupPath);
        if (Paths::AreFilesIdentical(InAbsolutePath, BackupPath))
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
        LPath BackupPath = InAbsolutePath;
        BackupPath.GetMutablePath().Append(InBackupExtension.data());
        BackupPath.GetMutablePath().Append(std::to_string(i).c_str());
        LPath PreviousBackupPath = InAbsolutePath;
        PreviousBackupPath.GetMutablePath().Append(InBackupExtension.data());
        PreviousBackupPath.GetMutablePath().Append(std::to_string(i - 1).c_str());

        if (i == 1)
        {
            PreviousBackupPath = InAbsolutePath;
        }

        if (Fs::exists(PreviousBackupPath.GetPath().ToC()))
        {
            Paths::CheckFile(BackupPath);
            Fs::copy_file(PreviousBackupPath.GetPath().ToC(), BackupPath.GetPath().ToC(), Fs::copy_options::overwrite_existing);
        }
    }

    return;
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

void Paths::CheckFile(const LPath& InAbsolutePath)
{
#if WITH_VIRTUAL_FILESYSTEM
    LOG_WARNING(LogSystem, "Access to the filesystem is denied on this platform. Tried to access: {}.", InAbsolutePath.GetPath())
    return;
#else /* WITH_VIRTUAL_FILESYSTEM */
    if (DoesFileExist(InAbsolutePath) == false)
    {
        Paths::CreateFileSlow(InAbsolutePath);
    }
    return;
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

bool Paths::AreFilesIdentical(const LPath& InFirst, const LPath& InSecond)
{
#if WITH_VIRTUAL_FILESYSTEM
    LOG_WARNING(LogSystem, "Access to the filesystem is denied on this platform. Tried to access: {}.", InFirst.GetPath())
    LOG_WARNING(LogSystem, "Access to the filesystem is denied on this platform. Tried to access: {}.", InSecond.GetPath())
    return false;
#else /* WITH_VIRTUAL_FILESYSTEM */
    namespace Fs = std::filesystem;

    std::ifstream f1(InFirst.GetPath().ToC(), std::ios::binary);
    std::ifstream f2(InSecond.GetPath().ToC(), std::ios::binary);

    if (!f1 || !f2)
    {
        panicMsgf( "Error opening files: [{}] || [{}].", InFirst.GetPath(), InSecond.GetPath() )
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
#endif /* !WITH_VIRTUAL_FILESYSTEM */
}

} /* ~Namespace Jafg::Paths */

#if PLATFORM_WINDOWS
    #pragma pop_macro( "CreateFile" )
#endif /* PLATFORM_WINDOWS */
