// Copyright mzoesch. All rights reserved.

#pragma once

#include "MyWorld/CommonTypes.h"

namespace Jafg
{

struct ChunkBoxVertex final
{
    enum TexOffset : u8
    {
        None  = 0x00 << 0,
        East  = 0x01 << 0,
        South = 0x01 << 1,
        Both  = East | South
    };

    FORCEINLINE ChunkBoxVertex() = default;
    FORCEINLINE ChunkBoxVertex(const u8 InLocationX, const u8 InLocationY, const u8 InLocationZ,
                               const LTextureIndex Idx, const u32 InDomainWidth,
                               const TexOffset InTexOffset, const u8 InNormal,
                               const LTextureIndex BlendTexIdx, const u32 InBlendDomainWidth)
            : LocationX(InLocationX), LocationY(InLocationY), LocationZ(InLocationZ), Normal(InNormal)
    {
        this->TextureGridX = (InTexOffset & TexOffset::East ) > 0;
        this->TextureGridY = (InTexOffset & TexOffset::South) > 0;
        this->TextureGridX += (Idx % InDomainWidth);
        this->TextureGridY += (Idx / InDomainWidth);

        this->BlendTextureGridX = (InTexOffset & TexOffset::East ) > 0;
        this->BlendTextureGridY = (InTexOffset & TexOffset::South) > 0;
        this->BlendTextureGridX += (BlendTexIdx % InBlendDomainWidth);
        this->BlendTextureGridY += (BlendTexIdx / InBlendDomainWidth);

        return;
    }

    u8 LocationX = 0;
    u8 LocationY = 0;
    u8 LocationZ = 0;

    u8 TextureGridX = 0;
    u8 TextureGridY = 0;

    u8 Normal = 0;

    u8 BlendTextureGridX = 0;
    u8 BlendTextureGridY = 0;
};
ENUM_CLASS_FLAGS(ChunkBoxVertex::TexOffset)

} /* ~Namespace Jafg */
