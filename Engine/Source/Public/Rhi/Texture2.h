// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/VkAl.h"
#include "Serialization/BulkData.h"
#include "Rhi/ResourceReference.h"

namespace Jafg
{

//# A two-dimensional extent structure compatible with the current underlying device API used.
template<typename T>
struct TExtent2
{
    typedef T value_type;
    typedef VkExtent2D type;

    NODISCARD FORCEINLINE operator type&() noexcept { return *reinterpret_cast<type*>( this ); }
    NODISCARD FORCEINLINE operator type const&() const noexcept { return *reinterpret_cast<type const*>(this); }

    NODISCARD FORCEINLINE operator type*() noexcept { return reinterpret_cast<type*>(this); }
    NODISCARD FORCEINLINE operator type const*() const noexcept { return reinterpret_cast<type const*>(this); }

    NODISCARD FORCEINLINE auto operator<=>(TExtent2 const&) const = default;

    T Width{};
    T Height{};
};
typedef TExtent2<u32> LTexture2Extent;
static_assert(sizeof(LTexture2Extent) == sizeof(VkExtent2D));
static_assert(std::is_standard_layout_v<LTexture2Extent>);
static_assert(std::is_same_v<LTexture2Extent::type, VkExtent2D>);

typedef EResourceStateBits ETexture2StateBits;
typedef EResourceState ETexture2State;

//# Represents a generic two-dimensional texture that can be uploaded to a device.
struct LTexture2 final
{
    struct HostInfo
    {
        vk::Format Format{ vk::Format::eUndefined };
    };
    struct DeviceInfo
    {
        //# The desired number of mip levels. If not available, the optimal number will be used.
        TOptional<u32> DesiredMipLevels;
        //# The allowed sample numbers. The highest available will be used unless greater than the max sample count of the device.
        vk::SampleCountFlags Samples{ vk::SampleCountFlagBits::e1 };
    };

    enum struct EResult
    {
        Success,
        FileNotFound,
        LoadingError,
    };

    constexpr LTexture2() noexcept = default;
    explicit LTexture2(LPath Path, HostInfo HostCreateInfo, DeviceInfo DeviceCreateInfo, ETexture2State State = ETexture2StateBits::None) noexcept
        : Path(std::move(Path))
    {
        if (State & ETexture2StateBits::Host || State & ETexture2StateBits::Device)
        {
            auto Result{this->LoadToHost(HostCreateInfo)};
            jassert(Result == EResult::Success)
        }

        if (State & ETexture2StateBits::Device)
        {
            this->LoadToDevice(DeviceCreateInfo);
            if ((State & ETexture2StateBits::Host) == ETexture2StateBits::None)
            {
                this->FreeFromHost();
            }
        }

        return;
    }
    PROHIBIT_REALLOC_OF_ANY_FORM(LTexture2)
    ~LTexture2() = default;

    FORCEINLINE constexpr bool IsOnHost() const noexcept { return this->MipMap0.IsAllocated(); }
    ENGINE_API EResult LoadToHost(HostInfo const& Info);
    inline void FreeFromHost() noexcept
    {
        if (this->MipMap0.IsAllocated())
        {
            this->MipMap0.Free();
        }
    }

    FORCEINLINE constexpr bool IsOnDevice() const noexcept { return this->Handle.GetBuffer(); }
    ENGINE_API void LoadToDevice(DeviceInfo const& Info);
    inline void FreeFromDevice() noexcept
    {
        this->View.clear();
        this->Handle.Free();
    }

    FORCEINLINE constexpr LPath const& GetPath() const noexcept { return this->Path; }

    FORCEINLINE constexpr auto const& GetMetadata() const noexcept { return this->Meta; }
    FORCEINLINE constexpr auto const& GetExtent() const noexcept { return this->Meta.Extent; }
    FORCEINLINE constexpr auto GetWidth() const noexcept { return this->Meta.Extent.Width; }
    FORCEINLINE constexpr auto GetHeight() const noexcept { return this->Meta.Extent.Height; }
    FORCEINLINE constexpr auto GetFormat() const noexcept { return this->Meta.Format; }
    FORCEINLINE constexpr auto GetChannelsPerPixel() const noexcept { return Vk_GetChannelsPerPixel(this->GetFormat()); }
    FORCEINLINE constexpr auto GetBytesPerPixel() const noexcept { return Vk_GetBytesPerPixel(this->GetFormat()); }
    FORCEINLINE constexpr u32  GetMipLevels() const noexcept { return this->Meta.MipLevels; }
    FORCEINLINE constexpr auto GetSamplesPerTexel() const noexcept { return this->Meta.Samples; }

    FORCEINLINE constexpr LByteBulkData const& GetFirstMipMap() const noexcept { return this->MipMap0; }

    FORCEINLINE constexpr LDeviceImage const& GetDeviceHandle() const noexcept { return this->Handle; }
    FORCEINLINE bool HasImageView() const noexcept { return static_cast<bool>(*this->View); }
    FORCEINLINE constexpr auto const& GetImageView() const noexcept { return this->View; }

private:

    struct Metadata
    {
        LTexture2Extent Extent;
        vk::Format Format{ vk::Format::eUndefined };
        u32 MipLevels{ std::numeric_limits<u32>::max() };
        vk::SampleCountFlagBits Samples{ vk::SampleCountFlagBits::e1 };
    };

    LPath Path;
    Metadata Meta;
    LByteBulkData MipMap0;
    LDeviceImage Handle;
    vk::raii::ImageView View{ nullptr };
};

typedef TResourceReference<LTexture2> LTexture2Ref;

} /* ~Namespace Jafg */
