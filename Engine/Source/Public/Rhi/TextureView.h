// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace Jafg
{

struct LTextureView final
{
    LPath Path;
    LString Name;
    LPath Texture;
    vk::Format Format{ vk::Format::eUndefined };
    TOptional<u32> MipLevels;
    TOptional<vk::SampleCountFlagBits> MaxSampleCount;
};

} /* ~Namespace Jafg */
