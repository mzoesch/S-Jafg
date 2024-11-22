// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "MyWorld/CommonTypes.h"

namespace Jafg
{

struct ChunkBoxVertex final
{
    enum TexOffset : uint8
    {
        None  = 0x00 << 0,
        East  = 0x01 << 0,
        South = 0x01 << 1,
        Both  = East | South
    };

    FORCEINLINE ChunkBoxVertex() = default;
    FORCEINLINE ChunkBoxVertex(const uint8 InLocationX, const uint8 InLocationY, const uint8 InLocationZ,
                               const LTextureIndex Idx, const uint32 InDomainWidth,
                               const TexOffset InTexOffset, const uint8 InNormal,
                               const LTextureIndex BlendTexIdx, const uint32 InBlendDomainWidth)
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

    uint8 LocationX = 0;
    uint8 LocationY = 0;
    uint8 LocationZ = 0;

    uint8 TextureGridX = 0;
    uint8 TextureGridY = 0;

    uint8 Normal = 0;

    uint8 BlendTextureGridX = 0;
    uint8 BlendTextureGridY = 0;
};
ENUM_CLASS_FLAGS(ChunkBoxVertex::TexOffset)

} /* ~Namespace Jafg */
