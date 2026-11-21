// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace Jafg
{

struct LTextureView final
{
    //# Absolute path to the view file.
    LPath Path;
    //# The name of the asset which may be overloaded by user preferences. Relative to its namespace.
    LString Identifier;
    //# Absolute path to the texture file.
    LPath Texture;
    vk::Format Format{ vk::Format::eUndefined };
    std::optional<u32> MipLevels;
    std::optional<vk::SampleCountFlagBits> MaxSampleCount;
};

} /* ~Namespace Jafg */
