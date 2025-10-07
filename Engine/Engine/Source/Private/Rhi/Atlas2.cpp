// Copyright mzoesch. All rights reserved.

#include "Lal.afx"
#include "Rhi/Atlas2.h"
#include "Rhi/RendererInformation.h"

void Jafg::LAtlas2::Make(const TArray<LTexture2>& InTextures, const bool bFreeOld)
{
    if (bFreeOld)
    {
        this->Data.Free();
    }
    checkSlow( this->Data.GetFirstMipMap().GetBulk().IsAllocated() == false )

    if (static_cast<LSize>(InTextures.size()) >= static_cast<LSize>(std::numeric_limits<u8>::max()))
    {
        panic( "Exceed texture count limit. Shaders need updating." )
    }

    const i32 TextureSizeLimit = RendererInformation::GetLimitTextureDimension();

    i32 MaxTextureSize = INDEX_NONE;
    for (const LTexture2& Texture : InTextures)
    {
        MaxTextureSize = Maths::Max(MaxTextureSize, static_cast<i32>(Texture.GetFirstMipMap().GetWidth()));
    }
    if (MaxTextureSize > TextureSizeLimit)
    {
        panicMsgf(
            "Texture size limit exceeded. Texture size limit: {}. Largest texture size: {}.",
            TextureSizeLimit, MaxTextureSize
        )
        return;
    }
    this->TexWidth = MaxTextureSize;

    const i32 AtlasTextureDimensionCount =  static_cast<i32>(Maths::Ceil<float>(Maths::Sqrt(static_cast<float>(InTextures.size()))));
    jassert(AtlasTextureDimensionCount > 0)

    this->Data.CreateEmpty(
        static_cast<u32>(this->TexWidth * AtlasTextureDimensionCount),
        static_cast<u32>(this->TexWidth * AtlasTextureDimensionCount),
        ERawImageFormat::BGRA8
    );

    for (LTextureIndex TextureIndex = 0; TextureIndex < static_cast<LTextureIndex>(InTextures.size()); ++TextureIndex)
    {
        const LTexture2& Texture = InTextures[static_cast<i32>(TextureIndex)];
        const LPoint TexturePoint = this->CalculateSpecificTexturePointOnAtlas(TextureIndex);

        this->Data.CopyTexture(Texture, TexturePoint, true);

        continue;
    }

    return;
}
