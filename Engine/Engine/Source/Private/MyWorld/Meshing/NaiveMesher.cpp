// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "MyWorld/MyWorldStatics.h"
#include "MyWorld/Meshing/NaiveMesher.h"
#include "MyWorld/Block.h"
#include "MyWorld/Blocks.h"
#include "MyWorld/CommonTypes.h"
#include "MyWorld/Chunk/Chunk.h"

void Jafg::LNaiveMesher::GenerateProceduralMesh()
{
    uint32 CurrentVertex = 0;
    for (LVoxelKeyDomainTy X = 0; X < MwStatics::ChunkSize; ++X)
    {
        for (LVoxelKeyDomainTy Y = 0; Y < MwStatics::ChunkSize; ++Y)
        {
            for (LVoxelKeyDomainTy Z = 0; Z < MwStatics::ChunkSize; ++Z)
            {
                const LVoxelKey VoxelKey     = { X, Y, Z };
                const voxel_t   CurrentVoxel = this->GetOwner().GetRawVoxelData(VoxelKey);

                if (CurrentVoxel == ECompileTimeVoxels::Null || CurrentVoxel == ECompileTimeVoxels::Air)
                {
                    continue;
                }

                const Block* Block = &Blocks::blocks[CurrentVoxel];

                const voxel_t NorthVoxel = this->GetOwner().GetRawVoxelDataByNonZeroOrigin(VoxelKey.GetNorthKey(), ECompileTimeVoxels::Air);
                if (NorthVoxel == ECompileTimeVoxels::Air)
                {
                    Vertices.Emplace(X + 1, Y + 1, Z + 1, Block->sideMinX, Block->sideMaxY);
                    Vertices.Emplace(X + 1, Y + 0, Z + 1, Block->sideMaxX, Block->sideMaxY);
                    Vertices.Emplace(X + 1, Y + 0, Z + 0, Block->sideMaxX, Block->sideMinY);
                    Vertices.Emplace(X + 1, Y + 1, Z + 0, Block->sideMinX, Block->sideMinY);

                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 1);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 3);
                    CurrentVertex += 4;
                }

                const voxel_t SouthVoxel = this->GetOwner().GetRawVoxelDataByNonZeroOrigin(VoxelKey.GetSouthKey(), ECompileTimeVoxels::Air);
                if (SouthVoxel == ECompileTimeVoxels::Air)
                {
                    Vertices.Emplace(X + 0, Y + 0, Z + 1, Block->sideMinX, Block->sideMaxY);
                    Vertices.Emplace(X + 0, Y + 1, Z + 1, Block->sideMaxX, Block->sideMaxY);
                    Vertices.Emplace(X + 0, Y + 1, Z + 0, Block->sideMaxX, Block->sideMinY);
                    Vertices.Emplace(X + 0, Y + 0, Z + 0, Block->sideMinX, Block->sideMinY);

                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 1);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 3);
                    CurrentVertex += 4;
                }

                const voxel_t WestVoxel = this->GetOwner().GetRawVoxelDataByNonZeroOrigin(VoxelKey.GetWestKey(), ECompileTimeVoxels::Air);
                if (WestVoxel == ECompileTimeVoxels::Air)
                {
                    Vertices.Emplace(X + 1, Y + 0, Z + 1, Block->sideMaxX, Block->sideMaxY);
                    Vertices.Emplace(X + 0, Y + 0, Z + 1, Block->sideMinX, Block->sideMaxY);
                    Vertices.Emplace(X + 0, Y + 0, Z + 0, Block->sideMinX, Block->sideMinY);
                    Vertices.Emplace(X + 1, Y + 0, Z + 0, Block->sideMaxX, Block->sideMinY);

                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 1);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 3);
                    CurrentVertex += 4;
                }

                const voxel_t EastVoxel = this->GetOwner().GetRawVoxelDataByNonZeroOrigin(VoxelKey.GetEastKey(), ECompileTimeVoxels::Air);
                if (EastVoxel == ECompileTimeVoxels::Air)
                {
                    Vertices.Emplace(X + 0, Y + 1, Z + 1, Block->sideMinX, Block->sideMaxY);
                    Vertices.Emplace(X + 1, Y + 1, Z + 1, Block->sideMaxX, Block->sideMaxY);
                    Vertices.Emplace(X + 1, Y + 1, Z + 0, Block->sideMaxX, Block->sideMinY);
                    Vertices.Emplace(X + 0, Y + 1, Z + 0, Block->sideMinX, Block->sideMinY);

                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 1);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 3);
                    CurrentVertex += 4;
                }

                const voxel_t UpVoxel = this->GetOwner().GetRawVoxelDataByNonZeroOrigin(VoxelKey.GetUpKey(), ECompileTimeVoxels::Air);
                if (UpVoxel == ECompileTimeVoxels::Air)
                {
                    Vertices.Emplace(X + 1, Y + 0, Z + 1, Block->topMinX, Block->topMaxY);
                    Vertices.Emplace(X + 1, Y + 1, Z + 1, Block->topMaxX, Block->topMaxY);
                    Vertices.Emplace(X + 0, Y + 1, Z + 1, Block->topMaxX, Block->topMinY);
                    Vertices.Emplace(X + 0, Y + 0, Z + 1, Block->topMinX, Block->topMinY);

                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 1);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 0);
                    Indices.Emplace(CurrentVertex + 2);
                    Indices.Emplace(CurrentVertex + 3);
                    CurrentVertex += 4;
                }

                const voxel_t DownVoxel = this->GetOwner().GetRawVoxelDataByNonZeroOrigin(VoxelKey.GetDownKey(), ECompileTimeVoxels::Air);
                if (DownVoxel == ECompileTimeVoxels::Air)
                {
                    Vertices.Emplace(X + 1, Y + 1, Z + 0, Block->bottomMinX, Block->bottomMaxY);
                    Vertices.Emplace(X + 1, Y + 0, Z + 0, Block->bottomMaxX, Block->bottomMaxY);
                    Vertices.Emplace(X + 0, Y + 0, Z + 0, Block->bottomMaxX, Block->bottomMinY);
                    Vertices.Emplace(X + 0, Y + 1, Z + 0, Block->bottomMinX, Block->bottomMinY);

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
