// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include <glm/glm.hpp>
#include "RhiFramework/Shader.h"


namespace JustTemp
{

void RHI_API A(uint32* Texture);
void RHI_API B(uint32 Texture);
void RHI_API C(float Zoom, Jafg::Shader* ShaderProgram, LIntVector2 WindowDimensions, glm::mat4 View);
uint32 RHI_API D(Jafg::Shader* ShaderProgram);


void RHI_API E(uint32* vertexArrayObject, uint32* vbo, uint32* ebo);
void RHI_API F(uint32* vertexArrayObject, uint32* vbo, uint32* ebo, std::vector<Jafg::Vertex>* vertices, std::vector<uint32>* indices
    , uint32* numTriangles);
void RHI_API G(uint32* vertexArrayObject, uint32* numTriangles, glm::vec3* worldPos, uint32* modelLoc);

}
