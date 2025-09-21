// Copyright mzoesch. All rights reserved.

#include "Lal.afx"
#include "MyWorld/MyWorldStatics.h"
#include "MyWorld/Meshing/NaiveMesher.h"
#include "MyWorld/CommonTypes.h"
#include "MyWorld/Chunk/Chunk.h"
#include "System/MaterialSubsystem.h"
#include "System/VoxelSubsystem.h"
#include "Stats/Stats.h"

void Jafg::LNaiveMesher::GenerateProceduralMesh(const JVoxelSubsystem* VoxelSubsystem, const JMaterialSubsystem* MaterialSubsystem)
{
    STAT_CYCLE_FUNCTION()

    u32 CurrentVertex = 0;
    for (LVoxelKeyDomainTy X = 0; X < MwStatics::ChunkSize; ++X)
    {
        for (LVoxelKeyDomainTy Y = 0; Y < MwStatics::ChunkSize; ++Y)
        {
            for (LVoxelKeyDomainTy Z = 0; Z < MwStatics::ChunkSize; ++Z)
            {
                const LVoxelKey VoxelKey     = { X, Y, Z };
                voxel_t   CurrentVoxel = this->GetOwner().GetRawVoxelData(VoxelKey);

                if (CurrentVoxel == ECompileTimeVoxels::Null || CurrentVoxel == ECompileTimeVoxels::Air)
                {
                    continue;
                }

                if (CurrentVoxel > 10)
                {
                    LOG_ERROR(LogTemporal, "Greater then 10;")
                    CurrentVoxel = this->GetOwner().GetRawVoxelData(VoxelKey);
                }

                const LVoxelMask& Mask = VoxelSubsystem->GetVoxelMask(CurrentVoxel);

                const voxel_t NorthVoxel = this->GetOwner().GetRawVoxelDataByNonZeroOrigin(VoxelKey.GetNorthKey(), ECompileTimeVoxels::Null);
                if (NorthVoxel == ECompileTimeVoxels::Air)
                {
                    LTextureIndex Idx = Mask.FindTextureIndex(ENormalLookup::North);
                    LTextureIndex BlendIdx = Mask.FindBlendTextureIndex(ENormalLookup::North);

                    Vertices.Emplace(X + 1, Y + 1, Z + 1, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::None, ENormalShadingMap::Forward, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 1, Y + 0, Z + 1, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::East, ENormalShadingMap::Forward, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 1, Y + 0, Z + 0, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::Both, ENormalShadingMap::Forward, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 1, Y + 1, Z + 0, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::South, ENormalShadingMap::Forward, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());

                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 1);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 3);
                    CurrentVertex += 4;
                }

                const voxel_t SouthVoxel = this->GetOwner().GetRawVoxelDataByNonZeroOrigin(VoxelKey.GetSouthKey(), ECompileTimeVoxels::Null);
                if (SouthVoxel == ECompileTimeVoxels::Air)
                {
                    LTextureIndex Idx = Mask.FindTextureIndex(ENormalLookup::South);
                    LTextureIndex BlendIdx = Mask.FindBlendTextureIndex(ENormalLookup::South);

                    Vertices.Emplace(X + 0, Y + 0, Z + 1, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::None, ENormalShadingMap::Backward, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 0, Y + 1, Z + 1, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::East, ENormalShadingMap::Backward, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 0, Y + 1, Z + 0, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::Both, ENormalShadingMap::Backward, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 0, Y + 0, Z + 0, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::South, ENormalShadingMap::Backward, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());

                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 1);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 3);
                    CurrentVertex += 4;
                }

                const voxel_t WestVoxel = this->GetOwner().GetRawVoxelDataByNonZeroOrigin(VoxelKey.GetWestKey(), ECompileTimeVoxels::Null);
                if (WestVoxel == ECompileTimeVoxels::Air)
                {
                    LTextureIndex Idx = Mask.FindTextureIndex(ENormalLookup::West);
                    LTextureIndex BlendIdx = Mask.FindBlendTextureIndex(ENormalLookup::West);

                    Vertices.Emplace(X + 1, Y + 0, Z + 1, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::None, ENormalShadingMap::Left, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 0, Y + 0, Z + 1, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::East, ENormalShadingMap::Left, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 0, Y + 0, Z + 0, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::Both, ENormalShadingMap::Left, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 1, Y + 0, Z + 0, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::South, ENormalShadingMap::Left, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());

                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 1);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 3);
                    CurrentVertex += 4;
                }

                const voxel_t EastVoxel = this->GetOwner().GetRawVoxelDataByNonZeroOrigin(VoxelKey.GetEastKey(), ECompileTimeVoxels::Null);
                if (EastVoxel == ECompileTimeVoxels::Air)
                {
                    LTextureIndex Idx = Mask.FindTextureIndex(ENormalLookup::East);
                    LTextureIndex BlendIdx = Mask.FindBlendTextureIndex(ENormalLookup::East);

                    Vertices.Emplace(X + 0, Y + 1, Z + 1, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::None, ENormalShadingMap::Right, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 1, Y + 1, Z + 1, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::East, ENormalShadingMap::Right, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 1, Y + 1, Z + 0, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::Both, ENormalShadingMap::Right, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 0, Y + 1, Z + 0, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::South, ENormalShadingMap::Right, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());

                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 1);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 3);
                    CurrentVertex += 4;
                }

                const voxel_t UpVoxel = this->GetOwner().GetRawVoxelDataByNonZeroOrigin(VoxelKey.GetUpKey(), ECompileTimeVoxels::Null);
                if (UpVoxel == ECompileTimeVoxels::Air)
                {
                    LTextureIndex Idx = Mask.FindTextureIndex(ENormalLookup::Up);
                    LTextureIndex BlendIdx = Mask.FindBlendTextureIndex(ENormalLookup::Up);

                    Vertices.Emplace(X + 1, Y + 0, Z + 1, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::None, ENormalShadingMap::Up, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 1, Y + 1, Z + 1, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::East, ENormalShadingMap::Up, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 0, Y + 1, Z + 1, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::Both, ENormalShadingMap::Up, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 0, Y + 0, Z + 1, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::South, ENormalShadingMap::Up, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());

                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 1);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 3);
                    CurrentVertex += 4;
                }

                const voxel_t DownVoxel = this->GetOwner().GetRawVoxelDataByNonZeroOrigin(VoxelKey.GetDownKey(), ECompileTimeVoxels::Null);
                if (DownVoxel == ECompileTimeVoxels::Air)
                {
                    LTextureIndex Idx = Mask.FindTextureIndex(ENormalLookup::Down);
                    LTextureIndex BlendIdx = Mask.FindBlendTextureIndex(ENormalLookup::Down);

                    Vertices.Emplace(X + 1, Y + 1, Z + 0, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::None, ENormalShadingMap::Down, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 1, Y + 0, Z + 0, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::East, ENormalShadingMap::Down, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 0, Y + 0, Z + 0, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::Both, ENormalShadingMap::Down, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());
                    Vertices.Emplace(X + 0, Y + 1, Z + 0, Idx, MaterialSubsystem->GetBlendOpaqueDomainWidth(),
                        ChunkBoxVertex::TexOffset::South, ENormalShadingMap::Down, BlendIdx, MaterialSubsystem->GetBlendersDomainWidth());

                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 1);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 3);
                    CurrentVertex += 4;
                }

                continue;
            }

            continue;
        }

        continue;
    }

    return;
}
