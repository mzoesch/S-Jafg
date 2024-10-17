// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "MyWorld/Chunk.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "MyWorld/Blocks.h"
#include "MyWorld/WorldGen.h"
#include "MyWorld/WorldStatics.h"

AChunk::AChunk(glm::vec3 GlobalChunkLocation)
{
    this->chunkPos = GlobalChunkLocation;
    worldPos = glm::vec3(GlobalChunkLocation.x * WorldStatics::ChunkSize,
                         GlobalChunkLocation.y * WorldStatics::ChunkSize,
                         GlobalChunkLocation.z * WorldStatics::ChunkSize);

    ready = false;
    generated = false;

    this->RawVoxelData = new uint32[WorldStatics::VoxelCount];
    memset(RawVoxelData, 0, WorldStatics::VoxelCount * sizeof(uint32));

    chunkThread = std::thread(&AChunk::GenerateChunk, this);

    std::cout << "Created chunk at: " << chunkPos.x << ", " << chunkPos.y << ", " << chunkPos.z << '\n';
    std::cout.flush();
}

AChunk::~AChunk()
{
    if (chunkThread.joinable())
        chunkThread.join();

    delete[] RawVoxelData;

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vertexArrayObject);
}

void AChunk::GenerateChunk()
{
    //std::cout << "Started thread: " << std::this_thread::get_id() << '\n';

    WorldGen::GenerateChunkData(chunkPos.x, chunkPos.y, chunkPos.z, WorldStatics::ChunkSize, RawVoxelData);

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
    WorldGen::GenerateChunkData(chunkPos.x + 1, chunkPos.y, chunkPos.z, WorldStatics::ChunkSize, northData);
    WorldGen::GenerateChunkData(chunkPos.x - 1, chunkPos.y, chunkPos.z, WorldStatics::ChunkSize, southData);
    WorldGen::GenerateChunkData(chunkPos.x, chunkPos.y + 1, chunkPos.z, WorldStatics::ChunkSize, eastData);
    WorldGen::GenerateChunkData(chunkPos.x, chunkPos.y - 1, chunkPos.z, WorldStatics::ChunkSize, westData);
    WorldGen::GenerateChunkData(chunkPos.x, chunkPos.y, chunkPos.z + 1, WorldStatics::ChunkSize, upData);
    WorldGen::GenerateChunkData(chunkPos.x, chunkPos.y, chunkPos.z - 1, WorldStatics::ChunkSize, downData);

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

                        vertices.push_back(Vertex(x + 1, y + 0, z + 0, block->sideMinX, block->sideMinY));
                        vertices.push_back(Vertex(x + 0, y + 0, z + 0, block->sideMaxX, block->sideMinY));
                        vertices.push_back(Vertex(x + 1, y + 0, z + 1, block->sideMinX, block->sideMaxY));
                        vertices.push_back(Vertex(x + 0, y + 0, z + 1, block->sideMaxX, block->sideMaxY));

                        indices.push_back(currentVertex + 0);
                        indices.push_back(currentVertex + 3);
                        indices.push_back(currentVertex + 1);
                        indices.push_back(currentVertex + 0);
                        indices.push_back(currentVertex + 2);
                        indices.push_back(currentVertex + 3);
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

                        vertices.push_back(Vertex(x + 0, y + 1, z + 0, block->sideMinX, block->sideMinY));
                        vertices.push_back(Vertex(x + 1, y + 1, z + 0, block->sideMaxX, block->sideMinY));
                        vertices.push_back(Vertex(x + 0, y + 1, z + 1, block->sideMinX, block->sideMaxY));
                        vertices.push_back(Vertex(x + 1, y + 1, z + 1, block->sideMaxX, block->sideMaxY));

                        indices.push_back(currentVertex + 0);
                        indices.push_back(currentVertex + 3);
                        indices.push_back(currentVertex + 1);
                        indices.push_back(currentVertex + 0);
                        indices.push_back(currentVertex + 2);
                        indices.push_back(currentVertex + 3);
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

                        vertices.push_back(Vertex(x + 0, y + 0, z + 0, block->sideMinX, block->sideMinY));
                        vertices.push_back(Vertex(x + 0, y + 1, z + 0, block->sideMaxX, block->sideMinY));
                        vertices.push_back(Vertex(x + 0, y + 0, z + 1, block->sideMinX, block->sideMaxY));
                        vertices.push_back(Vertex(x + 0, y + 1, z + 1, block->sideMaxX, block->sideMaxY));

                        indices.push_back(currentVertex + 0);
                        indices.push_back(currentVertex + 3);
                        indices.push_back(currentVertex + 1);
                        indices.push_back(currentVertex + 0);
                        indices.push_back(currentVertex + 2);
                        indices.push_back(currentVertex + 3);
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

                        vertices.push_back(Vertex(x + 1, y + 1, z + 0, block->sideMinX, block->sideMinY));
                        vertices.push_back(Vertex(x + 1, y + 0, z + 0, block->sideMaxX, block->sideMinY));
                        vertices.push_back(Vertex(x + 1, y + 1, z + 1, block->sideMinX, block->sideMaxY));
                        vertices.push_back(Vertex(x + 1, y + 0, z + 1, block->sideMaxX, block->sideMaxY));

                        indices.push_back(currentVertex + 0);
                        indices.push_back(currentVertex + 3);
                        indices.push_back(currentVertex + 1);
                        indices.push_back(currentVertex + 0);
                        indices.push_back(currentVertex + 2);
                        indices.push_back(currentVertex + 3);
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

                        vertices.push_back(Vertex(x + 1, y + 1, z + 0, block->bottomMinX, block->bottomMinY));
                        vertices.push_back(Vertex(x + 0, y + 1, z + 0, block->bottomMaxX, block->bottomMinY));
                        vertices.push_back(Vertex(x + 1, y + 0, z + 0, block->bottomMinX, block->bottomMaxY));
                        vertices.push_back(Vertex(x + 0, y + 0, z + 0, block->bottomMaxX, block->bottomMaxY));

                        indices.push_back(currentVertex + 0);
                        indices.push_back(currentVertex + 3);
                        indices.push_back(currentVertex + 1);
                        indices.push_back(currentVertex + 0);
                        indices.push_back(currentVertex + 2);
                        indices.push_back(currentVertex + 3);
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

                        vertices.push_back(Vertex(x + 0, y + 1, z + 1, block->topMinX, block->topMinY));
                        vertices.push_back(Vertex(x + 1, y + 1, z + 1, block->topMaxX, block->topMinY));
                        vertices.push_back(Vertex(x + 0, y + 0, z + 1, block->topMinX, block->topMaxY));
                        vertices.push_back(Vertex(x + 1, y + 0, z + 1, block->topMaxX, block->topMaxY));

                        indices.push_back(currentVertex + 0);
                        indices.push_back(currentVertex + 3);
                        indices.push_back(currentVertex + 1);
                        indices.push_back(currentVertex + 0);
                        indices.push_back(currentVertex + 2);
                        indices.push_back(currentVertex + 3);
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

    generated = true;

    //std::cout << "Generated: " << generated << '\n';
}

void AChunk::Render(unsigned int modelLoc)
{
    if (!ready)
    {
        if (generated)
        {
            numTriangles = indices.size();

            glGenVertexArrays(1, &vertexArrayObject);
            glBindVertexArray(vertexArrayObject);

            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, posX));
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texGridX));
            glEnableVertexAttribArray(1);

            glGenBuffers(1, &ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
                         GL_STATIC_DRAW);
            ready = true;
        }

        return;
    }

    //std::cout << "Rendering chunk " << chunkPos.x << ", " << chunkPos.y << ", " << chunkPos.z << '\n'
    //	<< "Chunk VAO: " << vertexArrayObject << '\n' << "Triangles: " << numTriangles << '\n';

    glBindVertexArray(vertexArrayObject);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, worldPos);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, numTriangles, GL_UNSIGNED_INT, 0);
}
