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

    virtual void GenerateProceduralMesh(const JVoxelSubsystem* VoxelSubsystem, const JMaterialSubsystem* MaterialSubsystem) override;
};

} /* ~Namespace Jafg */
