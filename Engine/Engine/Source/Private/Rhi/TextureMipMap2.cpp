// Copyright mzoesch. All rights reserved.

#include "Rhi/TextureMipMap2.h"

Jafg::LTextureMipMap2::LTextureMipMap2(LTextureMipMap2&& InOther) noexcept
{
    this->Size   = InOther.Size;
    this->Bulk   = std::move(InOther.Bulk);
    this->Format = InOther.Format;

    InOther.Size   = LSize::Zero();
    check( InOther.Bulk.IsAllocated() == false )
    InOther.Format = ERawImageFormat::Unspecified;

    return;
}

Jafg::LTextureMipMap2& Jafg::LTextureMipMap2::operator=(LTextureMipMap2&& InOther) noexcept
{
    if (this != &InOther)
    {
        this->Size   = InOther.Size;
        this->Bulk   = std::move(InOther.Bulk);
        this->Format = InOther.Format;

        InOther.Size   = LSize::Zero();
        check( InOther.Bulk.IsAllocated() == false )
        InOther.Format = ERawImageFormat::Unspecified;
    }

    return *this;
}

void Jafg::LTextureMipMap2::FreeBulk()
{
    if (this->Bulk.IsAllocated() == false)
    {
        return;
    }

    this->Bulk.FreeBulk();

    return;
}

void Jafg::LTextureMipMap2::AllocateBulk()
{
    check( this->Bulk.IsAllocated() == false )
    check( this->Size.X > 0 && this->Size.Y > 0 )
    check( this->Format != ERawImageFormat::Unspecified )

    const i32 BytesPerPixel = ERawImageFormat::GetBytesPerPixel(this->Format);
    this->Bulk.AllocateBulk(static_cast<i32>(this->Size.X * this->Size.Y) * BytesPerPixel, true);

    check( this->Bulk.IsAllocated() )

    return;
}

void Jafg::LTextureMipMap2::LoadFromBuffer(const u8* Buffer, const i32 Offset)
{
    check( this->Bulk.IsAllocated() == false )
    check( this->Size.X > 0 && this->Size.Y > 0 )
    check( this->Format != ERawImageFormat::Unspecified )
    check( Buffer != nullptr )

    const i32 BytesPerPixel = ERawImageFormat::GetBytesPerPixel(this->Format);
    this->Bulk.Serialize(Buffer, static_cast<i32>(this->Size.X * this->Size.Y) * BytesPerPixel, Offset);

    check( this->Bulk.IsAllocated() )

    return;
}
