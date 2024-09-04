// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "glad/glad.h"

struct VertexBufferElement
{
    uint32 Type;
    uint32 Count;
    uint8  bNormalized;

    static uint32 GetSizeOfType(const uint32 Type)
    {
        switch (Type)
        {
            case GL_FLOAT:          return sizeof(GLfloat);
            case GL_UNSIGNED_INT:   return sizeof(GLuint);
            case GL_UNSIGNED_BYTE:  return sizeof(GLbyte);
            default:                break;
        }

        check( false )

        return 0;
    }
};

struct VertexBufferLayout
{
public:

    VertexBufferLayout();

    template <typename T>
    void Push(const uint32 Count)
    {
        check( false )
    }

    template <>
    void Push<float>(const uint32 Count)
    {
        this->Elements.push_back( { GL_FLOAT, Count, GL_FALSE } );
        Stride += Count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
    }

    template <>
    void Push<uint32>(const uint32 Count)
    {
        this->Elements.push_back( { GL_UNSIGNED_INT, Count, GL_FALSE } );
        Stride += Count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
    }

    template <>
    void Push<uint8>(const uint32 Count)
    {
        this->Elements.push_back( { GL_UNSIGNED_BYTE, Count, GL_TRUE } );
        Stride += Count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
    }

    FORCEINLINE auto   GetElements() const -> const std::vector<VertexBufferElement>& { return this->Elements; }
    FORCEINLINE uint32 GetStride() const { return this->Stride; }

private:

    std::vector<VertexBufferElement> Elements;
    uint32 Stride;
};
