// Copyright mzoesch. All rights reserved.

#pragma once

#include "ChunkMesher.h"

namespace Jafg
{

class LNaiveMesher final : public LChunkMesher
{
public:

    LNaiveMesher() = delete;
    explicit LNaiveMesher(AChunk& Owner) : LChunkMesher(Owner) { }

    virtual void GenerateProceduralMesh(const JVoxelSubsystem* VoxelSubsystem, const JMaterialSubsystem* MaterialSubsystem) override;
};

} /* ~Namespace Jafg */
