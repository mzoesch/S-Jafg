// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Myworld/Generation/ChunkGenerator.h"
#include "ChunkGeneratorSubsystem.h"
#include "FastNoise/FastNoise.h"
#include "MyWorld/Chunk/Chunk.h"
#include "System/VoxelSubsystem.h"

void Jafg::ChunkGenerator::ShapeChunk(const LSharedChunkArgs* SharedArgs, const LChunkKey& InKey, voxel_t*& InOutChunkData)
{
    const voxel_t StoneIdx = SharedArgs->VoxelSubsystem->GetVoxelIndex("Stone");

    float* NoiseOutput = SharedArgs->ChunkGeneratorSubsystem->UsableContainer;
    SharedArgs->ChunkGeneratorSubsystem->FnGenerator->GenUniformGrid3D(
        NoiseOutput,
        InKey.X * MwStatics::ChunkSize, InKey.Y * MwStatics::ChunkSize, InKey.Z * MwStatics::ChunkSize,
        MwStatics::ChunkSize, MwStatics::ChunkSize, MwStatics::ChunkSize,
        0.004f, 1337
    );

    LChunkKeyDomainTy Index = INDEX_NONE;
    for (LChunkKeyDomainTy Z = 0; Z < MwStatics::ChunkSize; Z++)
    {
        for (LChunkKeyDomainTy Y = 0; Y < MwStatics::ChunkSize; Y++)
        {
            for (LChunkKeyDomainTy X = 0; X < MwStatics::ChunkSize; X++)
            {
                InOutChunkData[AChunk::GetRawVoxelIndex(X, Y, Z)] = NoiseOutput[++Index] > 0.0f
                    ? StoneIdx : ECompileTimeVoxels::Air;
            }
        }
    }

    return;
}
