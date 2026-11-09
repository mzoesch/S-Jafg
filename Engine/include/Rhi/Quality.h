// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

//#
//# Dictates the texture quality.
//#
//# Texture quality usually do not directly affect tps, but they dramatically affect memory usage
//# and may bring latency issues and high host consumption by increased workload for texture
//# unwrapping, generation, and streaming.
//#
enum struct ETextureQualityBits: u8
{
    //# Usually less than 512 sized textures.
    Potato      = 0x0 << 0,
    //# Usually less than 1k sized textures.
    Toaster     = 0x1 << 1,
    //# Usually 1k sized textures.
    Mobile      = 0x1 << 2,
    //# Usually 2k sized textures.
    Standard    = 0x1 << 3,
    //# Usually up to 4k sized textures.
    Ultra       = 0x1 << 4,
    //# Usually up to 8k sized textures.
    Cinematic   = 0x1 << 5,
};
SERDE_ENUM(ETextureQualityBits, Potato, Toaster, Mobile, Standard, Ultra, Cinematic)
SERDE_ENUM_STRUCT_FLAGS(ETextureQualityBits, ETextureQualityFlags)

//#
//# Which lod to use for a texture. By default, the highest lod is used.
//# If a lod does not exist, it is clamped to the lowest available.
//#
DECLARE_TRIVIAL_SERDE_ENUM(ETextureLodBits, u8,
    //# The highest lod and, therefore, the original. This is equal to #Auto.
    e00,
    e01,
    e02,
    e03,
    e04,
    e05,
    e06,
    e07,
    e08,
    e09,
    e10,
    e11,
    e12,
    e13,
    e14,
    //# The lowest lod.
    e15,
    //# Automatically determine the lod. This is the default.
    Auto
    )

} /* ~Namespace Jafg */

JAFG_PREF_OF(Jafg::ETextureQualityBits)
JAFG_PREF_OF(Jafg::ETextureLodBits)
