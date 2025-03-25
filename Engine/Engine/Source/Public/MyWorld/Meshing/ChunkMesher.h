// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Rhi/ChunkBoxVertex.h"

namespace Jafg
{

class AChunk;
class JMaterialSubsystem;
class JVoxelSubsystem;

class LChunkMesher
{
public:

    LChunkMesher() = delete;
    FORCEINLINE explicit LChunkMesher(AChunk& Owner) : Owner(&Owner) { }
    virtual ~LChunkMesher();

            void ClearProceduralMesh();
    virtual void GenerateProceduralMesh(const JVoxelSubsystem* VoxelSubsystem, const JMaterialSubsystem* MaterialSubsystem) = 0;
            void ApplyProceduralMesh();

    void RegenerateProceduralMesh();
    FORCEINLINE void RegenerateProceduralMesh(const JVoxelSubsystem* VoxelSubsystem, const JMaterialSubsystem* MaterialSubsystem)
    {
        this->ClearProceduralMesh();
        this->GenerateProceduralMesh(VoxelSubsystem, MaterialSubsystem);
        this->ApplyProceduralMesh();

        return;
    }

    FORCEINLINE auto GetOwner() const -> AChunk& { return *this->Owner; }
    FORCEINLINE auto GetNumTriangles() const -> i32 { return this->Indices.GetSize(); }
    FORCEINLINE auto GetVertices() const -> const TdhArray<ChunkBoxVertex>& { return this->Vertices; }
    FORCEINLINE auto GetIndices() const -> const TdhArray<u32>& { return this->Indices; }

protected:

    TdhArray<ChunkBoxVertex> Vertices = { };
    TdhArray<u32>         Indices  = { };

private:

    AChunk* Owner = nullptr;
};

} /* ~Namespace Jafg */
