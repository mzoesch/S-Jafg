// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg::Str
{

bool IsValidAscii(const char* InString, const int32 InRuneLenghtToCheck = -1);
bool IsValidUtf8(const char* InString, const int32 InRuneLenghtToCheck = -1);

} /* ~Namespace Jafg::Str */
