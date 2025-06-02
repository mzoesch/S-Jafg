// Copyright mzoesch. All rights reserved.

#include "MyWorld/Generation/ChunkGenerator.h"
#include "MyWorld/Generation/ChunkGeneratorSubsystem.h"
#include "MyWorld/Chunk/Chunk.h"
#include "System/VoxelSubsystem.h"
#include "Stats/Stats.h"

namespace
{

using namespace Jafg;

void ShapeChunk_Default(const LSharedChunkArgs* SharedArgs, const LChunkKey& InKey, voxel_t* InOutChunkData)
{
    STAT_CYCLE_FUNCTION()

    checkSlow( SharedArgs )
    checkSlow( InOutChunkData )

    constexpr i32 MapMaxHeight { 250 };

    const voxel_t StoneIdx = SharedArgs->VoxelSubsystem->GetVoxelIndex("Stone");

    if (InKey.Z < 0)
    {
        std::fill_n(InOutChunkData, MwStatics::VoxelCount - 1, StoneIdx);
        return;
    }

    f32 NoiseOutput[MwStatics::ChunkSizeCubed];
    SharedArgs->ChunkGeneratorSubsystem->GetFastNoiseGenerator()->GenUniformGrid3D(
        NoiseOutput,
        InKey.X * MwStatics::ChunkSize, InKey.Y * MwStatics::ChunkSize, InKey.Z * MwStatics::ChunkSize,
        MwStatics::ChunkSize, MwStatics::ChunkSize, MwStatics::ChunkSize,
        0.01f, 1337
    );

    STAT_QUICK_CYCLE_START("ChunkGenerator::ShapeChunk::GenerateVoxels")
    LChunkKeyDomain Index = INDEX_NONE;
    for (LChunkKeyDomain Z = 0; Z < MwStatics::ChunkSize; ++Z)
    {
        LChunkKeyDomain MapZ = (InKey.Z * MwStatics::ChunkSize) + Z;
        const f32 HeightInPercent = static_cast<f32>(Maths::Clamp(MapZ, 0, MapMaxHeight)) / static_cast<f32>(MapMaxHeight);
        const f32 Density = HeightInPercent * 2.0f - 1.0f;

        for (LChunkKeyDomain Y = 0; Y < MwStatics::ChunkSize; ++Y)
        {
            for (LChunkKeyDomain X = 0; X < MwStatics::ChunkSize; ++X)
            {
                InOutChunkData[AChunk::GetRawVoxelIndex(X, Y, Z)] =
                    NoiseOutput[++Index]
                    > Density
                    ? StoneIdx
                    : ECompileTimeVoxels::Air
                    ;
            }
        }
    }

    return;
}

void ShapeChunk_SuperFlat(const LSharedChunkArgs* SharedArgs, const LChunkKey& InKey, voxel_t* InOutChunkData)
{
    STAT_CYCLE_FUNCTION()

    checkSlow( SharedArgs )
    checkSlow( InOutChunkData )

    constexpr i32 MapHeight { 140 };

    const voxel_t StoneIdx = SharedArgs->VoxelSubsystem->GetVoxelIndex("Stone");

    for (LChunkKeyDomain Z = 0; Z < MwStatics::ChunkSize; ++Z)
    {
        LChunkKeyDomain MapZ = (InKey.Z * MwStatics::ChunkSize) + Z;

        for (LChunkKeyDomain Y = 0; Y < MwStatics::ChunkSize; ++Y)
        {
            for (LChunkKeyDomain X = 0; X < MwStatics::ChunkSize; ++X)
            {
                InOutChunkData[AChunk::GetRawVoxelIndex(X, Y, Z)] = MapZ < MapHeight ? StoneIdx : ECompileTimeVoxels::Air;
            }
        }
    }

    return;
}

} /* ~Namespace <Anonymous> */

void ChunkGenerator::ShapeChunk(const LSharedChunkArgs* SharedArgs, const LChunkKey& InKey, voxel_t* InOutChunkData)
{
    if (SharedArgs->bSuperFlat)
    {
        ShapeChunk_SuperFlat(SharedArgs, InKey, InOutChunkData);
    }
    else
    {
        ShapeChunk_Default(SharedArgs, InKey, InOutChunkData);
    }

    return;
}

void ChunkGenerator::ReplaceSurface(const LSharedChunkArgs* SharedArgs, const LChunkKey& InKey, AChunk* Target, voxel_t* InOutChunkData)
{
    STAT_CYCLE_FUNCTION()

    checkSlow( SharedArgs )
    checkSlow( Target )
    checkSlow( InOutChunkData )

    // constexpr i32 DirtHeight { 3 };
    //
    // const voxel_t GrassIdx = SharedArgs->VoxelSubsystem->GetVoxelIndex("Grass");
    // const voxel_t DirtIdx  = SharedArgs->VoxelSubsystem->GetVoxelIndex("Dirt");
    //
    // for (LChunkKeyDomain X = 0; X < MwStatics::ChunkSize; ++X)
    // {
    //     for (LChunkKeyDomain Y = 0; Y < MwStatics::ChunkSize; ++Y)
    //     {
    //         u8 CurrentDirtDepth = 0;
    //
    //         for (LChunkKeyDomain Z = MwStatics::ChunkSize - 1 + DirtHeight; Z >= MwStatics::ChunkSize; --Z)
    //         {
    //             if (Target->GetRawVoxelDataByNonZeroOrigin(LVoxelKey(X, Y, Z)) == ECompileTimeVoxels::Air)
    //             {
    //                 CurrentDirtDepth = 0;
    //                 continue;
    //             }
    //
    //             ++CurrentDirtDepth;
    //
    //             continue;
    //         }
    //
    //         for (LChunkKeyDomain Z = MwStatics::ChunkSize - 1; Z >= 0; --Z)
    //         {
    //             voxel_t& Voxel = InOutChunkData[AChunk::GetRawVoxelIndex(X, Y, Z)];
    //             if (Voxel == ECompileTimeVoxels::Air)
    //             {
    //                 CurrentDirtDepth = 0;
    //                 continue;
    //             }
    //
    //             const voxel_t VoxelAbove = Target->GetRawVoxelDataByNonZeroOrigin(LVoxelKey(X, Y, Z + 1));
    //             if (VoxelAbove == ECompileTimeVoxels::Air)
    //             {
    //                 Voxel = GrassIdx;
    //                 CurrentDirtDepth = 1;
    //                 continue;
    //             }
    //
    //             if (CurrentDirtDepth > DirtHeight - 1)
    //             {
    //                 continue;
    //             }
    //
    //             Voxel = DirtIdx;
    //             ++CurrentDirtDepth;
    //
    //             continue;
    //         }
    //
    //         continue;
    //     }
    //
    //     continue;
    // }

    return;
}
