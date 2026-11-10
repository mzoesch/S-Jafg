// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/RendererCore.h"
#include "Rhi/Texture2.h"
#include "Rhi/AmbientOcclusion.h"

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
        bool bAllowSelection{};
        bool bAmbientOcclusion{};
    };

    NODISCARD FORCEINLINE constexpr bool IsInitialized() const noexcept { return this->Extent.width > 0; }

    ENGINE_API void Initialize(CreateInfo const& Info);
    ENGINE_API void RenderSelected(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info)> What) const;
    ENGINE_API void Render(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info)> What) const;

    //#
    //# The passes of the screen space ambient occlusion. They have to run in this order and before #Render:
    //# 1. The linear view space depth of the scene (single sampled, depth tested).
    //# 2. The noisy occlusion from that depth.
    //# 3. The blurred occlusion, which is what the scene samples during #Render.
    //#
    ENGINE_API void RenderSceneDepth(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info)> What) const;
    ENGINE_API void RenderAmbientOcclusion(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info)> What) const;
    ENGINE_API void RenderAmbientOcclusionBlur(LRenderInfo const& Info, TFunction2<void(LRenderInfo const& Info)> What) const;

    NODISCARD FORCEINLINE constexpr rhi::extent2 GetExtent() const noexcept { return this->Extent; }
    NODISCARD FORCEINLINE constexpr rhi::extent2 const& GetExtentAsLValue() const noexcept { return this->Extent; }

    NODISCARD FORCEINLINE constexpr bool IsDepthTested() const noexcept { return !!*this->DepthTarget.Image; }
    NODISCARD FORCEINLINE constexpr auto const& GetDepthImage() const noexcept { return this->DepthTarget.Image; }
    NODISCARD FORCEINLINE constexpr auto const& GetDepthImageView() const noexcept { return this->DepthTarget.ImageView; }

    NODISCARD FORCEINLINE constexpr auto const& GetMsaa() const noexcept { return this->MsaaTarget; }
    NODISCARD FORCEINLINE constexpr bool IsResolvingMsaa() const noexcept { return this->ResolveFlags != vk::ResolveModeFlagBits::eNone; }
    NODISCARD FORCEINLINE constexpr auto const& GetResolved() const noexcept { check(this->ResolvedTarget.has_value()) return *this->ResolvedTarget; }

    FORCEINLINE constexpr bool IsMsaaBindless() const noexcept { return this->MsaaTarget.BindlessIndex != INDEX_NONE; }
    FORCEINLINE constexpr u32 GetMsaaBindlessIndex() const noexcept
    {
        check(this->MsaaTarget.BindlessIndex >= 0 && this->MsaaTarget.BindlessIndex <= static_cast<i64>(std::numeric_limits<u32>::max()))
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
    //# Internal method. Do not use!!!
    FORCEINLINE constexpr void _SetResolvedBindlessIndex(i64 Value) const noexcept
    {
        check(this->ResolvedTarget.has_value())
        this->ResolvedTarget->BindlessIndex = Value;
    }

    NODISCARD FORCEINLINE constexpr auto const& GetSelectedImage() const noexcept { return this->SelectedTarget.Image; }
    NODISCARD FORCEINLINE constexpr auto const& GetSelectedImageView() const noexcept { return this->SelectedTarget.ImageView; }

    NODISCARD FORCEINLINE constexpr bool HasAmbientOcclusion() const noexcept { return !!*this->SceneDepthTarget.Image; }
    NODISCARD FORCEINLINE constexpr auto const& GetSceneDepthImageView() const noexcept { return this->SceneDepthTarget.ImageView; }
    NODISCARD FORCEINLINE constexpr auto const& GetAmbientOcclusionImageView() const noexcept { return this->AmbientOcclusionTarget.ImageView; }
    NODISCARD FORCEINLINE constexpr auto const& GetAmbientOcclusionBlurImageView() const noexcept { return this->AmbientOcclusionBlurTarget.ImageView; }

    //# The depth prepass is cleared to this depth. Has to be beyond what the shaders treat as background.
    static constexpr f32 BackgroundSceneDepth{ 1e30f };
    static constexpr vk::Format SceneDepthFormat{ vk::Format::eR32Sfloat };
    static constexpr vk::Format AmbientOcclusionFormat{ vk::Format::eR8Unorm };

    vk::ClearValue ClearColor{vk::ClearColorValue{std::array<f32,4>{0.0f, 0.0f, 0.0f, 1.0f}}};

protected:

    rhi::extent2 Extent;

    struct Target
    {
        rhi::device_image Image;
        vk::raii::ImageView ImageView{ nullptr };
    };
    Target DepthTarget;

    struct BindlessTarget : public Target
    {
        //# TODO: BUG: !!!This index is leaked. Add dctor somewhere.
        mutable i64 BindlessIndex{ INDEX_NONE };
    };
    BindlessTarget MsaaTarget;

    Target SelectedTarget;

    //# Linear view space depth of the scene as a color, so that it can be sampled.
    Target SceneDepthTarget;
    //# Single sampled depth buffer to depth test the scene depth prepass.
    Target SceneDepthBufferTarget;
    Target AmbientOcclusionTarget;
    Target AmbientOcclusionBlurTarget;

private:

    void RenderAuxiliary(LRenderInfo const& Info, Target const& Color, Target const* Depth, vk::ClearValue const& Clear
        , TFunction2<void(LRenderInfo const& Info)> What) const;

protected:

    vk::ResolveModeFlagBits ResolveMode{ vk::ResolveModeFlagBits::eNone };
    vk::ResolveModeFlagBits ResolveFlags{ vk::ResolveModeFlagBits::eAverage };
    std::optional<BindlessTarget> ResolvedTarget;
};

} /* ~Namespace Jafg */
