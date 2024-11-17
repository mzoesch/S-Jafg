// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Rhi/Texture2.h"
#include "System/EnginePath.h"
#include "System/Paths.h"
#include <stb_image.h>

bool Jafg::LTexture2::CreateEmpty(const uint32 InWidth, const uint32 InHeight, const ERawImageFormat::Type InFormat)
{
    check( this->MipMap.Bulk.IsAllocated() == false )

    this->MipMap.Size.X = InWidth;
    this->MipMap.Size.Y = InHeight;
    this->MipMap.Format = InFormat;
    this->MipMap.AllocateBulk();

    return true;
}

bool Jafg::LTexture2::LoadFromDisk(const LEnginePath& Path, const JUserPreferences& UserPreferences)
{
    check( this->MipMap.Bulk.IsAllocated() == false )

    const LPath AbsolutePath = Path.ResolveAbsolutePath(UserPreferences);
    Paths::DoesFileExistPanicked(AbsolutePath);

    int32 Width      = 0;
    int32 Height     = 0;
    int32 NrChannels = 0;
    ::stbi_set_flip_vertically_on_load(false);
    uint8* Data      = ::stbi_load(AbsolutePath.GetPath().ToC(), &Width, &Height, &NrChannels, 4);

    jassert( Data )
    jassert( NrChannels == 4 )
    jassert( Width > 0 && Height > 0 )
    jassert( Width == 16 ) // Currently only supporting 16x16 textures. But in the future we ofc will support more.
    jassert( Height == 16 )

    this->MipMap.Size.X = Width;
    this->MipMap.Size.Y = Height;
    this->MipMap.Format = ERawImageFormat::BGRA8;
    this->MipMap.LoadFromBuffer(Data, 0);

    ::stbi_image_free(Data);

    return true;
}

void Jafg::LTexture2::CopyTexture(const LTexture2& InTexture, const LPoint& InPoint /* = LPoint::Zero()*/, const bool bKeepCurrentTexture /*= false*/)
{
    if (bKeepCurrentTexture == false)
    {
        this->Free();
        this->CreateEmpty(InTexture.MipMap.Size.X, InTexture.MipMap.Size.Y, InTexture.MipMap.Format);
    }

    check( this->MipMap.Format == InTexture.MipMap.Format && this->MipMap.Format != ERawImageFormat::Unspecified )
    check( this->MipMap.Bulk.IsAllocated() )
    check( this->MipMap.Size.X >= InTexture.MipMap.Size.X )
    check( this->MipMap.Size.Y >= InTexture.MipMap.Size.Y )
    check( InPoint.X + InTexture.MipMap.Size.X <= this->MipMap.Size.X )
    check( InPoint.Y + InTexture.MipMap.Size.Y <= this->MipMap.Size.Y )

    uint32 CurrentHeightCursor = InPoint.Y;
    while (CurrentHeightCursor - InPoint.Y < InTexture.GetHeight())
    {
        uint8* CurrentTextureDestination = this->MipMap.Bulk.GetBulk();
        CurrentTextureDestination += static_cast<LuPtrSize>((CurrentHeightCursor * this->GetWidth() + InPoint.X) * this->GetBytesPerPixel());
        checkSlow( CurrentTextureDestination >= this->MipMap.Bulk.GetBulk() )
        checkSlow( CurrentTextureDestination < this->MipMap.Bulk.GetBulk() + this->MipMap.Bulk.GetByteSize() )

        const uint8* OtherTextureSource = InTexture.MipMap.Bulk.GetBulk();
        OtherTextureSource += static_cast<LuPtrSize>((CurrentHeightCursor - InPoint.Y) * InTexture.GetWidth() * InTexture.GetBytesPerPixel());
        checkSlow( OtherTextureSource >= InTexture.MipMap.Bulk.GetBulk() )
        checkSlow( OtherTextureSource < InTexture.MipMap.Bulk.GetBulk() + InTexture.MipMap.Bulk.GetByteSize() )

        ::memcpy(CurrentTextureDestination, OtherTextureSource, InTexture.GetWidth() * static_cast<LuPtrSize>(InTexture.GetBytesPerPixel()));

        ++CurrentHeightCursor;

        continue;
    }

    return;
}
