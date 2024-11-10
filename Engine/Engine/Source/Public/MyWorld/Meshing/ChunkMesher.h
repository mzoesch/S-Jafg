// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "RhiFramework/ChunkBoxVertex.h"

namespace Jafg
{

class AChunk;

class LChunkMesher
{
public:

    LChunkMesher() = delete;
    FORCEINLINE explicit LChunkMesher(AChunk& Owner) : Owner(&Owner) { }
    virtual ~LChunkMesher();

            void ClearProceduralMesh();
    virtual void GenerateProceduralMesh() = 0;
            void ApplyProceduralMesh() const;

    FORCEINLINE void RegenerateProceduralMesh()
    {
        this->ClearProceduralMesh();
        this->GenerateProceduralMesh();
        this->ApplyProceduralMesh();

        return;
    }

    FORCEINLINE auto GetOwner() const -> AChunk& { return *this->Owner; }
    FORCEINLINE auto GetNumTriangles() const -> int32 { return this->Indices.GetSize(); }
    FORCEINLINE auto GetVertices() const -> const TdhArray<ChunkBoxVertex>& { return this->Vertices; }
    FORCEINLINE auto GetIndices() const -> const TdhArray<uint32>& { return this->Indices; }

protected:

    TdhArray<ChunkBoxVertex> Vertices = { };
    TdhArray<uint32>         Indices  = { };

private:

    AChunk* Owner = nullptr;
};

} /* ~Namespace Jafg */
