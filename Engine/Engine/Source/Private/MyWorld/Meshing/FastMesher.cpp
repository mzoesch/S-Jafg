// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "MyWorld/Meshing/FastMesher.h"
#include "MyWorld/Blocks.h"
#include "MyWorld/WorldStatics.h"
#include "MyWorld/Chunk/Chunk.h"

void Jafg::LFastChunkMesher::GenerateProceduralMesh()
{
    glm::vec3 ChunkPos = this->GetOwner().ChunkPos;

    unsigned int CurrentVertex = 0;
    for (LVoxelKeyDomainTy x = 0; x < WorldStatics::ChunkSize; x++)
    {
        for (LVoxelKeyDomainTy y = 0; y < WorldStatics::ChunkSize; y++)
        {
            for (LVoxelKeyDomainTy z = 0; z < WorldStatics::ChunkSize; z++)
            {
                AChunk::LVoxelIndex Index = this->GetOwner().GetRawVoxelIndex(x, y, z);
                // int index = x * WorldStatics::ChunkSize * WorldStatics::ChunkSize + y * WorldStatics::ChunkSize + z;
                if (this->GetOwner().RawVoxelData[Index] == 0)
                    continue;

                const Block* block = &Blocks::blocks[this->GetOwner().RawVoxelData[Index]];

                // North
                {
                    int northBlock = 0;
                    // if (y > 0)
                    // {
                    //     int northIndex = x * WorldStatics::ChunkSize * WorldStatics::ChunkSize + (y - 1) *
                    //         WorldStatics::ChunkSize + z;
                    //     northBlock = RawVoxelData[northIndex];
                    // }
                    // else
                    // {
                    //     int northIndex = x * WorldStatics::ChunkSize * WorldStatics::ChunkSize + (
                    //         WorldStatics::ChunkSize - 1) * WorldStatics::ChunkSize + z;
                    //     northBlock = northData[northIndex];
                    // }

                    if (northBlock == 0)
                    {
                        // vertices.push_back(Vertex(x + 1, y + 0, z + 0, block->sideMinX, block->sideMinY));
                        // vertices.push_back(Vertex(x + 0, y + 0, z + 0, block->sideMaxX, block->sideMinY));
                        // vertices.push_back(Vertex(x + 1, y + 1, z + 0, block->sideMinX, block->sideMaxY));
                        // vertices.push_back(Vertex(x + 0, y + 1, z + 0, block->sideMaxX, block->sideMaxY));

                        Vertices.Emplace(x + 1, y + 0, z + 0, block->sideMinX, block->sideMinY);
                        Vertices.Emplace(x + 0, y + 0, z + 0, block->sideMaxX, block->sideMinY);
                        Vertices.Emplace(x + 1, y + 0, z + 1, block->sideMinX, block->sideMaxY);
                        Vertices.Emplace(x + 0, y + 0, z + 1, block->sideMaxX, block->sideMaxY);

                        Indices.Emplace(CurrentVertex + 0);
                        Indices.Emplace(CurrentVertex + 3);
                        Indices.Emplace(CurrentVertex + 1);
                        Indices.Emplace(CurrentVertex + 0);
                        Indices.Emplace(CurrentVertex + 2);
                        Indices.Emplace(CurrentVertex + 3);
                        CurrentVertex += 4;
                    }
                }

                // South
                {
                    int southBlock = 0;
                    // if (y < WorldStatics::ChunkSize - 1)
                    // {
                    //     int southIndex = x * WorldStatics::ChunkSize * WorldStatics::ChunkSize + (y + 1) *
                    //         WorldStatics::ChunkSize + z;
                    //     southBlock = RawVoxelData[southIndex];
                    // }
                    // else
                    // {
                    //     int southIndex = x * WorldStatics::ChunkSize * WorldStatics::ChunkSize + 0 *
                    //         WorldStatics::ChunkSize + z;
                    //     southBlock = southData[southIndex];
                    // }
                    if (southBlock == 0)
                    {
                        // vertices.push_back(Vertex(x + 0, y + 0, z + 1, block->sideMinX, block->sideMinY));
                        // vertices.push_back(Vertex(x + 1, y + 0, z + 1, block->sideMaxX, block->sideMinY));
                        // vertices.push_back(Vertex(x + 0, y + 1, z + 1, block->sideMinX, block->sideMaxY));
                        // vertices.push_back(Vertex(x + 1, y + 1, z + 1, block->sideMaxX, block->sideMaxY));

                        Vertices.Emplace(x + 0, y + 1, z + 0, block->sideMinX, block->sideMinY);
                        Vertices.Emplace(x + 1, y + 1, z + 0, block->sideMaxX, block->sideMinY);
                        Vertices.Emplace(x + 0, y + 1, z + 1, block->sideMinX, block->sideMaxY);
                        Vertices.Emplace(x + 1, y + 1, z + 1, block->sideMaxX, block->sideMaxY);

                        Indices.Emplace(CurrentVertex + 0);
                        Indices.Emplace(CurrentVertex + 3);
                        Indices.Emplace(CurrentVertex + 1);
                        Indices.Emplace(CurrentVertex + 0);
                        Indices.Emplace(CurrentVertex + 2);
                        Indices.Emplace(CurrentVertex + 3);
                        CurrentVertex += 4;
                    }
                }

                // West
                {
                    int westBlock = 0;
                    // if (x > 0)
                    // {
                    //     int blockIndex = (x - 1) * WorldStatics::ChunkSize * WorldStatics::ChunkSize + y *
                    //         WorldStatics::ChunkSize + z;
                    //     westBlock = RawVoxelData[blockIndex];
                    // }
                    // else
                    // {
                    //     int blockIndex = (WorldStatics::ChunkSize - 1) * WorldStatics::ChunkSize *
                    //         WorldStatics::ChunkSize + y * WorldStatics::ChunkSize + z;
                    //     westBlock = westData[blockIndex];
                    // }
                    if (westBlock == 0)
                    {
                        // vertices.push_back(Vertex(x + 0, y + 0, z + 0, block->sideMinX, block->sideMinY));
                        // vertices.push_back(Vertex(x + 0, y + 0, z + 1, block->sideMaxX, block->sideMinY));
                        // vertices.push_back(Vertex(x + 0, y + 1, z + 0, block->sideMinX, block->sideMaxY));
                        // vertices.push_back(Vertex(x + 0, y + 1, z + 1, block->sideMaxX, block->sideMaxY));

                        Vertices.Emplace(x + 0, y + 0, z + 0, block->sideMinX, block->sideMinY);
                        Vertices.Emplace(x + 0, y + 1, z + 0, block->sideMaxX, block->sideMinY);
                        Vertices.Emplace(x + 0, y + 0, z + 1, block->sideMinX, block->sideMaxY);
                        Vertices.Emplace(x + 0, y + 1, z + 1, block->sideMaxX, block->sideMaxY);

                        Indices.Emplace(CurrentVertex + 0);
                        Indices.Emplace(CurrentVertex + 3);
                        Indices.Emplace(CurrentVertex + 1);
                        Indices.Emplace(CurrentVertex + 0);
                        Indices.Emplace(CurrentVertex + 2);
                        Indices.Emplace(CurrentVertex + 3);
                        CurrentVertex += 4;
                    }
                }

                // East
                {
                    int eastBlock = 0;
                    // if (x < WorldStatics::ChunkSize - 1)
                    // {
                    //     int blockIndex = (x + 1) * WorldStatics::ChunkSize * WorldStatics::ChunkSize + y *
                    //         WorldStatics::ChunkSize + z;
                    //     eastBlock = RawVoxelData[blockIndex];
                    // }
                    // else
                    // {
                    //     int blockIndex = 0 * WorldStatics::ChunkSize * WorldStatics::ChunkSize + y *
                    //         WorldStatics::ChunkSize + z;
                    //     eastBlock = eastData[blockIndex];
                    // }
                    if (eastBlock == 0)
                    {
                        // vertices.push_back(Vertex(x + 1, y + 0, z + 1, block->sideMinX, block->sideMinY));
                        // vertices.push_back(Vertex(x + 1, y + 0, z + 0, block->sideMaxX, block->sideMinY));
                        // vertices.push_back(Vertex(x + 1, y + 1, z + 1, block->sideMinX, block->sideMaxY));
                        // vertices.push_back(Vertex(x + 1, y + 1, z + 0, block->sideMaxX, block->sideMaxY));

                        Vertices.Emplace(x + 1, y + 1, z + 0, block->sideMinX, block->sideMinY);
                        Vertices.Emplace(x + 1, y + 0, z + 0, block->sideMaxX, block->sideMinY);
                        Vertices.Emplace(x + 1, y + 1, z + 1, block->sideMinX, block->sideMaxY);
                        Vertices.Emplace(x + 1, y + 0, z + 1, block->sideMaxX, block->sideMaxY);

                        Indices.Emplace(CurrentVertex + 0);
                        Indices.Emplace(CurrentVertex + 3);
                        Indices.Emplace(CurrentVertex + 1);
                        Indices.Emplace(CurrentVertex + 0);
                        Indices.Emplace(CurrentVertex + 2);
                        Indices.Emplace(CurrentVertex + 3);
                        CurrentVertex += 4;
                    }
                }

                // Bottom
                {
                    int bottomBlock = 0;
                    // if (z > 0)
                    // {
                    //     int blockIndex = x * WorldStatics::ChunkSize * WorldStatics::ChunkSize + y *
                    //         WorldStatics::ChunkSize + (z - 1);
                    //     bottomBlock = RawVoxelData[blockIndex];
                    // }
                    // else
                    // {
                    //     int blockIndex = x * WorldStatics::ChunkSize * WorldStatics::ChunkSize + y *
                    //         WorldStatics::ChunkSize + (WorldStatics::ChunkSize - 1);
                    //     bottomBlock = downData[blockIndex];
                    // }
                    if (bottomBlock == 0)
                    {
                        // vertices.push_back(Vertex(x + 1, y + 0, z + 1, block->bottomMinX, block->bottomMinY));
                        // vertices.push_back(Vertex(x + 0, y + 0, z + 1, block->bottomMaxX, block->bottomMinY));
                        // vertices.push_back(Vertex(x + 1, y + 0, z + 0, block->bottomMinX, block->bottomMaxY));
                        // vertices.push_back(Vertex(x + 0, y + 0, z + 0, block->bottomMaxX, block->bottomMaxY));

                        Vertices.Emplace(x + 1, y + 1, z + 0, block->bottomMinX, block->bottomMinY);
                        Vertices.Emplace(x + 0, y + 1, z + 0, block->bottomMaxX, block->bottomMinY);
                        Vertices.Emplace(x + 1, y + 0, z + 0, block->bottomMinX, block->bottomMaxY);
                        Vertices.Emplace(x + 0, y + 0, z + 0, block->bottomMaxX, block->bottomMaxY);

                        Indices.Emplace(CurrentVertex + 0);
                        Indices.Emplace(CurrentVertex + 3);
                        Indices.Emplace(CurrentVertex + 1);
                        Indices.Emplace(CurrentVertex + 0);
                        Indices.Emplace(CurrentVertex + 2);
                        Indices.Emplace(CurrentVertex + 3);
                        CurrentVertex += 4;
                    }
                }

                // Top
                {
                    int topBlock = 0;
                    // if (z < WorldStatics::ChunkSize - 1)
                    // {
                    //     int blockIndex = x * WorldStatics::ChunkSize * WorldStatics::ChunkSize + y *
                    //         WorldStatics::ChunkSize + (z + 1);
                    //     topBlock = RawVoxelData[blockIndex];
                    // }
                    // else
                    // {
                    //     int blockIndex = x * WorldStatics::ChunkSize * WorldStatics::ChunkSize + y *
                    //         WorldStatics::ChunkSize + 0;
                    //     topBlock = upData[blockIndex];
                    // }
                    if (topBlock == 0)
                    {
                        // vertices.push_back(Vertex(x + 0, y + 1, z + 1, block->topMinX, block->topMinY));
                        // vertices.push_back(Vertex(x + 1, y + 1, z + 1, block->topMaxX, block->topMinY));
                        // vertices.push_back(Vertex(x + 0, y + 1, z + 0, block->topMinX, block->topMaxY));
                        // vertices.push_back(Vertex(x + 1, y + 1, z + 0, block->topMaxX, block->topMaxY));

                        Vertices.Emplace(x + 0, y + 1, z + 1, block->topMinX, block->topMinY);
                        Vertices.Emplace(x + 1, y + 1, z + 1, block->topMaxX, block->topMinY);
                        Vertices.Emplace(x + 0, y + 0, z + 1, block->topMinX, block->topMaxY);
                        Vertices.Emplace(x + 1, y + 0, z + 1, block->topMaxX, block->topMaxY);

                        Indices.Emplace(CurrentVertex + 0);
                        Indices.Emplace(CurrentVertex + 3);
                        Indices.Emplace(CurrentVertex + 1);
                        Indices.Emplace(CurrentVertex + 0);
                        Indices.Emplace(CurrentVertex + 2);
                        Indices.Emplace(CurrentVertex + 3);
                        CurrentVertex += 4;
                    }
                }
            }
        }
    }

    return;
}
