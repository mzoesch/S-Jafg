// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "System/SystemForward.h"

namespace Jafg
{

class JUserPreferences;

//#
//# The finder is a high-level file-system platform abstraction. All functions defined below work on any platform.
//# Reading, checking for file existence, walking along directories, etc.
//# The finder is not capable of creating and reading files (as this would not be possible on all platforms).
//# If you need to create / write files, use the Jafg::Paths namespace instead.
//#
namespace Finder
{

ENGINE_API LPath GetEngineRootDir();
ENGINE_API LPath GetSavedDir();
ENGINE_API LPath GetUserPreferencesFile();

//#
//# Reads a file content.
//# If an embedded version of the file is found in the engine, this function will return that. If not it will try to
//# ask the current platform to read the file.
//# This function will panic if something goes wrong.
//# @return The file content.
//# @remark Works with embedded files.
//#
ENGINE_API LString ReadFile(const LEnginePath& InEnginePath);
ENGINE_API void ReadFileAsBinary(const LEnginePath& InEnginePath, const u8*& OutBuffer, u64& OutBufferOverflowGuard);
ENGINE_API void FreeReadFileBinaryBuffer(const u8*& InBuffer);

//#
//# @return True if the path / file exists.
//# @remark Works with embedded files.
//#
ENGINE_API bool DoesFileExists(const LEnginePath& InEnginePath);
ENGINE_API bool DoesFileExistsChecked(const LEnginePath& InEnginePath);
ENGINE_API bool DoesFileExistsPanicked(const LEnginePath& InEnginePath);

//#
//# Ensures a file existence. If it does not exist, it will try to create it if filesystem access to the
//# supporting platform is allowed. Otherwise, it will panic.
//#
ENGINE_API void EnsureFile(const LEnginePath& InEnginePath);

//#
//# Checks if a file exists. If it does not exist the program will panic.
//#
ENGINE_API void CheckFile(const LEnginePath& InEnginePath);

ENGINE_API LPath ResolvePathToRelativeModulePath(const LEnginePath& InEnginePath);
ENGINE_API LPath ResolvePathToRelativeEnginePath(const LEnginePath& InEnginePath, const JUserPreferences& InUserPreferences);
ENGINE_API LPath ResolvePathToAbsolutePath(const LEnginePath& InEnginePath, const JUserPreferences& InUserPreferences);
ENGINE_API LPath ResolvePathToAbsolutePath(const EEnginePaths::Type& InEnginePath, const JUserPreferences& InUserPreferences);

ENGINE_API TArray<LString> FindFiles(
    const LPath& InAbsolutePath,
    const bool bKeepExtension = false,
    const LString& InFileExtension = ".*"
);
ENGINE_API TArray<LString> FindFiles(
    const EEnginePaths::Type InEnginePathTy,
    const JUserPreferences& InUserPreferences,
    const bool bKeepExtension = false,
    const LString& InFileExtension = ".*"
);

} /* ~Namespace Finder */

} /* ~Namespace Jafg */
