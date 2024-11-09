// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

class AChunk;

class LChunkMesher
{
public:

    LChunkMesher() = delete;
    FORCEINLINE explicit LChunkMesher(AChunk& Owner) : Owner(&Owner) { }
    virtual ~LChunkMesher() = default;

    virtual void GenerateProceduralMesh() = 0;

    FORCEINLINE auto GetOwner() const -> AChunk& { return *this->Owner; }

private:

    AChunk* Owner = nullptr;
};

} /* ~Namespace Jafg */
