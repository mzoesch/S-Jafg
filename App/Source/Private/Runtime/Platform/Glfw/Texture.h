// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

class Texture
{

public:

    explicit Texture(String InFilePath);
    ~Texture();

    void Bind(uint32 Slot = 0) const;
    void Unbind() const;

    FORCEINLINE int32 GetWidth() const { return Width; }
    FORCEINLINE int32 GetHeight() const { return Height; }

private:

    uint32 RendererId;
    String FilePath;
    uint8* LocalBuffer;
    int32 Width, Height, Bpp; // Bits per pixel
};
