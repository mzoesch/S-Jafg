// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "MyWorld/Meshing/FastMesher.h"
#include "MyWorld/Blocks.h"
#include "MyWorld/Chunk/Chunk.h"

void Jafg::LFastChunkMesher::GenerateProceduralMesh(const JVoxelSubsystem* VoxelSubsystem, const JMaterialSubsystem* MaterialSubsystem)
{
    unsigned int CurrentVertex = 0;
    // for (LVoxelKeyDomainTy x = 0; x < MwStatics::ChunkSize; x++)
    // {
    //     for (LVoxelKeyDomainTy y = 0; y < MwStatics::ChunkSize; y++)
    //     {
    //         for (LVoxelKeyDomainTy z = 0; z < MwStatics::ChunkSize; z++)
    //         {
    //             const AChunk::LVoxelIndex Index = AChunk::GetRawVoxelIndex(x, y, z);
    //
    //             if (this->GetOwner().RawVoxelData[Index] == 0)
    //             {
    //                 continue;
    //             }
    //
    //             const Block* block = &Blocks::blocks[this->GetOwner().RawVoxelData[Index]];
    //
    //             /* North */
    //             {
    //                 Vertices.Emplace(x + 1, y + 0, z + 0, block->sideMinX, block->sideMinY);
    //                 Vertices.Emplace(x + 0, y + 0, z + 0, block->sideMaxX, block->sideMinY);
    //                 Vertices.Emplace(x + 1, y + 0, z + 1, block->sideMinX, block->sideMaxY);
    //                 Vertices.Emplace(x + 0, y + 0, z + 1, block->sideMaxX, block->sideMaxY);
    //
    //                 Indices.Emplace(CurrentVertex + 0);
    //                 Indices.Emplace(CurrentVertex + 3);
    //                 Indices.Emplace(CurrentVertex + 1);
    //                 Indices.Emplace(CurrentVertex + 0);
    //                 Indices.Emplace(CurrentVertex + 2);
    //                 Indices.Emplace(CurrentVertex + 3);
    //                 CurrentVertex += 4;
    //             }
    //
    //             /* South */
    //             {
    //                 Vertices.Emplace(x + 0, y + 1, z + 0, block->sideMinX, block->sideMinY);
    //                 Vertices.Emplace(x + 1, y + 1, z + 0, block->sideMaxX, block->sideMinY);
    //                 Vertices.Emplace(x + 0, y + 1, z + 1, block->sideMinX, block->sideMaxY);
    //                 Vertices.Emplace(x + 1, y + 1, z + 1, block->sideMaxX, block->sideMaxY);
    //
    //                 Indices.Emplace(CurrentVertex + 0);
    //                 Indices.Emplace(CurrentVertex + 3);
    //                 Indices.Emplace(CurrentVertex + 1);
    //                 Indices.Emplace(CurrentVertex + 0);
    //                 Indices.Emplace(CurrentVertex + 2);
    //                 Indices.Emplace(CurrentVertex + 3);
    //                 CurrentVertex += 4;
    //             }
    //
    //             /* West */
    //             {
    //                 Vertices.Emplace(x + 0, y + 0, z + 0, block->sideMinX, block->sideMinY);
    //                 Vertices.Emplace(x + 0, y + 1, z + 0, block->sideMaxX, block->sideMinY);
    //                 Vertices.Emplace(x + 0, y + 0, z + 1, block->sideMinX, block->sideMaxY);
    //                 Vertices.Emplace(x + 0, y + 1, z + 1, block->sideMaxX, block->sideMaxY);
    //
    //                 Indices.Emplace(CurrentVertex + 0);
    //                 Indices.Emplace(CurrentVertex + 3);
    //                 Indices.Emplace(CurrentVertex + 1);
    //                 Indices.Emplace(CurrentVertex + 0);
    //                 Indices.Emplace(CurrentVertex + 2);
    //                 Indices.Emplace(CurrentVertex + 3);
    //                 CurrentVertex += 4;
    //             }
    //
    //             /* East */
    //             {
    //                 Vertices.Emplace(x + 1, y + 1, z + 0, block->sideMinX, block->sideMinY);
    //                 Vertices.Emplace(x + 1, y + 0, z + 0, block->sideMaxX, block->sideMinY);
    //                 Vertices.Emplace(x + 1, y + 1, z + 1, block->sideMinX, block->sideMaxY);
    //                 Vertices.Emplace(x + 1, y + 0, z + 1, block->sideMaxX, block->sideMaxY);
    //
    //                 Indices.Emplace(CurrentVertex + 0);
    //                 Indices.Emplace(CurrentVertex + 3);
    //                 Indices.Emplace(CurrentVertex + 1);
    //                 Indices.Emplace(CurrentVertex + 0);
    //                 Indices.Emplace(CurrentVertex + 2);
    //                 Indices.Emplace(CurrentVertex + 3);
    //                 CurrentVertex += 4;
    //             }
    //
    //             /* Bottom */
    //             {
    //                 Vertices.Emplace(x + 1, y + 1, z + 0, block->bottomMinX, block->bottomMinY);
    //                 Vertices.Emplace(x + 0, y + 1, z + 0, block->bottomMaxX, block->bottomMinY);
    //                 Vertices.Emplace(x + 1, y + 0, z + 0, block->bottomMinX, block->bottomMaxY);
    //                 Vertices.Emplace(x + 0, y + 0, z + 0, block->bottomMaxX, block->bottomMaxY);
    //
    //                 Indices.Emplace(CurrentVertex + 0);
    //                 Indices.Emplace(CurrentVertex + 3);
    //                 Indices.Emplace(CurrentVertex + 1);
    //                 Indices.Emplace(CurrentVertex + 0);
    //                 Indices.Emplace(CurrentVertex + 2);
    //                 Indices.Emplace(CurrentVertex + 3);
    //                 CurrentVertex += 4;
    //             }
    //
    //             /* Top */
    //             {
    //                 Vertices.Emplace(x + 0, y + 1, z + 1, block->topMinX, block->topMinY);
    //                 Vertices.Emplace(x + 1, y + 1, z + 1, block->topMaxX, block->topMinY);
    //                 Vertices.Emplace(x + 0, y + 0, z + 1, block->topMinX, block->topMaxY);
    //                 Vertices.Emplace(x + 1, y + 0, z + 1, block->topMaxX, block->topMaxY);
    //
    //                 Indices.Emplace(CurrentVertex + 0);
    //                 Indices.Emplace(CurrentVertex + 3);
    //                 Indices.Emplace(CurrentVertex + 1);
    //                 Indices.Emplace(CurrentVertex + 0);
    //                 Indices.Emplace(CurrentVertex + 2);
    //                 Indices.Emplace(CurrentVertex + 3);
    //                 CurrentVertex += 4;
    //             }
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
