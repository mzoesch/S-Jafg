// Copyright mzoesch. All rights reserved.

#pragma once

//#
//# Functions in this namespace will panic if something goes wrong unless stated otherwise.
//#
namespace Finder
{

LAL_API LPath GetCwd();

inline LPath GetSavedDir() noexcept { return LPath{ "Saved" }; }
inline LPath GetUserPreferencesFile() noexcept { LPath Out { GetSavedDir() }; Out.Append("/MyPreferences.cfg"); return Out; }
inline LPath GetSavesDir() noexcept { LPath Out { GetSavedDir() }; Out.Append("/Saves"); return Out; }
inline LPath GetDumpsDir() noexcept { LPath Out { GetSavedDir() }; Out.Append("/Dumps"); return Out; }
inline LPath GetMostRecentMemDumpFile() noexcept { LPath Out { GetDumpsDir() }; Out.Append("/proc.dmp"); return Out; }

LAL_API bool DoesExist(const LPathView& Path);
inline  bool DoesExistChecked(const LPathView& Path) { const bool bOut { DoesExist(Path) }; check( bOut ) return bOut; }
inline  bool DoesExistAsserted(const LPathView& Path) { const bool bOut { DoesExist(Path) }; jassert( bOut ) return bOut; }

LAL_API bool DoesFileExist(const LPathView& File);
inline  bool DoesFileExistChecked(const LPathView& File) { const bool bOut { DoesFileExist(File) }; check( bOut ) return bOut; }
inline  bool DoesFileExistAsserted(const LPathView& File) { const bool bOut { DoesFileExist(File) }; jassert( bOut ) return bOut; }

LAL_API bool DoesDirectoryExist(const LPathView& Directory);
inline  bool DoesDirectoryExistChecked(const LPathView& Directory) { const bool bOut { DoesDirectoryExist(Directory) }; check( bOut ) return bOut; }
inline  bool DoesDirectoryExistAsserted(const LPathView& Directory) { const bool bOut { DoesDirectoryExist(Directory) }; jassert( bOut ) return bOut; }

//#
//# Ensure a file. If it does not exist, it will be created along with their parent directories if necessary.
//# This is a safe function that will panic if the path is outside the engine root directory.
//#
LAL_API void EnsureFile(const LPathView& File);
//# Check a file. If it does not exist, the program will panic. This function may read any file.
LAL_API void CheckFile(const LPathView& File);

LAL_API bool AreFilesIdentical(const LPathView& A, const LPathView& B);

LAL_API void CreateFile(const LPathView& File, const bool bMakeParents = false);
LAL_API void CreateDirectories(const LPathView& Directory);

//#
//# Reads a file from the platform.
//# This function will panic if something goes wrong.
//# @note This function will panic if the file does not exist or access to the filesystem is denied.
//#
LAL_API LString    ReadFile(const LPathView& File);
LAL_API TArray<u8> ReadFileAsBinary(const LPathView& File);

//# Same as #ReadFile but will not panic, instead write the error message to the provided string if available.
LAL_API Jafg::TOptional<LString>    TryReadFile(const LPathView& File, LString* OutHumanReadableError = nullptr);
LAL_API Jafg::TOptional<TArray<u8>> TryReadFileAsBinary(const LPathView& File, LString* OutHumanReadableError = nullptr);

LAL_API void OverrideFile(const LPathView& File, const LStringView& Content, const bool bUseNativeLineEndings = false);

LAL_API void MakeFileBackup(const LPathView& File, const bool bMakeIfSame = false, i32 Count = 5, const LStringView& Extension = ".old");

//#
//# Searches the given directory for files with the given extension following std regrex.
//#
LAL_API TArray<LString> FindFiles
(
    const LPathView& Directory,
    const bool bKeepExtension = true,
    const LStringView& Extension = "*",
    const std::regex_constants::syntax_option_type Options = std::regex_constants::ECMAScript
);

//#
//# Searches the given directory for files with the given extension following std regex.
//#
LAL_API TArray<LString> FindFilesRecursively
(
    const LPathView& Directory,
    const bool bKeepExtension = true,
    const LStringView& Extension = "*",
    const std::regex_constants::syntax_option_type Options = std::regex_constants::ECMAScript
);

//#
//# Finds all files with the given name in the given directory and all subdirectories.
//#
LAL_API TArray<LString> FindFilesRecursivelyByName
(
    const LPathView& Directory,
    const LStringView& FileName
);

} /* ~Namespace Finder */
