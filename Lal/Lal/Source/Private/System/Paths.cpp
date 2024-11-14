// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "System/Paths.h"
#include <filesystem>

namespace Jafg
{

bool Paths::DoesFileExist(const LPath& InAbsolutePath)
{
    return std::filesystem::exists(InAbsolutePath.GetPath().ToC());
}

} /* ~Namespace Jafg::Paths */
