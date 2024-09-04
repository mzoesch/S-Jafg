// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

class VertexBuffer
{
public:

    VertexBuffer(const void* Data, const uint32 Size);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

private:

    uint32 RendererId;
};
