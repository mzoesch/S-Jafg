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
    virtual ~LChunkMesher() = default;

            void ClearProceduralMesh();
    virtual void GenerateProceduralMesh(const JVoxelSubsystem* VoxelSubsystem, const JMaterialSubsystem* MaterialSubsystem) = 0;
            void ApplyProceduralMesh();

    FORCEINLINE void RegenerateProceduralMesh(const JVoxelSubsystem* VoxelSubsystem, const JMaterialSubsystem* MaterialSubsystem);

    FORCEINLINE AChunk& GetOwner() const { return *this->Owner; }
    FORCEINLINE i32 GetNumTriangles() const { return this->NumTriangles; }
    FORCEINLINE const TArray<ChunkBoxVertex>& GetVertices() const { return this->Vertices; }
    FORCEINLINE const TArray<u32>& GetIndices() const { return this->Indices; }

    FORCEINLINE std::mutex& GetMutex() { return this->Mutex; }

protected:

    i32                    NumTriangles { 0 };
    TArray<ChunkBoxVertex> Vertices;
    TArray<u32>            Indices;
    std::mutex             Mutex;

private:

    AChunk* Owner { nullptr };
};

FORCEINLINE void LChunkMesher::RegenerateProceduralMesh(const JVoxelSubsystem* VoxelSubsystem, const JMaterialSubsystem* MaterialSubsystem)
{
    if (this->Mutex.try_lock())
    {
        this->ClearProceduralMesh();
        this->GenerateProceduralMesh(VoxelSubsystem, MaterialSubsystem);
        this->ApplyProceduralMesh();

        this->Mutex.unlock();
    }

    return;
}

} /* ~Namespace Jafg */
