// Copyright mzoesch. All rights reserved.

#include "IndexBuffer.h"
#include "CoreAFX.h"
#include "Renderer.h"

IndexBuffer::IndexBuffer(const uint32* Data, const uint32 Count) : Count(Count)
{
    static_assert(sizeof(uint32) == sizeof(GLuint), "sizeof(uint32) != sizeof(GLuint)");

    GL_CALL( glGenBuffers(1, &this->RendererId) )
    GL_CALL( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->RendererId) )
    GL_CALL( glBufferData(GL_ELEMENT_ARRAY_BUFFER, Count * sizeof(uint32), Data, GL_STATIC_DRAW) )

    return;
}

IndexBuffer::~IndexBuffer()
{
    GL_CALL( glDeleteBuffers(1, &this->RendererId) )
}

void IndexBuffer::Bind() const
{
    GL_CALL( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->RendererId) )
}

void IndexBuffer::Unbind() const
{
    GL_CALL( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) )
}
