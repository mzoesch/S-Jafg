// Copyright mzoesch. All rights reserved.

#include "Lal.afx"
#include "Rhi/TextureCore.h"

i32 Jafg::ERawImageFormat::GetChannelsPerPixel(const ERawImageFormat::Type InFormat)
{
    switch (InFormat)
    {
    case ERawImageFormat::BGRA8: { return 4; }
    case ERawImageFormat::BGR8:  { return 3; }
    default: { checkNoEntry() return 0; }
    }
}

i32 Jafg::ERawImageFormat::GetBytesPerPixel(const ERawImageFormat::Type InFormat)
{
    switch (InFormat)
    {
    case ERawImageFormat::BGRA8: { return 4; }
    case ERawImageFormat::BGR8:  { return 3; }
    default: { checkNoEntry() return 0; }
    }
}

LString Jafg::LexToString(const ERawImageFormat::Type InFormat)
{
    switch (InFormat)
    {
    case ERawImageFormat::Unspecified: { return "Unspecified"; }
    case ERawImageFormat::BGRA8:       { return "BGRA8"; }
    case ERawImageFormat::BGR8:        { return "BGR8"; }
    default: { checkNoEntry() return "Unknown"; }
    }
}
