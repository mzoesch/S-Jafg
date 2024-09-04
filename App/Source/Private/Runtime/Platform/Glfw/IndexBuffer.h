// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

class IndexBuffer
{
public:

    IndexBuffer(const uint32* Data, const uint32 Count);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    FORCEINLINE uint32 GetCount() const { return this->Count; }

private:

    uint32 RendererId;
    uint32 Count;
};
