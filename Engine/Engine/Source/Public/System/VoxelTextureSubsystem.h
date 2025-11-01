// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/EngineSubsystem.h"
#include "MyWorld/CommonTypes.h"
#include "VoxelTextureSubsystem.generated.h"

namespace Jafg
{

struct LDiskVoxelTexture;
struct LDiskBlendTexture;

//#
//# A texture that is present in the voxel texture directory.
//#
//# It must fulfill the following requirements:
//#   - The texture must be a PNG file.
//#   - The texture must be located in the voxel texture directory of their owning project.
//#   - The dimensions must be a number that is a power of two. Note that one single texture that is greater than
//#     all other textures will cause an upscaling of all lower resolution textures.
//#   - The first runes until the first occurrence of a #TexSectionDividerChar must exactly match the voxel name. This
//#     is case-sensitive.
//#   - Optionally add a normal vector with the following format:
//#     <VoxelName><TexSectionDividerChar><NormalVector> (See CommonTypes.h for the normal vector format). This texture
//#     will now only be used for the face with a normal that includes the described normal vector. You may provide a
//#     default texture for all other faces that are not covered by the normal textures by just commiting the normal
//#     vector in one of the texture names for the voxel.
//#   - Optionally add a blending texture with the following format:
//#     <VoxelName><TexSectionDividerChar><Blend> (See CommonTypes.h for the blend format). This texture will be blended
//#     with the other texture.
//#
struct LDiskVoxelTexture final
{
    inline static char TexSectionDividerChar = '_';

    LString Name;

    auto GetVoxelName() const -> LString;
    auto GetNormalLookUpBasedOfFileName() const -> ENormalLookup::Type;
    auto GetBlendLookUpBasedOfFileName(const TArray<LDiskBlendTexture>& InCurrentUsedBlends) const -> LTextureIndex;
};

struct LDiskBlendTexture final
{
    LString Name;
};

//#
//# Texture subsystem that manages the loading, caching and freeing of all textures that are present.
//#
DECLARE_JAFG_CLASS()
class JVoxelTextureSubsystem final : public JEngineSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JVoxelTextureSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;

public:

    //#
    //# @return All unique texture names that where found in all texture directories based on user preferences.
    //#
    TArray<LDiskVoxelTexture> FindMeaningFullVoxelTextureNames() const;

    //#
    //# @return All unique blend texture names that where found in all texture directories based on user preferences.
    //#
    TArray<LDiskBlendTexture> FindMeaningBlendTextureNames() const;
};

} /* ~Namespace Jafg */
