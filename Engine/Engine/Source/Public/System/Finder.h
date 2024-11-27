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
