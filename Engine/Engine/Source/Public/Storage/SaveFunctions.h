// Copyright mzoesch. All rights reserved.

#pragma once

#include "System/Path.h"

namespace Jafg::Saves
{

ENGINE_API TOptional<LString> GetDisplayName(const LPath& InPath, LString* OutError = nullptr);

} /* ~Namespace Jafg::Saves */
