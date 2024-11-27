// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "System/Path.h"

namespace Jafg
{

namespace Paths
{

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
