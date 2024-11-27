// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Myworld/Generation/ChunkGenerator.h"
#include "ChunkGeneratorSubsystem.h"
#include "FastNoise/FastNoise.h"
#include "MyWorld/Chunk/Chunk.h"
#include "System/VoxelSubsystem.h"

void Jafg::ChunkGenerator::ShapeChunk(const LSharedChunkArgs* SharedArgs, const LChunkKey& InKey, voxel_t*& InOutChunkData)
{
    checkSlow( SharedArgs )
    checkSlow( InOutChunkData )

    const voxel_t StoneIdx = SharedArgs->VoxelSubsystem->GetVoxelIndex("Stone");

    float NoiseOutput[MwStatics::ChunkSizeSquared];
    SharedArgs->ChunkGeneratorSubsystem->FnGenerator->GenUniformGrid2D(
        NoiseOutput,
        InKey.Y * MwStatics::ChunkSize, InKey.X * MwStatics::ChunkSize,
        MwStatics::ChunkSize, MwStatics::ChunkSize,
        0.02f, 1337
    );

    LChunkKeyDomainTy Index = INDEX_NONE;
    for (LChunkKeyDomainTy X = 0; X < MwStatics::ChunkSize; ++X)
    {
        for (LChunkKeyDomainTy Y = 0; Y < MwStatics::ChunkSize; ++Y)
        {
            const int32 Height = static_cast<int32>(Maths::Floor(NoiseOutput[++Index] * 32.0f) + 32.0f);
            for (LChunkKeyDomainTy Z = 0; Z < MwStatics::ChunkSize; ++Z)
            {
                const int32 MapZ = InKey.Z * MwStatics::ChunkSize + Z;
                InOutChunkData[AChunk::GetRawVoxelIndex(X, Y, Z)] = MapZ < Height ? StoneIdx : ECompileTimeVoxels::Air;
            }
        }
    }

    return;
}

void Jafg::ChunkGenerator::ReplaceSurface(const LSharedChunkArgs* SharedArgs, const LChunkKey& InKey, AChunk* Target, voxel_t*& InOutChunkData)
{
    checkSlow( SharedArgs )
    checkSlow( Target )
    checkSlow( InOutChunkData )

    constexpr int32 DirtHeight { 3 };

    const voxel_t GrassIdx = SharedArgs->VoxelSubsystem->GetVoxelIndex("Grass");
    const voxel_t DirtIdx  = SharedArgs->VoxelSubsystem->GetVoxelIndex("Dirt");

    for (LChunkKeyDomainTy X = 0; X < MwStatics::ChunkSize; ++X)
    {
        for (LChunkKeyDomainTy Y = 0; Y < MwStatics::ChunkSize; ++Y)
        {
            uint8 CurrentDirtDepth = 0;

            for (LChunkKeyDomainTy Z = MwStatics::ChunkSize - 1 + DirtHeight; Z >= MwStatics::ChunkSize; --Z)
            {
                if (Target->GetRawVoxelDataByNonZeroOrigin(LVoxelKey(X, Y, Z)) == ECompileTimeVoxels::Air)
                {
                    CurrentDirtDepth = 0;
                    continue;
                }

                ++CurrentDirtDepth;

                continue;
            }

            for (LChunkKeyDomainTy Z = MwStatics::ChunkSize - 1; Z >= 0; --Z)
            {
                voxel_t& Voxel = InOutChunkData[AChunk::GetRawVoxelIndex(X, Y, Z)];
                if (Voxel == ECompileTimeVoxels::Air)
                {
                    CurrentDirtDepth = 0;
                    continue;
                }

                const voxel_t VoxelAbove = Target->GetRawVoxelDataByNonZeroOrigin(LVoxelKey(X, Y, Z + 1));
                if (VoxelAbove == ECompileTimeVoxels::Air)
                {
                    Voxel = GrassIdx;
                    CurrentDirtDepth = 1;
                    continue;
                }

                if (CurrentDirtDepth > DirtHeight - 1)
                {
                    continue;
                }

                Voxel = DirtIdx;
                ++CurrentDirtDepth;

                continue;
            }

            continue;
        }

        continue;
    }

    return;
}
