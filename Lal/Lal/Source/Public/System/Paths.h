// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "System/Path.h"

namespace Jafg
{

namespace Paths
{

/**
 * Reads a file from the platform.
 * This function will panic if something goes wrong.
 * @note This function will panic if the file does not exist or access to the filesystem is denied. If you need to
 *       access embedded files, use the appropriate function from the Finder namespace in the engine module.
 * @remark Does not work with embedded files.
 */
LStringLegacy ReadFile(const LPath& InAbsolutePath);

/**
 * @return The stem:
 *    "A.b"   ->   "A"
 *    "A.b.c" -> "A.b"
 *    "A"     -> "A"
 *    ""      -> ""
 */
LSimpleString GetStem(const LSimpleString& InFileName); /* File or path. Does not matter. */
void GetStemInline(LSimpleString& InOutFileName);

/**
 * @remark Does not work with embedded files.
 */
bool DoesFileExist(const LPath& InAbsolutePath);
bool DoesFileExistChecked(const LPath& InAbsolutePath);
bool DoesFileExistPanicked(const LPath& InAbsolutePath);

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
        panicMsgf( "File does not exist: %s.", InAbsolutePath.GetPath() )
    }
    return Out;
}

} /* ~Namespace Jafg */
