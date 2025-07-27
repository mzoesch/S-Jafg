// Copyright mzoesch. All rights reserved.

#include "Rhi/Texture2.h"
#include "System/Paths.h"
#include "System/EnginePath.h"
#include "Rhi/RhiVendorInclude.h"

Smart::TUnique<u8[]> Jafg::Texture2::RotateCW(const u8* InData, const u32 InWidth, const u32 InHeight, const u32 InChannels)
{
    check( InData )

    const u32 NewW { InHeight };
    const u32 NewH { InWidth  };

    Smart::TUnique<u8[]> Buffer { new u8[InWidth * InHeight * InChannels] };

    for (u32 Y { 0 }; Y < InHeight; ++Y)
    {
        for (u32 X { 0 }; X < InWidth; ++X)
        {
            for (u32 C { 0 }; C < InChannels; ++C)
            {
                const u32 srcIndex { (Y * InWidth + X) * InChannels + C };
                const u32 dstX { InHeight - 1 - Y };
                const u32 dstY { X };
                const u32 dstIndex { (dstY * NewW + dstX) * InChannels + C };
                Buffer[dstIndex] = InData[srcIndex];

                continue;
            }

            continue;
        }

        continue;
    }

    return Buffer;
}

Smart::TUnique<u8[]> Jafg::Texture2::RotateCCW(const u8* InData, const u32 InWidth, const u32 InHeight, const u32 InChannels)
{
    check( InData )

    const u32 NewW { InHeight };
    const u32 NewH { InWidth  };

    Smart::TUnique<u8[]> Buffer { new u8[InWidth * InHeight * InChannels] };

    for (u32 Y { 0 }; Y < InHeight; ++Y)
    {
        for (u32 X { 0 }; X < InWidth; ++X)
        {
            for (u32 C { 0 }; C < InChannels; ++C)
            {
                const u32 srcIndex { (Y * InWidth + X) * InChannels + C };
                const u32 dstX { Y };
                const u32 dstY { InWidth - 1 - X };
                const u32 dstIndex { (dstY * NewW + dstX) * InChannels + C };
                Buffer[dstIndex] = InData[srcIndex];

                continue;
            }

            continue;
        }

        continue;
    }

    return Buffer;
}

bool Jafg::LTexture2::CreateEmpty(const u32 InWidth, const u32 InHeight, const ERawImageFormat::Type InFormat)
{
    check( this->MipMap.GetBulk().IsAllocated() == false )

    this->MipMap.Size.X = InWidth;
    this->MipMap.Size.Y = InHeight;
    this->MipMap.Format = InFormat;
    this->MipMap.AllocateBulk();

    return true;
}

bool Jafg::LTexture2::LoadFromDisk(const LPath& Path, const ERawImageFormat::Type InFormat)
{
    if (Paths::DoesFileExist(Path) == false)
    {
        return false;
    }

    LOG_TRACE(LogSystem, "Loading texture2 [{}].", Path)
    check( this->MipMap.GetBulk().IsAllocated() == false )

    TArray<u8> Bin = Paths::ReadFileAsBinary(Path);

    i32 Width      = 0;
    i32 Height     = 0;
    i32 NrChannels = 0;

    ::stbi_set_flip_vertically_on_load(false);
    u8* Data = ::stbi_load_from_memory(Bin.GetDataPointer(), static_cast<int>(Bin.GetSize()), &Width, &Height, &NrChannels, ERawImageFormat::GetChannelsPerPixel(InFormat));

    if (stbi_failure_reason())
    {
        LOG_ERROR(LogSystem, "Failed to load texture [{}] from disk. Reason: [{}].", Path, stbi_failure_reason())
        check( Data == nullptr )
        return false;
    }

    if (NrChannels != ERawImageFormat::GetChannelsPerPixel(InFormat))
    {
        LOG_VERBOSE
        (
            LogRhi,
            "Texture2 [{}] was loaded from disk with [{}] channels to memory with [{}] channels.",
            Path,
            NrChannels,
            ERawImageFormat::GetChannelsPerPixel(InFormat)
        )
    }

    jassert( Data )
    jassert( Width > 0 && Height > 0 )

    this->MipMap.Size.X = Width;
    this->MipMap.Size.Y = Height;
    this->MipMap.Format = InFormat;
    this->MipMap.LoadFromBuffer(Data, 0);

    ::stbi_image_free(Data);

    return true;
}

bool Jafg::LTexture2::LoadFromDisk(const LEnginePath& Path, const ERawImageFormat::Type InFormat)
{
    LOG_TRACE(LogSystem, "Loading texture2 [{}].", Path.GetRelativeUnresolvedPath())

    check( this->MipMap.GetBulk().IsAllocated() == false )

    i32 Width      = 0;
    i32 Height     = 0;
    i32 NrChannels = 0;

    Finder::DoesFileExistsAsserted(Path);
    const u8* Bulk     = nullptr;
    u64       BulkSize = 0;
    Finder::ReadFileAsBinary(Path, &Bulk, &BulkSize);

    ::stbi_set_flip_vertically_on_load(false);
    u8* Data = ::stbi_load_from_memory(Bulk, static_cast<int>(BulkSize), &Width, &Height, &NrChannels, ERawImageFormat::GetChannelsPerPixel(InFormat));

    if (stbi_failure_reason())
    {
        LOG_ERROR(LogRhi, "Failed to load texture [{}] from disk. Reason: [{}].", Path.GetRelativeUnresolvedPath(), stbi_failure_reason())
        check( Data == nullptr )
        return false;
    }

    if (NrChannels != ERawImageFormat::GetChannelsPerPixel(InFormat))
    {
        LOG_VERBOSE
        (
            LogRhi,
            "Texture2 [{}] was loaded from disk with [{}] channels to memory with [{}] channels.",
            Path.GetRelativeUnresolvedPath(),
            NrChannels,
            ERawImageFormat::GetChannelsPerPixel(InFormat)
        )
    }

    jassert( Data )
    jassert( Width > 0 && Height > 0 )

    this->MipMap.Size.X = Width;
    this->MipMap.Size.Y = Height;
    this->MipMap.Format = InFormat;

    this->MipMap.LoadFromBuffer(Data, 0);

    ::stbi_image_free(Data);
    Finder::FreeReadFileBinaryBuffer(&Bulk);

    return true;
}

void Jafg::LTexture2::Free()
{
    this->Handle.Shred();
    this->MipMap.FreeBulk();

    return;
}

void Jafg::LTexture2::CopyTexture(const LTexture2& InTexture, const LPoint& InPoint /* = LPoint::Zero()*/, const bool bKeepCurrentTexture /*= false*/)
{
    if (bKeepCurrentTexture == false)
    {
        this->Free();
        this->CreateEmpty(InTexture.MipMap.GetWidth(), InTexture.MipMap.GetHeight(), InTexture.MipMap.GetFormat());
    }

    check( this->MipMap.GetFormat() == InTexture.MipMap.GetFormat() && this->MipMap.GetFormat() != ERawImageFormat::Unspecified )
    check( this->MipMap.GetBulk().IsAllocated() )
    check( this->MipMap.GetWidth() >= InTexture.MipMap.GetWidth() )
    check( this->MipMap.GetHeight() >= InTexture.MipMap.GetHeight() )
    check( InPoint.X + InTexture.MipMap.GetWidth() <= this->MipMap.GetWidth() )
    check( InPoint.Y + InTexture.MipMap.GetHeight() <= this->MipMap.GetHeight() )

    u32 CurrentHeightCursor = InPoint.Y;
    while (CurrentHeightCursor - InPoint.Y < InTexture.GetHeight())
    {
        u8* CurrentTextureDestination = this->MipMap.Bulk.GetRawBulk();
        CurrentTextureDestination += static_cast<LSize>((CurrentHeightCursor * this->GetWidth() + InPoint.X) * this->GetBytesPerPixel());
        checkSlow( CurrentTextureDestination >= this->MipMap.GetBulk().GetRawBulk() )
        checkSlow( CurrentTextureDestination < this->MipMap.GetBulk().GetRawBulk() + this->MipMap.GetBulk().GetByteSize() )

        const u8* OtherTextureSource = InTexture.MipMap.GetBulk().GetRawBulk();
        OtherTextureSource += static_cast<LSize>((CurrentHeightCursor - InPoint.Y) * InTexture.GetWidth() * InTexture.GetBytesPerPixel());
        checkSlow( OtherTextureSource >= InTexture.MipMap.GetBulk().GetRawBulk() )
        checkSlow( OtherTextureSource < InTexture.MipMap.GetBulk().GetRawBulk() + InTexture.MipMap.GetBulk().GetByteSize() )

        ::memcpy(CurrentTextureDestination, OtherTextureSource, InTexture.GetWidth() * static_cast<LSize>(InTexture.GetBytesPerPixel()));

        ++CurrentHeightCursor;

        continue;
    }

    return;
}
