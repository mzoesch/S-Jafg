// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "JustTemp.h"

namespace Jafg
{

class AChunk;

class LChunkMesher
{
public:

    LChunkMesher() = delete;
    FORCEINLINE explicit LChunkMesher(AChunk& Owner) : Owner(&Owner) { }
    virtual ~LChunkMesher() = default;

    virtual void ClearProceduralMesh()    = 0;
    virtual void GenerateProceduralMesh() = 0;
    virtual void ApplyProceduralMesh()    = 0;

    FORCEINLINE void RegenerateProceduralMesh()
    {
        this->ClearProceduralMesh();
        this->GenerateProceduralMesh();
        this->ApplyProceduralMesh();

        return;
    }

    FORCEINLINE auto GetOwner() const -> AChunk& { return *this->Owner; }
    FORCEINLINE auto GetNumTriangles() const -> int32 { return this->Indices.GetSize(); }
    FORCEINLINE auto GetVertices() const -> const TdhArray<Vertex>& { return this->Vertices; }
    FORCEINLINE auto GetIndices() const -> const TdhArray<uint32>& { return this->Indices; }

protected:

    TdhArray<Vertex> Vertices = {};
    TdhArray<uint32> Indices = {};

private:

    AChunk* Owner = nullptr;
};

} /* ~Namespace Jafg */
