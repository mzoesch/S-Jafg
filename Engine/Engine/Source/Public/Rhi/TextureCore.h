// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

namespace ERawImageFormat
{

enum Type : u8
{
    Unspecified,
    BGRA8,
};

ENGINE_API i32 GetChannelsPerPixel(const ERawImageFormat::Type InFormat);
ENGINE_API i32 GetBytesPerPixel(const ERawImageFormat::Type InFormat);

} /* ~Namespace ERawImageFormat */

} /* ~Namespace Jafg */
