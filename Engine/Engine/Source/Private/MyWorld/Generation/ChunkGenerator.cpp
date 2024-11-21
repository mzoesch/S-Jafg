// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Myworld/Generation/ChunkGenerator.h"
#include "ChunkGeneratorSubsystem.h"
#include "FastNoise/FastNoise.h"
#include "MyWorld/Chunk/Chunk.h"
#include "System/VoxelSubsystem.h"

void Jafg::ChunkGenerator::ShapeChunk(const LSharedChunkArgs* SharedArgs, const LChunkKey& InKey, voxel_t*& InOutChunkData)
{
    SCOPED_TIME_TAKEN_MEASURER_MsgF(LogChunkGeneration, Trace, "{}", InKey.ToString())

    const voxel_t StoneIdx = SharedArgs->VoxelSubsystem->GetVoxelIndex("Stone");

    float NoiseOutput[MwStatics::ChunkSizeSquared];

    // SharedArgs->ChunkGeneratorSubsystem->FnGenerator->GenUniformGrid3D(
    //     NoiseOutput,
    //     InKey.X * MwStatics::ChunkSize, InKey.Y * MwStatics::ChunkSize, InKey.Z * MwStatics::ChunkSize,
    //     MwStatics::ChunkSize, MwStatics::ChunkSize, MwStatics::ChunkSize,
    //     0.004f, 1337
    // );

    SharedArgs->ChunkGeneratorSubsystem->FnGenerator->GenUniformGrid2D(
        NoiseOutput,
        InKey.Y * MwStatics::ChunkSize, InKey.X * MwStatics::ChunkSize,
        MwStatics::ChunkSize, MwStatics::ChunkSize,
        0.004f, 1337
    );

    LChunkKeyDomainTy Index = INDEX_NONE;
    for (LChunkKeyDomainTy X = 0; X < MwStatics::ChunkSize; X++)
    {
        for (LChunkKeyDomainTy Y = 0; Y < MwStatics::ChunkSize; Y++)
        {
            const int32 Height = Maths::Floor(NoiseOutput[++Index] * 32) + 32;
            for (LChunkKeyDomainTy Z = 0; Z < MwStatics::ChunkSize; Z++)
            {
                const int32 MapZ = InKey.Z * MwStatics::ChunkSize + Z;
                InOutChunkData[AChunk::GetRawVoxelIndex(X, Y, Z)] = MapZ < Height ? StoneIdx : ECompileTimeVoxels::Air;
            }
        }
    }

    // LChunkKeyDomainTy Index = INDEX_NONE;
    // for (LChunkKeyDomainTy Z = 0; Z < MwStatics::ChunkSize; Z++)
    // {
    //     for (LChunkKeyDomainTy Y = 0; Y < MwStatics::ChunkSize; Y++)
    //     {
    //         for (LChunkKeyDomainTy X = 0; X < MwStatics::ChunkSize; X++)
    //         {
    //             InOutChunkData[AChunk::GetRawVoxelIndex(X, Y, Z)] = NoiseOutput[++Index] > 0.0f
    //                 ? StoneIdx : ECompileTimeVoxels::Air;
    //         }
    //     }
    // }

    return;
}
