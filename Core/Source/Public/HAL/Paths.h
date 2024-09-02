// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Paths
{

TChar GetNormalPathSeparator();

String Normalize(const String& Path);
/** Normalize and combine. */
String Combine(const String& A, const String& B);
/** Use if paths are already normalized. */
String Combine(const String& A, const String& B, const bool bPreventNormalization);
/** Combine with a specified separator. */
String Combine(const String& A, const String& B, const bool bPreventNormalization, const TChar Separator);

String EngineRootDir();

}
