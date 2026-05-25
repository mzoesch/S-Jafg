// Copyright mzoesch. All rights reserved.

#pragma once

#if JAFG_PLATFORM_WINDOWS /* Just some windows nonsense... */
    #ifdef CreateFile
        #pragma push_macro( "CreateFile" )
        #undef CreateFile
    #endif /* CreateFile */
#endif /* JAFG_PLATFORM_WINDOWS */

//#
//# Functions in this namespace will panic if something goes wrong unless stated otherwise.
//#
namespace Finder
{

namespace Detail
{
ENGINE_API LPath GetEngineRootDir();
ENGINE_API LPath GetSelfProcDir();
//#
//# When in a corrupted state, avoid touching the heap. Therefore, this is a legal way to access the
//# path for the target core dump. It is guaranteed to be always valid and accessible.
//#
//# Modifying this variable in any way is UB und a race condition which could lead to a catastrophic SIGSEGV.
//#
ENGINE_API extern LPath DumpFile;
#if JAFG_PLATFORM_LINUX
    //# Same as #DumpFile -- no modifying allowed.
    ENGINE_API extern std::optional<LPath> _gdb;
#endif /* JAFG_PLATFORM_LINUX */
} /* ~Namespace Detail */

inline LPath GetCwd();

//#
//# Everything that should be saved for a longer period of time. But if they get deleted by the user, it's ok.
//#
inline LPath GetTempDir() noexcept { return LPath{"Temp"}; }
inline LPath GetDumpsDir() noexcept { return GetTempDir() / "Dumps"; }
inline LPath GetMostRecentMemDumpFile() noexcept { return GetDumpsDir() / "proc.dmp"; }
inline LPath GetMostRecentStackTraceFile() noexcept { return GetDumpsDir() / "stack.trace"; }

//#
//# User folder. Store user specific stuff here. That should be saved between sessions and for a long time.
//# If a user uninstalls the program, this folder should contain only files that they might want to keep, for a
//# potential reinstallation.
//# This contains user preferences, progression data, etc.
//#
inline LPath GetSavedDir() noexcept { return LPath{"Saved"}; }
inline LPath GetUserPreferencesFile() noexcept { return GetSavedDir() / "MyPreferences.cfg"; }

inline LPath GetContentDir() noexcept { return LPath{"Content"}; }
inline LPath GetMaterialsDir() noexcept { return GetContentDir() / "Materials"; }
inline LPath GetModelsDir() noexcept { return GetContentDir() / "Models"; }
//# Jafg will automatically look for shaders in this directory trailing with ".shader.json".
inline LPath GetShadersDir() noexcept { return GetContentDir() / "Shaders"; }
inline LPath GetTexturesDir() noexcept { return GetContentDir() / "Textures"; }

inline bool DoesExist(LPath const& Path);
inline bool DoesExistChecked(LPath const& Path) { bool bOut{DoesExist(Path)}; check(bOut) return bOut; }
inline bool DoesExistAsserted(LPath const& Path) { bool bOut{DoesExist(Path)}; jassert(bOut) return bOut; }

inline bool DoesFileExist(LPath const& File);
inline bool DoesFileExistChecked(LPath const& File) { bool bOut{DoesFileExist(File)}; check(bOut) return bOut; }
inline bool DoesFileExistAsserted(LPath const& File) { bool bOut{DoesFileExist(File)}; jassert(bOut) return bOut; }

inline bool DoesDirectoryExist(LPath const& Directory);
inline bool DoesDirectoryExistChecked(LPath const& Directory) { bool bOut{DoesDirectoryExist(Directory)}; check(bOut) return bOut; }
inline bool DoesDirectoryExistAsserted(LPath const& Directory) { bool bOut{DoesDirectoryExist(Directory)}; jassert(bOut) return bOut; }

//#
//# Ensure a file. If it does not exist, it will be created along with their parent directories if necessary.
//# This is a safe function that will panic if the path is outside the engine root directory.
//#
inline void EnsureFile(LPath const& File);
//# Check a file. If it does not exist, the program will panic. This function may read any file.
inline void CheckFile(LPath const& File);

inline bool AreFilesIdentical(LPath const& A, LPath const& B);

inline void CreateFile(LPath const& File, bool bMakeParents = false);
inline void CreateDirectories(LPath const& Directory);

//#
//# Reads a file from the platform.
//# This function will panic if something goes wrong.
//# @note This function will panic if the file does not exist or access to the filesystem is denied.
//#
inline LString    ReadFile(LPath const& File);
inline TArray<u8> ReadFileAsBinary(LPath const& File);

//# Same as #ReadFile but will not panic, instead write the error message to the provided string if available.
inline std::optional<LString>    TryReadFile(LPath const& File, LString* OutHumanReadableError = nullptr);
inline std::optional<TArray<u8>> TryReadFileAsBinary(LPath const& File, LString* OutHumanReadableError = nullptr);

inline void OverrideFile(LPath const& File, LStringView Content, bool bUseNativeLineEndings = false);

inline void MakeFileBackup(LPath const& File, bool bMakeIfSame = false, i32 Count = 5, LStringView Extension = ".old");

inline LString Normalize(LString File) noexcept
{
#if JAFG_PLATFORM_WINDOWS
    auto Location{File.find('\\')};
    while (Location != File.npos)
    {
        File[Location] = '/';
        Location = File.find('\\', Location);
    }
#endif /* JAFG_PLATFORM_WINDOWS */
    return File;
}

//#
//# Searches the given directory for files with the given extension following std regrex.
//#
inline TArray<LPath> FindFiles
(
    const LPath& Directory,
    const bool bKeepExtension = true,
    const LStringView& Extension = "*",
    const std::regex_constants::syntax_option_type Options = std::regex_constants::ECMAScript
);

//#
//# Searches the given directory for files with the given extension following std regex.
//#
inline TArray<LPath> FindFilesRecursively
(
    const LPath& Directory,
    const bool bKeepExtension = true,
    LStringView Regex = "*",
    const std::regex_constants::syntax_option_type Options = std::regex_constants::ECMAScript
);

//#
//# Finds all files with the given name in the given directory and all subdirectories.
//#
inline TArray<LPath> FindFilesRecursivelyByName
(
    LPath const& Directory,
    std::basic_string_view<LPath::value_type> FileName
);

} /* ~Namespace Finder */

#if JAFG_PLATFORM_USES_STD_FINDER
    #include "System/StdFinder.h"
#endif /* JAFG_PLATFORM_USES_STD_FINDER */
