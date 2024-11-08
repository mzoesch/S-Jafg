// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "MyWorld/Chunk/Chunk.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "JustTemp.h"
#include "MyWorld/Blocks.h"
#include "MyWorld/WorldGen.h"
#include "MyWorld/WorldStatics.h"

void Jafg::AChunk::BeginLife()
{
    Super::BeginLife();

    this->WorldLocation = this->ChunkKey.ToWorldSpaceVector();

    bReady = false;
    bGenerated = false;

    this->RawVoxelData = new uint32[MwStatics::VoxelCount];
    memset(this->RawVoxelData, 0, MwStatics::VoxelCount * sizeof(uint32));
    this->GenerateChunk();

    return;
}

void Jafg::AChunk::EndLife()
{
    Super::EndLife();

    JustTemp::E(&VertexArrayObject, &Vbo, &Vbo);

    delete[] RawVoxelData;
    RawVoxelData = nullptr;

    Vertices.Empty();
    Indices.Empty();

    return;
}

void Jafg::AChunk::GenerateChunk()
{
    //std::cout << "Started thread: " << std::this_thread::get_id() << '\n';

    WorldGen::GenerateChunkData(static_cast<int>(ChunkPos.x), static_cast<int>(ChunkPos.y), static_cast<int>(ChunkPos.z), WorldStatics::ChunkSize, RawVoxelData);

    // std::vector<unsigned int> northData, southData, eastData, westData, upData, downData;
    uint32* northData = new uint32[WorldStatics::VoxelCount];
    uint32* southData = new uint32[WorldStatics::VoxelCount];
    uint32* eastData = new uint32[WorldStatics::VoxelCount];
    uint32* westData = new uint32[WorldStatics::VoxelCount];
    uint32* upData = new uint32[WorldStatics::VoxelCount];
    uint32* downData = new uint32[WorldStatics::VoxelCount];
    memset(northData, 0, WorldStatics::VoxelCount * sizeof(uint32));
    memset(southData, 0, WorldStatics::VoxelCount * sizeof(uint32));
    memset(eastData, 0, WorldStatics::VoxelCount * sizeof(uint32));
    memset(westData, 0, WorldStatics::VoxelCount * sizeof(uint32));
    memset(upData, 0, WorldStatics::VoxelCount * sizeof(uint32));
    memset(downData, 0, WorldStatics::VoxelCount * sizeof(uint32));

    // WorldGen::GenerateChunkData(chunkPos.x, chunkPos.y, chunkPos.z - 1, WorldStatics::ChunkSize, northData);
    // WorldGen::GenerateChunkData(chunkPos.x, chunkPos.y, chunkPos.z + 1, WorldStatics::ChunkSize, southData);
    // WorldGen::GenerateChunkData(chunkPos.x + 1, chunkPos.y, chunkPos.z, WorldStatics::ChunkSize, eastData);
    // WorldGen::GenerateChunkData(chunkPos.x - 1, chunkPos.y, chunkPos.z, WorldStatics::ChunkSize, westData);
    // WorldGen::GenerateChunkData(chunkPos.x, chunkPos.y + 1, chunkPos.z, WorldStatics::ChunkSize, upData);
    // WorldGen::GenerateChunkData(chunkPos.x, chunkPos.y - 1, chunkPos.z, WorldStatics::ChunkSize, downData);
    WorldGen::GenerateChunkData(static_cast<int>(ChunkPos.x + 1), static_cast<int>(ChunkPos.y), static_cast<int>(ChunkPos.z), WorldStatics::ChunkSize, northData);
    WorldGen::GenerateChunkData(static_cast<int>(ChunkPos.x - 1), static_cast<int>(ChunkPos.y), static_cast<int>(ChunkPos.z), WorldStatics::ChunkSize, southData);
    WorldGen::GenerateChunkData(static_cast<int>(ChunkPos.x), static_cast<int>(ChunkPos.y + 1), static_cast<int>(ChunkPos.z), WorldStatics::ChunkSize, eastData);
    WorldGen::GenerateChunkData(static_cast<int>(ChunkPos.x), static_cast<int>(ChunkPos.y - 1), static_cast<int>(ChunkPos.z), WorldStatics::ChunkSize, westData);
    WorldGen::GenerateChunkData(static_cast<int>(ChunkPos.x), static_cast<int>(ChunkPos.y), static_cast<int>(ChunkPos.z + 1), WorldStatics::ChunkSize, upData);
    WorldGen::GenerateChunkData(static_cast<int>(ChunkPos.x), static_cast<int>(ChunkPos.y), static_cast<int>(ChunkPos.z - 1), WorldStatics::ChunkSize, downData);

    //std::cout << "Got chunk data in thread: " << std::this_thread::get_id() << '\n';

    unsigned int currentVertex = 0;
    for (char x = 0; x < WorldStatics::ChunkSize; x++)
    {
        for (char y = 0; y < WorldStatics::ChunkSize; y++)
        {
            for (char z = 0; z < WorldStatics::ChunkSize; z++)
            {
                int Index = GetIndex(x, y, z);
                // int index = x * WorldStatics::ChunkSize * WorldStatics::ChunkSize + y * WorldStatics::ChunkSize + z;
                if (RawVoxelData[Index] == 0)
                    continue;

                const Block* block = &Blocks::blocks[RawVoxelData[Index]];

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

                        Indices.Emplace(currentVertex + 0);
                        Indices.Emplace(currentVertex + 3);
                        Indices.Emplace(currentVertex + 1);
                        Indices.Emplace(currentVertex + 0);
                        Indices.Emplace(currentVertex + 2);
                        Indices.Emplace(currentVertex + 3);
                        currentVertex += 4;
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

                        Indices.Emplace(currentVertex + 0);
                        Indices.Emplace(currentVertex + 3);
                        Indices.Emplace(currentVertex + 1);
                        Indices.Emplace(currentVertex + 0);
                        Indices.Emplace(currentVertex + 2);
                        Indices.Emplace(currentVertex + 3);
                        currentVertex += 4;
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

                        Indices.Emplace(currentVertex + 0);
                        Indices.Emplace(currentVertex + 3);
                        Indices.Emplace(currentVertex + 1);
                        Indices.Emplace(currentVertex + 0);
                        Indices.Emplace(currentVertex + 2);
                        Indices.Emplace(currentVertex + 3);
                        currentVertex += 4;
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

                        Indices.Emplace(currentVertex + 0);
                        Indices.Emplace(currentVertex + 3);
                        Indices.Emplace(currentVertex + 1);
                        Indices.Emplace(currentVertex + 0);
                        Indices.Emplace(currentVertex + 2);
                        Indices.Emplace(currentVertex + 3);
                        currentVertex += 4;
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

                        Indices.Emplace(currentVertex + 0);
                        Indices.Emplace(currentVertex + 3);
                        Indices.Emplace(currentVertex + 1);
                        Indices.Emplace(currentVertex + 0);
                        Indices.Emplace(currentVertex + 2);
                        Indices.Emplace(currentVertex + 3);
                        currentVertex += 4;
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

                        Indices.Emplace(currentVertex + 0);
                        Indices.Emplace(currentVertex + 3);
                        Indices.Emplace(currentVertex + 1);
                        Indices.Emplace(currentVertex + 0);
                        Indices.Emplace(currentVertex + 2);
                        Indices.Emplace(currentVertex + 3);
                        currentVertex += 4;
                    }
                }
            }
        }
    }

    //std::cout << "Finished generating in thread: " << std::this_thread::get_id() << '\n';

    delete[] northData;
    delete[] southData;
    delete[] eastData;
    delete[] westData;
    delete[] upData;
    delete[] downData;

    bGenerated = true;

    //std::cout << "Generated: " << generated << '\n';
}

void Jafg::AChunk::Render(unsigned int ModelLoc)
{
    if (!bReady)
    {
        if (bGenerated)
        {
            JustTemp::F(&VertexArrayObject, &Vbo, &Ebo, &Vertices, &Indices, &NumTriangles);
            // numTriangles = static_cast<unsigned int>( indices.size() );
            //
            // glGenVertexArrays(1, &vertexArrayObject);
            // glBindVertexArray(vertexArrayObject);
            //
            // glGenBuffers(1, &vbo);
            // glBindBuffer(GL_ARRAY_BUFFER, vbo);
            // glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
            //
            // glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, posX)));
            // glEnableVertexAttribArray(0);
            // glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texGridX)));
            // glEnableVertexAttribArray(1);
            //
            // glGenBuffers(1, &ebo);
            // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            // glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(),
            //              GL_STATIC_DRAW);
            bReady = true;
        }

        return;
    }

    //std::cout << "Rendering chunk " << chunkPos.x << ", " << chunkPos.y << ", " << chunkPos.z << '\n'
    //	<< "Chunk VAO: " << vertexArrayObject << '\n' << "Triangles: " << numTriangles << '\n';

    JustTemp::G(&VertexArrayObject, &NumTriangles, &WorldLocation, &ModelLoc);

    // glBindVertexArray(vertexArrayObject);
    //
    // glm::mat4 model = glm::mat4(1.0f);
    // model = glm::
    // translate(model, worldPos);
    // glUniformMatrix4fv(static_cast<GLint>(modelLoc), 1, GL_FALSE, glm::value_ptr(model));
    //
    // glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(numTriangles), GL_UNSIGNED_INT, 0);

    return;
}
