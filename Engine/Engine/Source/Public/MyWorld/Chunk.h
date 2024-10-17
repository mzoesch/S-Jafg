// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

#include <vector>
#include <thread>
#include <glm/glm.hpp>
#include "MyWorld/WorldStatics.h"

struct Vertex
{
    char posX, posY, posZ;
    char texGridX, texGridY;

    Vertex(char _posX, char _posY, char _posZ, char _texGridX, char _texGridY)
    {
        posX = _posX;
        posY = _posY;
        posZ = _posZ;

        texGridX = _texGridX;
        texGridY = _texGridY;
    }
};

class AChunk
{

public:

    AChunk(glm::vec3 chunkPos);
    ~AChunk();

    void GenerateChunk();
    void Render(unsigned int modelLoc);

    uint32* RawVoxelData;
    // std::vector<unsigned int> RawVoxelData;
    glm::vec3 chunkPos;
    bool ready;
    bool generated;

    FORCEINLINE static int32 GetIndex(const int32 X, const int32 Y, const int32 Z)
    {
        return X + Y * WorldStatics::ChunkSize + Z * WorldStatics::ChunkSize * WorldStatics::ChunkSize;
    }

private:
    unsigned int vertexArrayObject;
    unsigned int vbo, ebo;
    unsigned int numTriangles;
    glm::vec3 worldPos;
    std::thread chunkThread;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};
