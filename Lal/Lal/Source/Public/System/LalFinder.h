// Copyright mzoesch. All rights reserved.

#pragma once

#if PLATFORM_WINDOWS /* Just some windows nonsense... */
    #ifdef CreateFile
        #pragma push_macro( "CreateFile" )
        #undef CreateFile
    #endif /* CreateFile */
#endif /* PLATFORM_WINDOWS */

//#
//# Functions in this namespace will panic if something goes wrong unless stated otherwise.
//#
namespace Finder
{

LAL_API LPath GetCwd();

inline LPath GetSavedDir() noexcept { return LPath{ "Saved" }; }
inline LPath GetUserPreferencesFile() noexcept { LPath Out { GetSavedDir() }; Out.concat("/MyPreferences.cfg"); return Out; }
inline LPath GetSavesDir() noexcept { LPath Out { GetSavedDir() }; Out.concat("/Saves"); return Out; }
inline LPath GetDumpsDir() noexcept { LPath Out { GetSavedDir() }; Out.concat("/Dumps"); return Out; }
inline LPath GetMostRecentMemDumpFile() noexcept { LPath Out { GetDumpsDir() }; Out.concat("/proc.dmp"); return Out; }

LAL_API bool DoesExist(const LPath& Path);
inline  bool DoesExistChecked(const LPath& Path) { const bool bOut { DoesExist(Path) }; check( bOut ) return bOut; }
inline  bool DoesExistAsserted(const LPath& Path) { const bool bOut { DoesExist(Path) }; jassert( bOut ) return bOut; }

LAL_API bool DoesFileExist(const LPath& File);
inline  bool DoesFileExistChecked(const LPath& File) { const bool bOut { DoesFileExist(File) }; check( bOut ) return bOut; }
inline  bool DoesFileExistAsserted(const LPath& File) { const bool bOut { DoesFileExist(File) }; jassert( bOut ) return bOut; }

LAL_API bool DoesDirectoryExist(const LPath& Directory);
inline  bool DoesDirectoryExistChecked(const LPath& Directory) { const bool bOut { DoesDirectoryExist(Directory) }; check( bOut ) return bOut; }
inline  bool DoesDirectoryExistAsserted(const LPath& Directory) { const bool bOut { DoesDirectoryExist(Directory) }; jassert( bOut ) return bOut; }

//#
//# Ensure a file. If it does not exist, it will be created along with their parent directories if necessary.
//# This is a safe function that will panic if the path is outside the engine root directory.
//#
LAL_API void EnsureFile(const LPath& File);
//# Check a file. If it does not exist, the program will panic. This function may read any file.
LAL_API void CheckFile(const LPath& File);

LAL_API bool AreFilesIdentical(const LPath& A, const LPath& B);

LAL_API void CreateFile(const LPath& File, const bool bMakeParents = false);
LAL_API void CreateDirectories(const LPath& Directory);

//#
//# Reads a file from the platform.
//# This function will panic if something goes wrong.
//# @note This function will panic if the file does not exist or access to the filesystem is denied.
//#
LAL_API LString    ReadFile(const LPath& File);
LAL_API TArray<u8> ReadFileAsBinary(const LPath& File);

//# Same as #ReadFile but will not panic, instead write the error message to the provided string if available.
LAL_API TOptional<LString>    TryReadFile(const LPath& File, LString* OutHumanReadableError = nullptr);
LAL_API TOptional<TArray<u8>> TryReadFileAsBinary(const LPath& File, LString* OutHumanReadableError = nullptr);

LAL_API void OverrideFile(const LPath& File, const LStringView& Content, const bool bUseNativeLineEndings = false);

LAL_API void MakeFileBackup(const LPath& File, const bool bMakeIfSame = false, i32 Count = 5, const LStringView& Extension = ".old");

//#
//# Searches the given directory for files with the given extension following std regrex.
//#
LAL_API TArray<LString> FindFiles
(
    const LPath& Directory,
    const bool bKeepExtension = true,
    const LStringView& Extension = "*",
    const std::regex_constants::syntax_option_type Options = std::regex_constants::ECMAScript
);

//#
//# Searches the given directory for files with the given extension following std regex.
//#
LAL_API TArray<LString> FindFilesRecursively
(
    const LPath& Directory,
    const bool bKeepExtension = true,
    const LStringView& Regex = "*",
    const std::regex_constants::syntax_option_type Options = std::regex_constants::ECMAScript
);

//#
//# Finds all files with the given name in the given directory and all subdirectories.
//#
LAL_API TArray<LString> FindFilesRecursivelyByName
(
    const LPath& Directory,
    const LStringView& FileName
);

} /* ~Namespace Finder */
