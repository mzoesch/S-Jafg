// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

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
class RHI_API Shader
{
public:
    // the program ID
    unsigned int ID;

    Shader() = default;
    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void Use();
    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
};
}
