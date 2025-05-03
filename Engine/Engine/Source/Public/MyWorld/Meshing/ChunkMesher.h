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
    FORCEINLINE void RegenerateProceduralMesh(const JVoxelSubsystem* VoxelSubsystem, const JMaterialSubsystem* MaterialSubsystem);

    FORCEINLINE auto GetOwner() const -> AChunk& { return *this->Owner; }
    FORCEINLINE auto GetNumTriangles() const -> i32 { return this->Indices.GetSize(); }
    FORCEINLINE auto GetVertices() const -> const TArray<ChunkBoxVertex>& { return this->Vertices; }
    FORCEINLINE auto GetIndices() const -> const TArray<u32>& { return this->Indices; }

protected:

    TArray<ChunkBoxVertex> Vertices;
    TArray<u32>            Indices;

private:

    AChunk* Owner = nullptr;
};

FORCEINLINE void LChunkMesher::RegenerateProceduralMesh(const JVoxelSubsystem* VoxelSubsystem, const JMaterialSubsystem* MaterialSubsystem)
{
    this->ClearProceduralMesh();
    this->GenerateProceduralMesh(VoxelSubsystem, MaterialSubsystem);
    this->ApplyProceduralMesh();

    return;
}

} /* ~Namespace Jafg */
