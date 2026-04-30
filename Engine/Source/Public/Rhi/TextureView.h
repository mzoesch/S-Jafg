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
    std::optional<u32> MipLevels;
    std::optional<vk::SampleCountFlagBits> MaxSampleCount;
};

} /* ~Namespace Jafg */
