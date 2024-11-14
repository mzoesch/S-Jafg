// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Rhi/TextureCore.h"

int32 Jafg::ERawImageFormat::GetChannelsPerPixel(const ERawImageFormat::Type InFormat)
{
    switch (InFormat)
    {
    case ERawImageFormat::BGRA8: { return 4; }
    default: { checkNoEntry() return 0; }
    }
}

int32 Jafg::ERawImageFormat::GetBytesPerPixel(const ERawImageFormat::Type InFormat)
{
    switch (InFormat)
    {
    case ERawImageFormat::BGRA8: { return 4; }
    default: { checkNoEntry() return 0; }
    }
}
