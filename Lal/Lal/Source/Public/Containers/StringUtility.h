// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg::Str
{

LAL_API bool IsValidAscii(const char* InString, const i32 InRuneLengthToCheck = INDEX_NONE);
LAL_API bool IsValidUtf8(const char* InString, const i32 InRuneLengthToCheck = INDEX_NONE);

} /* ~Namespace Jafg::Str */
