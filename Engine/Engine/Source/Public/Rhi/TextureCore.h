// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

namespace ERawImageFormat
{

enum Type : uint8
{
    Unspecified,
    BGRA8,
};

int32 GetChannelsPerPixel(const ERawImageFormat::Type InFormat);
int32 GetBytesPerPixel(const ERawImageFormat::Type InFormat);

} /* ~Namespace ERawImageFormat */

} /* ~Namespace Jafg */
