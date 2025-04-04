// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "System/Path.h"

#if PLATFORM_WINDOWS /* Just some windows nonsense... */
    #pragma push_macro( "CreateFile" )
    #undef CreateFile
#endif /* PLATFORM_WINDOWS */

namespace Jafg
{

//#
//# An intermediate file-system platform abstraction that works only on platforms that provide an actual file system.
//# All paths have to be absolute to ensure compatibility with all platforms.
//# This namespace provides the bare minimum of functions if you need more advanced file-system operations, use the
//# appropriate functions from the Jafg::Finder namespace in the engine module (they will include deferred file-paths
//# based of user-preferences that are only evaluated when directly asking the supporting platform.).
//#
namespace Paths
{

/**
 * Reads a file from the platform.
 * This function will panic if something goes wrong.
 * @note This function will panic if the file does not exist or access to the filesystem is denied. If you need to
 *       access embedded files, use the appropriate function from the Finder namespace in the engine module.
 * @remark Does not work with embedded files.
 */
LStringLegacy ReadFileLegacy(const LPath& InAbsolutePath);
LString ReadFile(const LPath& InAbsolutePath);

/**
 * @return The stem:
 *    "A.b"   ->   "A"
 *    "A.b.c" -> "A.b"
 *    "A"     -> "A"
 *    ""      -> ""
 */
LString GetStem(const LString& InFileName); /* File or path. Does not matter. */
void GetStemInline(LString& InOutFileName);

/**
 * @remark Does not work with embedded files.
 */
bool DoesFileExist(const LPath& InAbsolutePath);
bool DoesFileExistChecked(const LPath& InAbsolutePath);
bool DoesFileExistPanicked(const LPath& InAbsolutePath);

void CreateFileSlow(const LPath& InAbsolutePath);
void OverrideFile(const LPath& InAbsolutePath, const LStringView& InContent, const bool bUseNativeLineEndings = false);
/** @param InBackupCount At least one. */
void MakeFileBackup(const LPath& InAbsolutePath, const bool bMakeIfSame = false, const i32 InBackupCount = 5, const LStringView& InBackupExtension = ".old");

/**
 * Check a file.
 * Does nothing if the specified file exists but will create it if it does not exist.
 * @remark Does not work with embedded files.
 */
void CheckFile(const LPath& InAbsolutePath);
bool AreFilesIdentical(const LPath& InFirst, const LPath& InSecond);

} /* ~Namespace Paths */

inline bool Paths::DoesFileExistChecked(const LPath& InAbsolutePath)
{
    const bool Out = Paths::DoesFileExist(InAbsolutePath);
    check( Out )
    return Out;
}

inline bool Paths::DoesFileExistPanicked(const LPath& InAbsolutePath)
{
    const bool Out = Paths::DoesFileExist(InAbsolutePath);
    if (Out == false)
    {
        panicMsgf( "File does not exist: %s.", InAbsolutePath.GetPath().ToC() )
    }
    return Out;
}

} /* ~Namespace Jafg */

#if PLATFORM_WINDOWS
    #pragma pop_macro( "CreateFile" )
#endif /* PLATFORM_WINDOWS */
