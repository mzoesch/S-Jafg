// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "System/Path.h"

#if WITH_VIRTUAL_FILESYSTEM
    #error "This filesystem abstraction is not available when using a virtual filesystem."
#endif /* WITH_VIRTUAL_FILESYSTEM */

#if PLATFORM_WINDOWS /* Just some windows nonsense... */
    #pragma push_macro( "CreateFile" )
    #undef CreateFile
#endif /* PLATFORM_WINDOWS */

namespace Jafg
{

//#
//# An intermediate file-system platform abstraction that works only on platforms that provide an actual file system.
//#
//# This namespace provides "low level" file access functions if you need more advanced file-system operations, use the
//# appropriate functions from the Jafg::Finder namespace in the engine module (they will include deferred file-paths
//# based of user-preferences that are only evaluated when directly asking the supporting platform.), advanced file
//# search functions, etc.
//#
namespace Paths
{

LAL_API LPath GetMemoryDumpFilePath();

//#
//# Reads a file from the platform.
//# This function will panic if something goes wrong.
//# @note This function will panic if the file does not exist or access to the filesystem is denied. If you need to
//#       access embedded files, use the appropriate function from the Finder namespace in the engine module.
//#
LAL_API LString    ReadFile(const LPath& InFilePath);
LAL_API TArray<u8> ReadFileAsBinary(const LPath& InFilePath);

//# Same as #ReadFile but will not panic, instead write the error message to the provided string if available.
LAL_API TOptional<LString> TryReadFile(const LPath& InFilePath, LString* OutHumanReadableError = nullptr);

//#
//# File or path. Does not matter.
//#
//# @return The stem:
//#    "A.b"   ->   "A"
//#    "A.b.c" -> "A.b"
//#    "A"     -> "A"
//#    ""      -> ""
//#
LAL_API LString GetStem(const LString& InFileName);
LAL_API void GetStemInline(LString& InOutFileName);

//# Check if the given path exists.
LAL_API bool DoesPathExist(const LPath& InPath);
inline  bool DoesPathExistChecked(const LPath& InPath);
inline  bool DoesPathExistAsserted(const LPath& InPath);

//# Check if the given path exists and is a file.
LAL_API bool DoesFileExist(const LPath& InFilePath);
inline  bool DoesFileExistChecked(const LPath& InFilePath);
inline  bool DoesFileExistAsserted(const LPath& InFilePath);

//# Check if the given path exists and is a directory.
LAL_API bool DoesDirExist(const LPath& InDirPath);
inline  bool DoesDirExistChecked(const LPath& InDirPath);
inline  bool DoesDirExistAsserted(const LPath& InDirPath);

LAL_API bool IsAbsolute(const LPath& InPath);
LAL_API bool IsRelative(const LPath& InPath);

//#
//# Creates a file if it does not exist.
//# If something goes wrong, this program will panic.
//#
LAL_API void CreateFileSlow(const LPath& InFilePath, const bool bMakeParents = false);
LAL_API void CreateDirectories(const LPath& InDirPath);
//# Override a file with the provided content. If the file does not exist, it will be created.
LAL_API void OverrideFile(const LPath& InFileName, const LStringView& InContent, const bool bUseNativeLineEndings = false);

//# @param InBackupCount At least one.
LAL_API void MakeFileBackup(const LPath& InFileName, const bool bMakeIfSame = false, const i32 InBackupCount = 5, const LStringView& InBackupExtension = ".old");

//# Ensure a file. If it does not exist, it will be created (and their parent directories if necessary).
LAL_API void EnsureFile(const LPath& InFileName);
//# Check a file. If it does not exist, the program will panic.
LAL_API void CheckFile(const LPath& InFileName);

LAL_API bool AreFilesIdentical(const LPath& InFirst, const LPath& InSecond);

} /* ~Namespace Paths */

inline bool Paths::DoesPathExistChecked(const LPath& InPath)
{
    const bool Out = Paths::DoesPathExist(InPath);
    check( Out )
    return Out;
}

inline bool Paths::DoesPathExistAsserted(const LPath& InPath)
{
    const bool Out = Paths::DoesPathExist(InPath);
    jassert( Out )
    return Out;
}

inline bool Paths::DoesFileExistChecked(const LPath& InFilePath)
{
    const bool Out = Paths::DoesFileExist(InFilePath);
    check( Out )
    return Out;
}

inline bool Paths::DoesFileExistAsserted(const LPath& InFilePath)
{
    const bool Out = Paths::DoesFileExist(InFilePath);
    jassert( Out )
    return Out;
}

inline bool Paths::DoesDirExistChecked(const LPath& InDirPath)
{
    const bool Out = Paths::DoesDirExist(InDirPath);
    check( Out )
    return Out;
}

inline bool Paths::DoesDirExistAsserted(const LPath& InDirPath)
{
    const bool Out = Paths::DoesDirExist(InDirPath);
    jassert( Out )
    return Out;
}

} /* ~Namespace Jafg */

#if PLATFORM_WINDOWS
    #pragma pop_macro( "CreateFile" )
#endif /* PLATFORM_WINDOWS */
