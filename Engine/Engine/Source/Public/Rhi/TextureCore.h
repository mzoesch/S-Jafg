// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

namespace ERawImageFormat
{

enum Type : u8
{
    Unspecified,
    BGRA8,
    BGR8,
};

ENGINE_API i32 GetChannelsPerPixel(const ERawImageFormat::Type InFormat);
ENGINE_API i32 GetBytesPerPixel(const ERawImageFormat::Type InFormat);

} /* ~Namespace ERawImageFormat */

ENGINE_API LString LexToString(const ERawImageFormat::Type InFormat);

} /* ~Namespace Jafg */
