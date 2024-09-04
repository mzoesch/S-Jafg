// Copyright mzoesch. All rights reserved.

#include "VertexBuffer.h"
#include "CoreAFX.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* Data, const uint32 Size)
{
    GL_CALL( glGenBuffers(1, &this->RendererId) )
    GL_CALL( glBindBuffer(GL_ARRAY_BUFFER, this->RendererId) )
    GL_CALL( glBufferData(GL_ARRAY_BUFFER, Size, Data, GL_STATIC_DRAW) )

    return;
}

VertexBuffer::~VertexBuffer()
{
    GL_CALL( glDeleteBuffers(1, &this->RendererId) )
}

void VertexBuffer::Bind() const
{
    GL_CALL( glBindBuffer(GL_ARRAY_BUFFER, this->RendererId) )
}

void VertexBuffer::Unbind() const
{
    GL_CALL( glBindBuffer(GL_ARRAY_BUFFER, 0) )
}
