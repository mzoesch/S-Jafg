// Copyright mzoesch. All rights reserved.

#include "Rhi/Texture2Handle.h"
#include "Rhi/Texture2.h"
#include "Rhi/RhiVendorInclude.h"

Jafg::LTexture2Handle::LTexture2Handle(const LTexture2& InTexture)
{
    this->Upload(InTexture);
    return;
}

void Jafg::LTexture2Handle::Upload(const LTexture2& InTexture)
{
    check( InTexture.GetWidth() > 0 && InTexture.GetHeight() > 0 )
    check( this->Handle.IsSet() == false )

    check( InTexture.GetFormat() == ERawImageFormat::BGRA8 )

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    this->Handle.MakeMeaningful();

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, this->Handle.GetValuePtr());
    glBindTexture(GL_TEXTURE_2D, *this->Handle);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Or GL_LINEAR_MIPMAP_LINEAR if mipmaps
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D
    (
        GL_TEXTURE_2D, 0, GL_RGBA /* out */,
        static_cast<GLsizei>(InTexture.GetFirstMipMap().GetWidth()),
        static_cast<GLsizei>(InTexture.GetFirstMipMap().GetHeight()),
        0, GL_RGBA /* in */, GL_UNSIGNED_BYTE, InTexture.GetFirstMipMap().GetBulk().GetRawBulk()
    );

    // glGenerateMipmap(GL_TEXTURE_2D);

    return;
}

void Jafg::LTexture2Handle::Shred()
{
    if (this->IsValid())
    {
        LOG_VERBOSE(LogTextureSubsystem, "Shredding texture [{}].", *this->Handle)

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, *this->Handle);
        glDeleteTextures(1, this->Handle.GetValuePtr());
        this->Handle.Reset();
    }

    return;
}
