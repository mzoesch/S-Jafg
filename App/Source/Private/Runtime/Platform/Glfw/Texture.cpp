// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Texture.h"
#include "Renderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

Texture::Texture(String InFilePath)
: RendererId(0), FilePath(std::move(InFilePath)), LocalBuffer(nullptr), Width(0), Height(0), Bpp(0)
{
    stbi_set_flip_vertically_on_load(1);
    LocalBuffer = stbi_load(FilePath.c_str(), &Width, &Height, &Bpp, 4);

    GL_CALL( glGenTextures(1, &RendererId) )
    GL_CALL( glBindTexture(GL_TEXTURE_2D, RendererId) )

    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) )
    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) )
    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ) )
    GL_CALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ) )

    GL_CALL( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, LocalBuffer) )

    GL_CALL( glBindTexture(GL_TEXTURE_2D, 0) )

    if (LocalBuffer)
    {
        stbi_image_free(LocalBuffer);
    }

    return;
}

Texture::~Texture()
{
    GL_CALL( glDeleteTextures(1, &RendererId) )
}

void Texture::Bind(uint32 Slot) const
{
    GL_CALL( glActiveTexture(GL_TEXTURE0 + Slot) )
    GL_CALL( glBindTexture(GL_TEXTURE_2D, RendererId) )
}

void Texture::Unbind() const
{
    GL_CALL( glBindTexture(GL_TEXTURE_2D, 0) )
}
