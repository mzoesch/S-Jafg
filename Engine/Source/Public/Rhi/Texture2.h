// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/VkAl.h"
#include "Serialization/BulkData.h"

namespace Jafg
{

//# A 2D extent structure compatible with the current underlying device API used.
template<typename T>
struct TExtent2D
{
    typedef T value_type;
    typedef VkExtent2D type;

    NODISCARD FORCEINLINE operator type&() noexcept { return *reinterpret_cast<type*>( this ); }
    NODISCARD FORCEINLINE operator type const&() const noexcept { return *reinterpret_cast<type const*>(this); }

    NODISCARD FORCEINLINE operator type*() noexcept { return reinterpret_cast<type*>(this); }
    NODISCARD FORCEINLINE operator type const*() const noexcept { return reinterpret_cast<type const*>(this); }

    NODISCARD FORCEINLINE auto operator<=>(TExtent2D const&) const = default;

    T Width{};
    T Height{};
};

typedef TExtent2D<u32> LTextureExtent;
static_assert(sizeof(LTextureExtent) == sizeof(VkExtent2D));
static_assert(std::is_standard_layout_v<LTextureExtent>);
static_assert(std::is_same_v<LTextureExtent::type, VkExtent2D>);

enum struct ETextureLoadFlagBits
{
    Default         = 0x00,
    Load            = 0x01 << 0,
    Stage           = 0x01 << 1,
    FlipY           = 0x01 << 2,
};
ENUM_STRUCT_FLAGS(ETextureLoadFlagBits, ETextureLoadFlags)

//# Represents a generic two-dimensional texture that can be uploaded to a device.
class LTexture2
{
public:

    struct LMetadata
    {
        vk::Format Format{ vk::Format::eUndefined };

        //# The desired number of mip levels. If not available, the optimal number will be used.
        TOptional<u32> DesiredMipLevels;

        vk::SampleCountFlagBits Samples{ vk::SampleCountFlagBits::e1 };
    };

    constexpr LTexture2() noexcept = default;
    explicit LTexture2(LPath Path, LMetadata Meta, ETextureLoadFlags Flags = ETextureLoadFlagBits::Default) noexcept
        : Path(std::move(Path)), Meta(Meta)
    {
        if (Flags & ETextureLoadFlagBits::Load)
        {
            this->AllocateFromDisk(Flags);
        }
        else
        {
            check( !(Flags & ETextureLoadFlagBits::Stage) && "Cannot stage a texture that is not loaded." )
        }

        return;
    }
    PROHIBIT_COPY(LTexture2)
    DEFAULT_MOVE(LTexture2)
    ~LTexture2() noexcept = default;

    //# Load the texture from secondary storage to main memory.
    inline     void AllocateFromDisk(LPath Path, ETextureLoadFlags Flags = ETextureLoadFlagBits::Default);
    ENGINE_API void AllocateFromDisk(ETextureLoadFlags Flags = ETextureLoadFlagBits::Default);

    //# Allocate an empty texture with the given extent and format.
    ENGINE_API void AllocateEmpty(LTextureExtent const& Extent, bool bZeroed);

    //# Stage the current texture to the device.
    ENGINE_API void StageToDevice();

    FORCEINLINE constexpr void Free_MainMemory() noexcept { if (this->MipMap0.IsAllocated()) { this->MipMap0.Free(); } }
    FORCEINLINE void Free_Device() noexcept { this->View.clear(); this->Handle.Free(); }
    FORCEINLINE void Free_v2() noexcept
    {
        this->Free_MainMemory();
        this->Free_Device();

        return;
    }

    FORCEINLINE constexpr auto GetExtent() const noexcept { return this->Extent; }
    FORCEINLINE constexpr auto GetWidth() const noexcept { return this->Extent.Width; }
    FORCEINLINE constexpr auto GetHeight() const noexcept { return this->Extent.Height; }

    FORCEINLINE constexpr auto const& GetMetadata() const noexcept { return this->Meta; }

    FORCEINLINE constexpr auto GetFormat() const noexcept { return this->Meta.Format; }
    FORCEINLINE constexpr auto GetChannelsPerPixel() const noexcept { return Vk_GetChannelsPerPixel(this->GetFormat()); }
    FORCEINLINE constexpr auto GetBytesPerPixel() const noexcept { return Vk_GetBytesPerPixel(this->GetFormat()); }
    FORCEINLINE constexpr bool HasDesiredMipLevels() const noexcept { return this->Meta.DesiredMipLevels.has_value(); }
    FORCEINLINE constexpr u32  GetDesiredMipLevels() const noexcept { return this->Meta.DesiredMipLevels.value(); }
    FORCEINLINE constexpr auto GetSamplesPerTexel() const noexcept { return this->Meta.Samples; }

    FORCEINLINE constexpr bool IsOnMainMemory() const noexcept { return this->MipMap0.IsAllocated(); }
    FORCEINLINE constexpr LByteBulkData const& GetFirstMipMap() const noexcept { return this->MipMap0; }

    FORCEINLINE bool IsOnDevice() const noexcept { return this->Handle.GetBuffer(); }
    FORCEINLINE constexpr auto const& GetDeviceHandle() const noexcept { return this->Handle; }

    FORCEINLINE bool HasImageView() const noexcept { return static_cast<bool>(*this->View); }
    FORCEINLINE constexpr auto const& GetImageView() const noexcept { return this->View; }

private:

    LPath Path;
    LTextureExtent Extent;
    LMetadata Meta;
    LByteBulkData MipMap0;
    LDeviceImage Handle;
    vk::raii::ImageView View{ nullptr };
};

inline void LTexture2::AllocateFromDisk(LPath Path, ETextureLoadFlags Flags)
{
    this->Path = std::move(Path);
    this->AllocateFromDisk(Flags);

    return;
}

} /* ~Namespace Jafg */
