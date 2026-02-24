// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Rhi/VkAl.h"

namespace Jafg
{

struct LTextureView final
{
    LPath Path;
    LPath Texture;
    vk::Format Format{ vk::Format::eUndefined };
    TOptional<u32> MipLevels;
    TOptional<vk::SampleCountFlagBits> MaxSampleCount;
};

} /* ~Namespace Jafg */
