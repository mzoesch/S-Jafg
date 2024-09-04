// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

class  VertexBuffer;
struct VertexBufferLayout;

class VertexArray
{
public:

    VertexArray();
    ~VertexArray();

    void AddBuffer(const VertexBuffer& Vb, const VertexBufferLayout& Layout);

    void Bind() const;
    void Unbind() const;

private:

    uint32 RenderedId;
};
