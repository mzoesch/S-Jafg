// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Rhi/Texture2.h"

namespace Jafg
{

struct LRenderInfo;

struct LRenderTarget
{
    struct CreateInfo
    {
        LFrontend const& Frontend;
        rhi::extent2 Extent;
        vk::SampleCountFlagBits SampleCount;
        //# If set to eNone, then this #LRenderTarget will not resolve the msaa image.
        vk::ResolveModeFlagBits ResolveMode{ vk::ResolveModeFlagBits::eAverage };
        LLinearColor ClearColor{ LinearColors::Black };
        bool bDepthTest{};
    };

    NODISCARD FORCEINLINE constexpr bool IsInitialized() const noexcept { return this->Extent.width > 0; }

    ENGINE_API void Initialize(CreateInfo const& Info);
    ENGINE_API void Render(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info)> What) const;

    NODISCARD FORCEINLINE constexpr rhi::extent2 GetExtent() const noexcept { return this->Extent; }
    NODISCARD FORCEINLINE constexpr rhi::extent2 const& GetExtentAsLValue() const noexcept { return this->Extent; }

    NODISCARD FORCEINLINE constexpr bool IsDepthTested() const noexcept { return !!this->DepthImage.GetBuffer(); }
    NODISCARD FORCEINLINE constexpr auto const& GetDepthImage() const noexcept { return this->DepthImage; }
    NODISCARD FORCEINLINE constexpr auto const& GetDepthImageView() const noexcept { return this->DepthImageView; }

    NODISCARD FORCEINLINE constexpr auto const& GetMsaa() const noexcept { return this->MsaaTarget; }
    NODISCARD FORCEINLINE constexpr bool IsResolvingMsaa() const noexcept { return this->ResolveFlags != vk::ResolveModeFlagBits::eNone; }
    NODISCARD FORCEINLINE constexpr auto const& GetResolved() const noexcept { check(this->ResolvedTarget.has_value()) return *this->ResolvedTarget; }

    FORCEINLINE constexpr bool IsMsaaBindless() const noexcept { return this->MsaaTarget.BindlessIndex != INDEX_NONE; }
    FORCEINLINE constexpr u32 GetMsaaBindlessIndex() const noexcept
    {
        check(this->MsaaTarget.BindlessIndex >= std::numeric_limits<u32>::min() && this->MsaaTarget.BindlessIndex <= std::numeric_limits<u32>::max())
        return static_cast<u32>(this->MsaaTarget.BindlessIndex);
    }
    FORCEINLINE constexpr bool IsResolvedBindless() const noexcept { check(this->ResolvedTarget.has_value()) return this->ResolvedTarget->BindlessIndex != INDEX_NONE; }
    FORCEINLINE constexpr u32 GetResolvedBindlessIndex() const noexcept
    {
        check(this->ResolvedTarget.has_value())
        check(this->ResolvedTarget->BindlessIndex >= std::numeric_limits<u32>::min() && this->ResolvedTarget->BindlessIndex <= std::numeric_limits<u32>::max())
        return static_cast<u32>(this->ResolvedTarget->BindlessIndex);
    }

    //# Internal method. Do not use!!!
    FORCEINLINE constexpr void _SetMsaaBindlessIndex(i64 Value) const noexcept
    {
        this->MsaaTarget.BindlessIndex = Value;
    }
    FORCEINLINE constexpr void _SetResolvedBindlessIndex(i64 Value) const noexcept
    {
        check(this->ResolvedTarget.has_value())
        this->ResolvedTarget->BindlessIndex = Value;
    }

    vk::ClearValue ClearColor{vk::ClearColorValue{std::array<f32,4>{0.0f, 0.0f, 0.0f, 1.0f}}};

protected:

    rhi::extent2 Extent;

    LDeviceImage DepthImage;
    vk::raii::ImageView DepthImageView{ nullptr };

    struct Target
    {
        LDeviceImage Image;
        vk::raii::ImageView ImageView{ nullptr };
        mutable i64 BindlessIndex{ INDEX_NONE };
    };
    Target MsaaTarget;

    vk::ResolveModeFlagBits ResolveMode{ vk::ResolveModeFlagBits::eNone };
    vk::ResolveModeFlagBits ResolveFlags{ vk::ResolveModeFlagBits::eAverage };
    std::optional<Target> ResolvedTarget;
};

} /* ~Namespace Jafg */
