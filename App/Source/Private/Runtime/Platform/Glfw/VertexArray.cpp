// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "VertexArray.h"
#include "Renderer.h"

VertexArray::VertexArray()
{
    GL_CALL( glGenVertexArrays(1, &RenderedId) )
}

VertexArray::~VertexArray()
{
    GL_CALL( glDeleteVertexArrays(1, &RenderedId) )
}

void VertexArray::AddBuffer(const VertexBuffer& Vb, const VertexBufferLayout& Layout)
{
    this->Bind();

    Vb.Bind();

    const auto& Elements = Layout.GetElements();
    uint32 Offset = 0;
    for (uint32 i = 0; i < Elements.size(); i++)
    {
        const auto& Element = Elements[i];

        GL_CALL( glEnableVertexAttribArray(i) )
        GL_CALL( glVertexAttribPointer(
            i, Element.Count, Element.Type, Element.bNormalized,
            Layout.GetStride(), (const void*)Offset
        ) )

        Offset += Element.Count * VertexBufferElement::GetSizeOfType(Element.Type);
    }

    return;
}

void VertexArray::Bind() const
{
    GL_CALL( glBindVertexArray(RenderedId) )
}

void VertexArray::Unbind() const
{
    GL_CALL( glBindVertexArray(0) )
}
