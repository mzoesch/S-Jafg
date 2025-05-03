// Copyright mzoesch. All rights reserved.

#include "MyWorld/Generation/ChunkGenerator.h"
#include "MyWorld/Generation/ChunkGeneratorSubsystem.h"
#include "MyWorld/Chunk/Chunk.h"
#include "System/VoxelSubsystem.h"

void Jafg::ChunkGenerator::ShapeChunk(const LSharedChunkArgs* SharedArgs, const LChunkKey& InKey, voxel_t* InOutChunkData)
{
    checkSlow( SharedArgs )
    checkSlow( InOutChunkData )

    const voxel_t StoneIdx = SharedArgs->VoxelSubsystem->GetVoxelIndex("Stone");

#if PLATFORM_SUPPORTS_SIMD

    f32 NoiseOutput[MwStatics::ChunkSizeCubed];
    SharedArgs->ChunkGeneratorSubsystem->FnGenerator->GenUniformGrid3D(
        NoiseOutput,
        InKey.X * MwStatics::ChunkSize, InKey.Y * MwStatics::ChunkSize, InKey.Z * MwStatics::ChunkSize,
        MwStatics::ChunkSize, MwStatics::ChunkSize, MwStatics::ChunkSize,
        0.01f, 1337
    );

    LChunkKeyDomain Index = INDEX_NONE;
    for (LChunkKeyDomain Z = 0; Z < MwStatics::ChunkSize; ++Z)
    {
        for (LChunkKeyDomain Y = 0; Y < MwStatics::ChunkSize; ++Y)
        {
            for (LChunkKeyDomain X = 0; X < MwStatics::ChunkSize; ++X)
            {
                InOutChunkData[AChunk::GetRawVoxelIndex(X, Y, Z)] = NoiseOutput[++Index]
                    < 0.0f ? ECompileTimeVoxels::Air : StoneIdx;
            }
        }
    }


#else /* PLATFORM_SUPPORTS_SIMD */
    for (LChunkKeyDomainTy X = 0; X < MwStatics::ChunkSize; ++X)
    {
        for (LChunkKeyDomainTy Y = 0; Y < MwStatics::ChunkSize; ++Y)
        {
            for (LChunkKeyDomainTy Z = 0; Z < MwStatics::ChunkSize; ++Z)
            {
                const i32 MapZ = InKey.Z * MwStatics::ChunkSize + Z;
                InOutChunkData[AChunk::GetRawVoxelIndex(X, Y, Z)] = MapZ < 12 ? StoneIdx : ECompileTimeVoxels::Air;
            }
        }
    }
#endif /* !PLATFORM_SUPPORTS_SIMD */

    return;
}

void Jafg::ChunkGenerator::ReplaceSurface(const LSharedChunkArgs* SharedArgs, const LChunkKey& InKey, AChunk* Target, voxel_t* InOutChunkData)
{
    checkSlow( SharedArgs )
    checkSlow( Target )
    checkSlow( InOutChunkData )

    constexpr i32 DirtHeight { 3 };

    const voxel_t GrassIdx = SharedArgs->VoxelSubsystem->GetVoxelIndex("Grass");
    const voxel_t DirtIdx  = SharedArgs->VoxelSubsystem->GetVoxelIndex("Dirt");

    for (LChunkKeyDomain X = 0; X < MwStatics::ChunkSize; ++X)
    {
        for (LChunkKeyDomain Y = 0; Y < MwStatics::ChunkSize; ++Y)
        {
            u8 CurrentDirtDepth = 0;

            for (LChunkKeyDomain Z = MwStatics::ChunkSize - 1 + DirtHeight; Z >= MwStatics::ChunkSize; --Z)
            {
                if (Target->GetRawVoxelDataByNonZeroOrigin(LVoxelKey(X, Y, Z)) == ECompileTimeVoxels::Air)
                {
                    CurrentDirtDepth = 0;
                    continue;
                }

                ++CurrentDirtDepth;

                continue;
            }

            for (LChunkKeyDomain Z = MwStatics::ChunkSize - 1; Z >= 0; --Z)
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
