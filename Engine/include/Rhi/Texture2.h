// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Rhi/ResourceReference.h"
#include "Rhi/DeviceBuffers.h"

namespace Jafg
{

struct LTexture2;
typedef rhi::resource_state_bits ETexture2StateBits;
typedef rhi::resource_state_flags ETexture2State;

typedef rhi::shared_ref<LTexture2> LTexture2Ref;

//# Represents a generic two-dimensional texture that can be uploaded to a device.
struct LTexture2 final
{
    struct HostInfo final
    {
        vk::Format Format{ vk::Format::eUndefined };
    };
    struct DeviceInfo final
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
    static LStringView ResultToString(EResult Result) noexcept
    {
        switch (Result)
        {
        case EResult::Success: return "Success";
        case EResult::FileNotFound: return "FileNotFound";
        case EResult::LoadingError: return "LoadingError";
        }
        std::unreachable();
    }

    LTexture2() noexcept = default;
    explicit LTexture2(LPath Path, HostInfo HostCreateInfo, DeviceInfo DeviceCreateInfo) noexcept
        : Path{std::move(Path)}, HostCreateInfo{std::move(HostCreateInfo)}, DeviceCreateInfo{std::move(DeviceCreateInfo)}
    {
    }
    static LTexture2 FromMemory(
          algo::byte_bulk&& Data
        , vk::Format SrcFormat
        , rhi::extent2 Extent
        , HostInfo HostCreateInfo
        , DeviceInfo DeviceCreateInfo
        , ETexture2State State = ETexture2StateBits::Host
        );
    //# @see JTextureSubsystem::FromFile
    static LTexture2Ref FromFile(LPluginInfo Namespace, LStringView Path
        , HostInfo HostCreateInfo
        , DeviceInfo DeviceCreateInfo
        , ETexture2State State = ETexture2StateBits::Device
        );
    //# @see JTextureSubsystem::FromTextureView
    static LTexture2Ref FromTextureView(LPluginInfo Namespace, LStringView Name
        , ETexture2State State = ETexture2StateBits::Device
        );
    PROHIBIT_COPY(LTexture2)
    LTexture2(LTexture2&& Other) = default;
    LTexture2& operator=(LTexture2&& Rhs) = default;
    ~LTexture2() = default;

    ENGINE_API void BringToState(ETexture2State State);

    FORCEINLINE constexpr bool IsOnHost() const noexcept { return this->MipMap0.allocated(); }
    ENGINE_API EResult LoadToHost();
    inline void FreeFromHost() noexcept
    {
        if (this->MipMap0.allocated())
        {
            this->MipMap0.free();
        }
    }

    FORCEINLINE bool IsOnDevice() const noexcept { return !!*this->Handle; }
    ENGINE_API void LoadToDevice();
    inline void FreeFromDevice() noexcept
    {
        this->View.clear();
        this->Handle.free();
    }

    FORCEINLINE constexpr LPath const& GetPath() const noexcept { return this->Path; }
    FORCEINLINE constexpr HostInfo const& GetHostCreateInfo() const noexcept { return this->HostCreateInfo; }
    FORCEINLINE constexpr DeviceInfo const& GetDeviceCreateInfo() const noexcept { return this->DeviceCreateInfo; }

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

    FORCEINLINE constexpr algo::byte_bulk const& GetFirstMipMap() const noexcept { return this->MipMap0; }

    FORCEINLINE constexpr rhi::device_image const& GetDeviceHandle() const noexcept { return this->Handle; }
    FORCEINLINE bool HasImageView() const noexcept { return static_cast<bool>(*this->View); }
    FORCEINLINE constexpr auto const& GetImageView() const noexcept { return this->View; }

    NODISCARD FORCEINLINE constexpr bool IsBindless() const noexcept { return this->BindlessIndex != rhi::invalid_resource_index; }
    NODISCARD FORCEINLINE constexpr rhi::resource_index_t GetBindlessIndex() const noexcept
    {
        check(this->BindlessIndex != rhi::invalid_resource_index)
        return this->BindlessIndex;
    }
    //# Internal method. Do not use!!!
    FORCEINLINE constexpr void _SetBindlessIndex(u32 Value) noexcept { this->BindlessIndex = Value; }

private:

    struct Metadata final
    {
        rhi::extent2 Extent;
        vk::Format Format{ vk::Format::eUndefined };
        u32 MipLevels{ std::numeric_limits<u32>::max() };
        vk::SampleCountFlagBits Samples{ vk::SampleCountFlagBits::e1 };
    };

    LPath Path;
    HostInfo HostCreateInfo;
    DeviceInfo DeviceCreateInfo;
    Metadata Meta;
    rhi::resource_index_t BindlessIndex{ rhi::invalid_resource_index };
    algo::byte_bulk MipMap0;
    rhi::device_image Handle;
    vk::raii::ImageView View{ nullptr };
};

//#
//# - std::monostate:         No texture.
//# - LTexture2Ref:           The texture will be used (has to be valid).
//# - <LPluginInfo,LString>:  Texture view namespace+name (has to be valid).
//#
struct LOptionalTexture2Ref
{
    LOptionalTexture2Ref() noexcept
        : Variant{std::monostate{}} {}
    LOptionalTexture2Ref(std::monostate) noexcept
        : Variant{std::monostate{}} {}
    LOptionalTexture2Ref(LTexture2Ref Reference) noexcept
        : Variant{Reference} {}
    LOptionalTexture2Ref(std::pair<LPluginInfo, LString> Pair) noexcept
        : Variant{std::move(Pair)} {}
    LOptionalTexture2Ref(LPluginInfo Namespace, LString Name) noexcept
        : Variant{std::pair<LPluginInfo, LString>{Namespace, std::move(Name)}} {}

    std::variant<std::monostate, LTexture2Ref, std::pair<LPluginInfo, LString>> Variant;

    inline LTexture2Ref GetResolved() const
    {
        if (std::holds_alternative<LTexture2Ref>(this->Variant))
        {
            return std::get<LTexture2Ref>(this->Variant);
        }
        if (std::holds_alternative<std::pair<LPluginInfo, LString>>(this->Variant))
        {
            auto& Pair{std::get<std::pair<LPluginInfo, LString>>(this->Variant)};
            return LTexture2::FromTextureView(Pair.first, Pair.second);
        }
        return {};
    }
    inline LTexture2Ref Resolve()
    {
        this->Variant = this->GetResolved();
        if (!std::get<LTexture2Ref>(this->Variant).get())
        {
            LOG_FATAL(LogTextureSubsystem, "Failed to resolve texture.")
        }
        return std::get<LTexture2Ref>(this->Variant);
    }
};

} /* ~Namespace Jafg */
