// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Rhi/MaxDesiredFramesInFlight.h"

#if !JAFG_NO_GLAD
    // #include <glad/glad.h>  /* Include glad to get all the required OpenGL headers. */
#endif /* !JAFG_NO_GLAD */

#if !JAFG_NO_GLFW3
    #ifndef GLFW_INCLUDE_NONE
        #define GLFW_INCLUDE_NONE
    #endif /* !GLFW_INCLUDE_NONE */
    // #include <GLFW/glfw3.h> /* Include glfw3 after glad to avoid include order issues. */
#endif /* !JAFG_NO_GLFW3 */

#define VK_NO_PROTOTYPES
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan_raii.hpp>
// #include <volk.h>

// We support Vulkan 1.4. This is currently (2025) the latest version. Therefore, we do not need this right now; tm.
// #define VMA_VULKAN_VERSION 1004000
#if JAFG_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Weverything"
#endif /* JAFG_WITH_CLANG */
    #include "vk_mem_alloc.h"
#if JAFG_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* JAFG_WITH_CLANG */

#if PLATFORM_USES_WEBGL_TWO
    #include <GLES3/gl3.h>
#endif /* PLATFORM_USES_WEBGL_TWO */

#include "Framework/FrontendForward.h"

namespace vk
{

NLOHMANN_JSON_SERIALIZE_ENUM(CompareOp, {
    {CompareOp::eNever, "eNever"},
    {CompareOp::eLess, "eLess"},
    {CompareOp::eEqual, "eEqual"},
    {CompareOp::eLessOrEqual, "eLessOrEqual"},
    {CompareOp::eGreater, "eGreater"},
    {CompareOp::eNotEqual, "eNotEqual"},
    {CompareOp::eGreaterOrEqual, "eGreaterOrEqual"},
    {CompareOp::eAlways, "eAlways"},
    })
NLOHMANN_JSON_SERIALIZE_ENUM(StencilOp, {
    {StencilOp::eKeep, "eKeep"},
    {StencilOp::eZero, "eZero"},
    {StencilOp::eReplace, "eReplace"},
    {StencilOp::eIncrementAndClamp, "eIncrementAndClamp"},
    {StencilOp::eDecrementAndClamp, "eDecrementAndClamp"},
    {StencilOp::eInvert, "eInvert"},
    {StencilOp::eIncrementAndWrap, "eIncrementAndWrap"},
    {StencilOp::eDecrementAndWrap, "eDecrementAndWrap"},
    })
NLOHMANN_JSON_SERIALIZE_ENUM(PrimitiveTopology, {
    {PrimitiveTopology::ePointList, "ePointList"},
    {PrimitiveTopology::eLineList, "eLineList"},
    {PrimitiveTopology::eLineStrip, "eLineStrip"},
    {PrimitiveTopology::eTriangleList, "eTriangleList"},
    {PrimitiveTopology::eTriangleStrip, "eTriangleStrip"},
    {PrimitiveTopology::eTriangleFan, "eTriangleFan"},
    {PrimitiveTopology::eLineListWithAdjacency, "eLineListWithAdjacency"},
    {PrimitiveTopology::eLineStripWithAdjacency, "eLineStripWithAdjacency"},
    {PrimitiveTopology::eTriangleListWithAdjacency, "eTriangleListWithAdjacency"},
    {PrimitiveTopology::eTriangleStripWithAdjacency, "eTriangleStripWithAdjacency"},
    {PrimitiveTopology::ePatchList, "ePatchList"},
    })

ENGINE_API void from_json(json const& j, PipelineInputAssemblyStateCreateInfo& Info);
ENGINE_API void from_json(json const& j, PipelineDepthStencilStateCreateInfo& Info);
ENGINE_API void from_json(json const& j, StencilOpState& State);

} /* ~Namespace vk */

namespace rhi
{

typedef VmaAllocation device_allocation;
typedef VmaAllocationInfo device_allocation_info;

//# Present modes that jafg supports.
enum struct present_mode
{
    Immediate,
    Mailbox,
    Fifo,
    FifoRelaxed,
};
NODISCARD inline constexpr vk::PresentModeKHR vk_to_khr_present_mode(present_mode mode) noexcept
{
    switch (mode)
    {
    case present_mode::Immediate: return vk::PresentModeKHR::eImmediate;
    case present_mode::Mailbox: return vk::PresentModeKHR::eMailbox;
    case present_mode::Fifo: return vk::PresentModeKHR::eFifo;
    case present_mode::FifoRelaxed: return vk::PresentModeKHR::eFifoRelaxed;
    }
    std::unreachable();
}
NODISCARD inline constexpr bool is_present_mode_blocking(present_mode mode) noexcept
{
    return mode == present_mode::Fifo || mode == present_mode::FifoRelaxed;
}
NODISCARD LStringView inline constexpr to_string(present_mode Mode) noexcept
{
    switch (Mode)
    {
    case present_mode::Immediate: return "Immediate";
    case present_mode::Mailbox: return "Mailbox";
    case present_mode::Fifo: return "Fifo";
    case present_mode::FifoRelaxed: return "FifoRelaxed";
    }
    std::unreachable();
}

//# A two-dimensional extent structure compatible with the current underlying device API used.
struct extent2
{
    typedef vk::Extent2D type;
    typedef u32 domain_type;

    NODISCARD FORCEINLINE static constexpr extent2 from_native(type const& Native) noexcept
    {
        return *reinterpret_cast<extent2 const*>(&Native);
    }
    template<typename Value>
    NODISCARD FORCEINLINE static constexpr extent2 from_vec(TVec2<Value,maths::defaultp> Vec) noexcept
    {
        return {.width=static_cast<u32>(Vec.x), .height=static_cast<u32>(Vec.y)};
    }

    NODISCARD FORCEINLINE constexpr operator type&() noexcept { return *reinterpret_cast<type*>( this ); }
    NODISCARD FORCEINLINE constexpr operator type const&() const noexcept { return *reinterpret_cast<type const*>(this); }
    NODISCARD FORCEINLINE constexpr operator type*() noexcept { return reinterpret_cast<type*>(this); }
    NODISCARD FORCEINLINE constexpr operator type const*() const noexcept { return reinterpret_cast<type const*>(this); }

    NODISCARD FORCEINLINE constexpr auto operator<=>(extent2 const&) const = default;

    template<typename T>
    NODISCARD FORCEINLINE constexpr TVec2<T, maths::defaultp> ToVec() const noexcept
    {
        return {static_cast<T>(this->width), static_cast<T>(this->height)};
    }

    u32 width{};
    u32 height{};

    NODISCARD FORCEINLINE LString ToString() const noexcept
    {
        return std::format("extent2({}, {})", this->width, this->height);
    }
};
static_assert(std::is_standard_layout_v<extent2>);
static_assert(sizeof(extent2) == sizeof(extent2::type));
static_assert(std::is_same_v<decltype(extent2::width), decltype(extent2::type::width)>);
static_assert(std::is_same_v<decltype(extent2::height), decltype(extent2::type::height)>);
static_assert(offsetof(extent2, width) == offsetof(extent2::type, width));
static_assert(offsetof(extent2, height) == offsetof(extent2::type, height));
static_assert(std::is_same_v<extent2::type, vk::Extent2D>);

//# High-level texture coordinates behavior.
enum struct tex_coord_behavior : u8
{
    //# Scale UVs normalized.
    Scale,
    //# Scale UVs so that the vertical component of the texture is always [0,1] while preserving aspect.
    FitV,
    //# Scale UVs so that the horizontal component of the texture is always [0,1] while preserving aspect.
    FitH,
    //# Scale UVs so that the texture fits entirely in the target area while preserving aspect.
    FitAspect,
};
namespace uv
{
inline constexpr LVec4F identity{0.0f, 0.0f, 1.0f, 1.0f};
NODISCARD inline constexpr LVec4F fit_v(LVec4F UVs, LVec2F Extent, LVec2F TargetExtent) noexcept
{
    f32 CenterU{(UVs.x + UVs.z) * 0.5f};
    f32 ScaledU{(UVs.w - UVs.y) * ((TargetExtent.x  / TargetExtent.y) / (Extent.x / Extent.y))};
    return {
        CenterU - ScaledU * 0.5f, UVs.y,
        CenterU + ScaledU * 0.5f, UVs.w
        };
}
NODISCARD inline constexpr LVec4F fit_h(LVec4F UVs, LVec2F Extent, LVec2F TargetExtent) noexcept
{
    f32 CenterV{(UVs.y + UVs.w) * 0.5f};
    f32 ScaledV{(UVs.z - UVs.x) * ((Extent.x / Extent.y) / (TargetExtent.x  / TargetExtent.y))};
    return {
        UVs.x, CenterV - ScaledV * 0.5f,
        UVs.z, CenterV + ScaledV * 0.5f
        };
}
NODISCARD inline constexpr LVec4F fit_aspect(LVec4F UVs, LVec2F Extent, LVec2F TargetExtent) noexcept
{
    f32 AspectRatio{(TargetExtent.x  / TargetExtent.y) / (Extent.x / Extent.y)};
    if (AspectRatio > 1.0f)
    {
        return fit_v(UVs, Extent, TargetExtent);
    }
    return fit_h(UVs, Extent, TargetExtent);
}
NODISCARD inline constexpr LVec4F fit(LVec4F UVs, LVec2F Extent, LVec2F TargetExtent, tex_coord_behavior Behavior) noexcept
{
    if (Behavior == tex_coord_behavior::FitV) { return fit_v(UVs, Extent, TargetExtent); }
    if (Behavior == tex_coord_behavior::FitH) { return fit_h(UVs, Extent, TargetExtent); }
    if (Behavior == tex_coord_behavior::FitAspect) { return fit_aspect(UVs, Extent, TargetExtent); }
    return UVs;
}
NODISCARD inline constexpr LVec4F scale(LVec4F const& UVs, f32 Scale) noexcept
{
    check(Scale != 0.0f)
    LVec2F Center{(maths::xy(UVs) + maths::zw(UVs)) * 0.5f};
    LVec2F HalfSize{(maths::zw(UVs) - maths::xy(UVs)) * 0.5f / Scale};
    return {
        Center - HalfSize,
        Center + HalfSize,
        };
}
NODISCARD inline constexpr LVec4F pad(LVec4F UVs, f32 Padding, LVec2F Extent) noexcept
{
    LVec2F PaddingUV{Padding / Extent.x, Padding / Extent.y};
    return {
        maths::xy(UVs) + PaddingUV,
        maths::zw(UVs) - PaddingUV,
        };
}
NODISCARD inline constexpr LVec4F pipe(LVec4F UVs, LVec2F Extent, LVec2F TargetExtent, tex_coord_behavior Behavior
    , f32 Padding = 0.0f, f32 Scale = 1.0f) noexcept
{
    return pad(scale(fit(UVs, Extent, TargetExtent, Behavior), Scale), Padding, Extent);
}
} /* ~Namespace uv */

NODISCARD inline constexpr vk::SampleCountFlagBits vk_get_max_msaa_sample(vk::SampleCountFlags Counts) noexcept
{
    if (Counts & vk::SampleCountFlagBits::e64) { return vk::SampleCountFlagBits::e64; }
    if (Counts & vk::SampleCountFlagBits::e32) { return vk::SampleCountFlagBits::e32; }
    if (Counts & vk::SampleCountFlagBits::e16) { return vk::SampleCountFlagBits::e16; }
    if (Counts & vk::SampleCountFlagBits::e8)  { return vk::SampleCountFlagBits::e8; }
    if (Counts & vk::SampleCountFlagBits::e4)  { return vk::SampleCountFlagBits::e4; }
    if (Counts & vk::SampleCountFlagBits::e2)  { return vk::SampleCountFlagBits::e2; }
    if (Counts & vk::SampleCountFlagBits::e1)  { return vk::SampleCountFlagBits::e1; }
    std::unreachable();
}
NODISCARD inline constexpr vk::SampleCountFlagBits vk_clamp_msaa_samples(vk::SampleCountFlagBits Desired, vk::SampleCountFlags Allowed) noexcept
{
    if (Desired == vk::SampleCountFlagBits::e64 && (Allowed & vk::SampleCountFlagBits::e64)) { return vk::SampleCountFlagBits::e64; }
    if (Desired == vk::SampleCountFlagBits::e32 && (Allowed & vk::SampleCountFlagBits::e32)) { return vk::SampleCountFlagBits::e32; }
    if (Desired == vk::SampleCountFlagBits::e16 && (Allowed & vk::SampleCountFlagBits::e16)) { return vk::SampleCountFlagBits::e16; }
    if (Desired == vk::SampleCountFlagBits::e8 && (Allowed & vk::SampleCountFlagBits::e8))   { return vk::SampleCountFlagBits::e8; }
    if (Desired == vk::SampleCountFlagBits::e4 && (Allowed & vk::SampleCountFlagBits::e4))   { return vk::SampleCountFlagBits::e4; }
    if (Desired == vk::SampleCountFlagBits::e2 && (Allowed & vk::SampleCountFlagBits::e2))   { return vk::SampleCountFlagBits::e2; }
    if (Desired == vk::SampleCountFlagBits::e1 && (Allowed & vk::SampleCountFlagBits::e1))   { return vk::SampleCountFlagBits::e1; }
    LOG_FATAL(LogRhi, "Unsupported desired MSAA sample count [{}] or it is not allowed by the device capabilities [{}]."
        , vk::to_string(Desired), vk::to_string(Allowed))
}

NODISCARD inline constexpr std::size_t vk_channels_per_pixel(vk::Format Format) noexcept
{
    switch (Format)
    {
    case vk::Format::eR8G8B8A8Srgb:
    {
        return 4;
    }
    case vk::Format::eR8G8B8Srgb:
    {
        return 3;
    }
    default:
    {
        LOG_FATAL(LogRhi, "Unsupported format [{}] for channels per pixel query.", vk::to_string(Format) )
    }
    }
}

inline constexpr std::size_t vk_bytes_per_pixel(vk::Format Format) noexcept
{
    switch (Format)
    {
    case vk::Format::eR8Unorm:
    {
        return 1;
    }
    case vk::Format::eR8G8Unorm:
    {
        return 2;
    }
    case vk::Format::eR8G8B8Srgb:
    {
        return 3;
    }
    case vk::Format::eR8G8B8A8Unorm:
    case vk::Format::eR8G8B8A8Srgb:
    {
        return 4;
    }
    case vk::Format::eB8G8R8A8Unorm:
    {
        return 4;
    }
    case vk::Format::eR16G16B16A16Sfloat:
    {
        return 8;
    }
    case vk::Format::eR32G32B32A32Sfloat:
    {
        return 16;
    }
    default:
    {
        LOG_FATAL(LogRhi, "Unsupported format [{}] for bytes per pixel query.", vk::to_string(Format) )
    }
    }
}

} /* ~Namespace rhi */

JAFG_PREF_OF(rhi::present_mode)
