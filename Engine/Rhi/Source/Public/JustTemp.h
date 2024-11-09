// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include <glm/glm.hpp>
#include "RhiFramework/Shader.h"

namespace Jafg
{

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

}

namespace JustTemp
{

uint32 RHI_API D(Jafg::LShader* ShaderProgram);


void RHI_API E(uint32* vertexArrayObject, uint32* vbo, uint32* ebo);
void RHI_API F(uint32* vertexArrayObject, uint32* vbo, uint32* ebo, const Jafg::TdhArray<Jafg::Vertex>* vertices, const Jafg::TdhArray<uint32>* indices
    );
void RHI_API G(uint32* vertexArrayObject, uint32 numTriangles, Jafg::LVector* worldPos, uint32* modelLoc);

}
