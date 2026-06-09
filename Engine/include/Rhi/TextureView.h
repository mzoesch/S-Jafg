// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"

namespace Jafg
{

struct LTextureView final
{
    //# Path to the asset relative to the engine root dir.
    LPath Path;
    //# The name of the asset which may be overloaded by user preferences.
    LString Name;
    //# The path to the texture file relative to the engine root dir.
    LPath Texture;
    vk::Format Format{ vk::Format::eUndefined };
    std::optional<u32> MipLevels;
    std::optional<vk::SampleCountFlagBits> MaxSampleCount;
};

} /* ~Namespace Jafg */
