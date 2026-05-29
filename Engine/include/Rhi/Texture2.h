// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Serialization/BulkData.h"
#include "Rhi/ResourceReference.h"
#include "Rhi/DeviceBuffers.h"

namespace Jafg
{

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
        std::optional<u32> DesiredMipLevels;
        //# The allowed sample numbers. The highest available will be used unless greater than the max sample count of the device.
        std::optional<vk::SampleCountFlags> Samples;
    };

    enum struct EResult
    {
        Success,
        FileNotFound,
        LoadingError,
    };
    inline static LStringView ResultToString(EResult Result) noexcept
    {
        switch (Result)
        {
        case EResult::Success: return "Success";
        case EResult::FileNotFound: return "FileNotFound";
        case EResult::LoadingError: return "LoadingError";
        default: return "Unknown";
        }
    }

    constexpr LTexture2() noexcept = default;
    explicit LTexture2(LPath Path, HostInfo HostCreateInfo, DeviceInfo DeviceCreateInfo, ETexture2State State = ETexture2StateBits::None) noexcept
        : Path(std::move(Path))
    {
        if (State & ETexture2StateBits::Host || State & ETexture2StateBits::Device)
        {
            if (auto Result{this->LoadToHost(HostCreateInfo)}; Result != EResult::Success)
            {
                LOG_FATAL(LogRhi, "[{}]: Failed to load texture to host; Reason: [{}].", this->Path, LTexture2::ResultToString(Result))
            }
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
    static TSharedRef<LTexture2> FromMemory(LStringView HumanReadableName, LByteBulkData&& Data, vk::Format SrcFormat, rhi::extent2 Extent, HostInfo Info);
    static TSharedRef<LTexture2> FromTextureView(LStringView View);
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

    FORCEINLINE bool IsOnDevice() const noexcept { return this->Handle.GetBuffer(); }
    ENGINE_API void LoadToDevice(DeviceInfo const& Info);
    inline void FreeFromDevice() noexcept
    {
        this->View.clear();
        this->Handle.Free();
    }

    FORCEINLINE constexpr LPath const& GetPath() const noexcept { return this->Path; }

    FORCEINLINE constexpr auto const& GetMetadata() const noexcept { return this->Meta; }
    FORCEINLINE constexpr auto const& GetExtent() const noexcept { return this->Meta.Extent; }
    FORCEINLINE constexpr LVec2F GetExtentAsVec2F() const noexcept { return LVec2F{this->Meta.Extent.width, this->Meta.Extent.height}; }
    FORCEINLINE constexpr auto GetWidth() const noexcept { return this->Meta.Extent.width; }
    FORCEINLINE constexpr auto GetHeight() const noexcept { return this->Meta.Extent.height; }
    FORCEINLINE constexpr auto GetFormat() const noexcept { return this->Meta.Format; }
    FORCEINLINE constexpr auto GetChannelsPerPixel() const noexcept { return rhi::vk_channels_per_pixel(this->GetFormat()); }
    FORCEINLINE constexpr auto GetBytesPerPixel() const noexcept { return rhi::vk_bytes_per_pixel(this->GetFormat()); }
    FORCEINLINE constexpr u32  GetMipLevels() const noexcept { return this->Meta.MipLevels; }
    FORCEINLINE constexpr auto GetSamplesPerTexel() const noexcept { return this->Meta.Samples; }

    FORCEINLINE constexpr LByteBulkData const& GetFirstMipMap() const noexcept { return this->MipMap0; }

    FORCEINLINE constexpr LDeviceImage const& GetDeviceHandle() const noexcept { return this->Handle; }
    FORCEINLINE bool HasImageView() const noexcept { return static_cast<bool>(*this->View); }
    FORCEINLINE constexpr auto const& GetImageView() const noexcept { return this->View; }

    FORCEINLINE constexpr bool IsBindless() const noexcept { return this->BindlessIndex != INDEX_NONE; }
    FORCEINLINE constexpr u32 GetBindlessIndex() const noexcept
    {
        check(this->BindlessIndex >= std::numeric_limits<u32>::min() && this->BindlessIndex <= std::numeric_limits<u32>::max())
        return static_cast<u32>(this->BindlessIndex);
    }
    //# Internal method. Do not use!!!
    FORCEINLINE constexpr void _SetBindlessIndex(i64 Value) noexcept { this->BindlessIndex = Value; }

private:

    struct Metadata
    {
        rhi::extent2 Extent;
        vk::Format Format{ vk::Format::eUndefined };
        u32 MipLevels{ std::numeric_limits<u32>::max() };
        vk::SampleCountFlagBits Samples{ vk::SampleCountFlagBits::e1 };
    };

    LPath Path;
    Metadata Meta;
    LByteBulkData MipMap0;
    LDeviceImage Handle;
    vk::raii::ImageView View{ nullptr };
    i64 BindlessIndex{ INDEX_NONE };
};

typedef TSharedRef<LTexture2> LTexture2Ref;

//#
//# - std::monostate: No texture.
//# - LTexture2Ref: The texture will be used (has to be valid).
//# - LString: Texture view identifier (has to be valid).
//#
struct LOptionalTexture2Ref
{
    LOptionalTexture2Ref() noexcept : Variant{std::monostate{}} {}
    LOptionalTexture2Ref(std::monostate) noexcept : Variant{std::monostate{}} {}
    LOptionalTexture2Ref(LTexture2Ref Reference) noexcept : Variant{Reference} {}
    LOptionalTexture2Ref(char const* String) noexcept : Variant{String} {}
    LOptionalTexture2Ref(LString String) noexcept : Variant{std::move(String)} {}

    std::variant<std::monostate, LTexture2Ref, LString> Variant;

    inline LTexture2Ref GetResolved() const
    {
        if (std::holds_alternative<LTexture2Ref>(this->Variant))
        {
            return std::get<LTexture2Ref>(this->Variant);
        }
        if (std::holds_alternative<LString>(this->Variant))
        {
            return LTexture2::FromTextureView(std::get<LString>(this->Variant));
        }
        return {};
    }
    inline LTexture2Ref Resolve()
    {
        this->Variant = this->GetResolved();
        check(std::holds_alternative<LTexture2Ref>(this->Variant))
        if (!std::get<LTexture2Ref>(this->Variant).get())
        {
            this->Variant = std::monostate{};
            return {};
        }
        return std::get<LTexture2Ref>(this->Variant);
    }
};

} /* ~Namespace Jafg */
