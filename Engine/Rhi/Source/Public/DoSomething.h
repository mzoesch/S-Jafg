// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include <glm/fwd.hpp>

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

namespace Jafg
{
class Shader;
}

namespace JustTemporary
{

RHI_API void A(uint32& Texture);
RHI_API void B(uint32& Texture);
RHI_API void C(LIntVector2& WindowDimensions, glm::mat4 View, float Zoom, Jafg::Shader* ShaderProgram);

//* Chunk.cpp

RHI_API void D(unsigned int vbo, unsigned int ebo, unsigned int vertexArrayObject);
RHI_API void E(unsigned int vertexArrayObject, unsigned int vbo, unsigned int ebo,
    std::vector<unsigned int>& indices, std::vector<Vertex> vertices);

RHI_API void F(unsigned int vertexArrayObject, glm::vec3 worldPos, unsigned int numTriangles, unsigned int modelLoc);

RHI_API uint32 G(Jafg::Shader *ShaderProgram);

}
