// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "System/SystemForward.h"

namespace Jafg
{

class JUserPreferences;

namespace Finder
{

ENGINE_API LPath GetEngineRootDir();

//#
//# Reads a file content.
//# If an embedded version of the file is found in the engine, this function will return that. If not it will try to
//# ask the current platform to read the file.
//# This function will panic if something goes wrong.
//# @return The file content.
//# @remark Works with embedded files.
//#
ENGINE_API LStringLegacy ReadFile(const LEnginePath& InEnginePath);
ENGINE_API void ReadFileAsBinary(const LEnginePath& InEnginePath, const uint8*& OutBuffer, uint64& OutBufferOverflowGuard);
ENGINE_API void FreeReadFileBinaryBuffer(const uint8*& InBuffer);

//#
//# @return True if the path / file exists.
//# @remark Works with embedded files.
//#
ENGINE_API bool DoesExists(const LEnginePath& InEnginePath);
ENGINE_API bool DoesExistsChecked(const LEnginePath& InEnginePath);
ENGINE_API bool DoesExistsPanicked(const LEnginePath& InEnginePath);

ENGINE_API LPath ResolvePathToRelativeModulePath(const LEnginePath& InEnginePath);
ENGINE_API LPath ResolvePathToRelativeEnginePath(const LEnginePath& InEnginePath, const JUserPreferences& InUserPreferences);
ENGINE_API LPath ResolvePathToAbsolutePath(const LEnginePath& InEnginePath, const JUserPreferences& InUserPreferences);
ENGINE_API LPath ResolvePathToAbsolutePath(const EEnginePaths::Type& InEnginePath, const JUserPreferences& InUserPreferences);

ENGINE_API TdhArray<LSimpleString> FindFiles(
    const LPath& InAbsolutePath,
    const bool bKeepExtension = false,
    const LSimpleString& InFileExtension = ".*"
);
ENGINE_API TdhArray<LSimpleString> FindFiles(
    const EEnginePaths::Type InEnginePathTy,
    const JUserPreferences& InUserPreferences,
    const bool bKeepExtension = false,
    const LSimpleString& InFileExtension = ".*"
);

} /* ~Namespace Finder */

} /* ~Namespace Jafg */
