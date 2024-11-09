// Copyright mzoesch. All rights reserved.

#pragma once

#include "ChunkMesher.h"

namespace Jafg
{

class LFastChunkMesher final : public LChunkMesher
{
public:

    LFastChunkMesher() = delete;
    explicit LFastChunkMesher(AChunk& Owner) : LChunkMesher(Owner) { }
    ~LFastChunkMesher() override;

    virtual void ClearProceduralMesh() override;
    virtual void GenerateProceduralMesh() override;
    virtual void ApplyProceduralMesh() override;
};

} /* ~Namespace Jafg */
