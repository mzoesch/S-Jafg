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

#pragma region SerdeEnums
#pragma region SerdeEnumFormat
SERDE_STRING_ENUM_NON_INTRUSIVE(Format,
    eUndefined,
    eR4G4UnormPack8,
    eR4G4B4A4UnormPack16,
    eB4G4R4A4UnormPack16,
    eR5G6B5UnormPack16,
    eB5G6R5UnormPack16,
    eR5G5B5A1UnormPack16,
    eB5G5R5A1UnormPack16,
    eA1R5G5B5UnormPack16,
    eR8Unorm,
    eR8Snorm,
    eR8Uscaled,
    eR8Sscaled,
    eR8Uint,
    eR8Sint,
    eR8Srgb,
    eR8G8Unorm,
    eR8G8Snorm,
    eR8G8Uscaled,
    eR8G8Sscaled,
    eR8G8Uint,
    eR8G8Sint,
    eR8G8Srgb,
    eR8G8B8Unorm,
    eR8G8B8Snorm,
    eR8G8B8Uscaled,
    eR8G8B8Sscaled,
    eR8G8B8Uint,
    eR8G8B8Sint,
    eR8G8B8Srgb,
    eB8G8R8Unorm,
    eB8G8R8Snorm,
    eB8G8R8Uscaled,
    eB8G8R8Sscaled,
    eB8G8R8Uint,
    eB8G8R8Sint,
    eB8G8R8Srgb,
    eR8G8B8A8Unorm,
    eR8G8B8A8Snorm,
    eR8G8B8A8Uscaled,
    eR8G8B8A8Sscaled,
    eR8G8B8A8Uint,
    eR8G8B8A8Sint,
    eR8G8B8A8Srgb,
    eB8G8R8A8Unorm,
    eB8G8R8A8Snorm,
    eB8G8R8A8Uscaled,
    eB8G8R8A8Sscaled,
    eB8G8R8A8Uint,
    eB8G8R8A8Sint,
    eB8G8R8A8Srgb,
    eA8B8G8R8UnormPack32,
    eA8B8G8R8SnormPack32,
    eA8B8G8R8UscaledPack32,
    eA8B8G8R8SscaledPack32,
    eA8B8G8R8UintPack32,
    eA8B8G8R8SintPack32,
    eA8B8G8R8SrgbPack32,
    eA2R10G10B10UnormPack32,
    eA2R10G10B10SnormPack32,
    eA2R10G10B10UscaledPack32,
    eA2R10G10B10SscaledPack32,
    eA2R10G10B10UintPack32,
    eA2R10G10B10SintPack32,
    eA2B10G10R10UnormPack32,
    eA2B10G10R10SnormPack32,
    eA2B10G10R10UscaledPack32,
    eA2B10G10R10SscaledPack32,
    eA2B10G10R10UintPack32,
    eA2B10G10R10SintPack32,
    eR16Unorm,
    eR16Snorm,
    eR16Uscaled,
    eR16Sscaled,
    eR16Uint,
    eR16Sint,
    eR16Sfloat,
    eR16G16Unorm,
    eR16G16Snorm,
    eR16G16Uscaled,
    eR16G16Sscaled,
    eR16G16Uint,
    eR16G16Sint,
    eR16G16Sfloat,
    eR16G16B16Unorm,
    eR16G16B16Snorm,
    eR16G16B16Uscaled,
    eR16G16B16Sscaled,
    eR16G16B16Uint,
    eR16G16B16Sint,
    eR16G16B16Sfloat,
    eR16G16B16A16Unorm,
    eR16G16B16A16Snorm,
    eR16G16B16A16Uscaled,
    eR16G16B16A16Sscaled,
    eR16G16B16A16Uint,
    eR16G16B16A16Sint,
    eR16G16B16A16Sfloat,
    eR32Uint,
    eR32Sint,
    eR32Sfloat,
    eR32G32Uint,
    eR32G32Sint,
    eR32G32Sfloat,
    eR32G32B32Uint,
    eR32G32B32Sint,
    eR32G32B32Sfloat,
    eR32G32B32A32Uint,
    eR32G32B32A32Sint,
    eR32G32B32A32Sfloat,
    eR64Uint,
    eR64Sint,
    eR64Sfloat,
    eR64G64Uint,
    eR64G64Sint,
    eR64G64Sfloat,
    eR64G64B64Uint,
    eR64G64B64Sint,
    eR64G64B64Sfloat,
    eR64G64B64A64Uint,
    eR64G64B64A64Sint,
    eR64G64B64A64Sfloat,
    eB10G11R11UfloatPack32,
    eE5B9G9R9UfloatPack32,
    eD16Unorm,
    eX8D24UnormPack32,
    eD32Sfloat,
    eS8Uint,
    eD16UnormS8Uint,
    eD24UnormS8Uint,
    eD32SfloatS8Uint,
    eBc1RgbUnormBlock,
    eBc1RgbSrgbBlock,
    eBc1RgbaUnormBlock,
    eBc1RgbaSrgbBlock,
    eBc2UnormBlock,
    eBc2SrgbBlock,
    eBc3UnormBlock,
    eBc3SrgbBlock,
    eBc4UnormBlock,
    eBc4SnormBlock,
    eBc5UnormBlock,
    eBc5SnormBlock,
    eBc6HUfloatBlock,
    eBc6HSfloatBlock,
    eBc7UnormBlock,
    eBc7SrgbBlock,
    eEtc2R8G8B8UnormBlock,
    eEtc2R8G8B8SrgbBlock,
    eEtc2R8G8B8A1UnormBlock,
    eEtc2R8G8B8A1SrgbBlock,
    eEtc2R8G8B8A8UnormBlock,
    eEtc2R8G8B8A8SrgbBlock,
    eEacR11UnormBlock,
    eEacR11SnormBlock,
    eEacR11G11UnormBlock,
    eEacR11G11SnormBlock,
    eAstc4x4UnormBlock,
    eAstc4x4SrgbBlock,
    eAstc5x4UnormBlock,
    eAstc5x4SrgbBlock,
    eAstc5x5UnormBlock,
    eAstc5x5SrgbBlock,
    eAstc6x5UnormBlock,
    eAstc6x5SrgbBlock,
    eAstc6x6UnormBlock,
    eAstc6x6SrgbBlock,
    eAstc8x5UnormBlock,
    eAstc8x5SrgbBlock,
    eAstc8x6UnormBlock,
    eAstc8x6SrgbBlock,
    eAstc8x8UnormBlock,
    eAstc8x8SrgbBlock,
    eAstc10x5UnormBlock,
    eAstc10x5SrgbBlock,
    eAstc10x6UnormBlock,
    eAstc10x6SrgbBlock,
    eAstc10x8UnormBlock,
    eAstc10x8SrgbBlock,
    eAstc10x10UnormBlock,
    eAstc10x10SrgbBlock,
    eAstc12x10UnormBlock,
    eAstc12x10SrgbBlock,
    eAstc12x12UnormBlock,
    eAstc12x12SrgbBlock,
    eG8B8G8R8422Unorm,
    eG8B8G8R8422UnormKHR,
    eB8G8R8G8422Unorm,
    eB8G8R8G8422UnormKHR,
    eG8B8R83Plane420Unorm,
    eG8B8R83Plane420UnormKHR,
    eG8B8R82Plane420Unorm,
    eG8B8R82Plane420UnormKHR,
    eG8B8R83Plane422Unorm,
    eG8B8R83Plane422UnormKHR,
    eG8B8R82Plane422Unorm,
    eG8B8R82Plane422UnormKHR,
    eG8B8R83Plane444Unorm,
    eG8B8R83Plane444UnormKHR,
    eR10X6UnormPack16,
    eR10X6UnormPack16KHR,
    eR10X6G10X6Unorm2Pack16,
    eR10X6G10X6Unorm2Pack16KHR,
    eR10X6G10X6B10X6A10X6Unorm4Pack16,
    eR10X6G10X6B10X6A10X6Unorm4Pack16KHR,
    eG10X6B10X6G10X6R10X6422Unorm4Pack16,
    eG10X6B10X6G10X6R10X6422Unorm4Pack16KHR,
    eB10X6G10X6R10X6G10X6422Unorm4Pack16,
    eB10X6G10X6R10X6G10X6422Unorm4Pack16KHR,
    eG10X6B10X6R10X63Plane420Unorm3Pack16,
    eG10X6B10X6R10X63Plane420Unorm3Pack16KHR,
    eG10X6B10X6R10X62Plane420Unorm3Pack16,
    eG10X6B10X6R10X62Plane420Unorm3Pack16KHR,
    eG10X6B10X6R10X63Plane422Unorm3Pack16,
    eG10X6B10X6R10X63Plane422Unorm3Pack16KHR,
    eG10X6B10X6R10X62Plane422Unorm3Pack16,
    eG10X6B10X6R10X62Plane422Unorm3Pack16KHR,
    eG10X6B10X6R10X63Plane444Unorm3Pack16,
    eG10X6B10X6R10X63Plane444Unorm3Pack16KHR,
    eR12X4UnormPack16,
    eR12X4UnormPack16KHR,
    eR12X4G12X4Unorm2Pack16,
    eR12X4G12X4Unorm2Pack16KHR,
    eR12X4G12X4B12X4A12X4Unorm4Pack16,
    eR12X4G12X4B12X4A12X4Unorm4Pack16KHR,
    eG12X4B12X4G12X4R12X4422Unorm4Pack16,
    eG12X4B12X4G12X4R12X4422Unorm4Pack16KHR,
    eB12X4G12X4R12X4G12X4422Unorm4Pack16,
    eB12X4G12X4R12X4G12X4422Unorm4Pack16KHR,
    eG12X4B12X4R12X43Plane420Unorm3Pack16,
    eG12X4B12X4R12X43Plane420Unorm3Pack16KHR,
    eG12X4B12X4R12X42Plane420Unorm3Pack16,
    eG12X4B12X4R12X42Plane420Unorm3Pack16KHR,
    eG12X4B12X4R12X43Plane422Unorm3Pack16,
    eG12X4B12X4R12X43Plane422Unorm3Pack16KHR,
    eG12X4B12X4R12X42Plane422Unorm3Pack16,
    eG12X4B12X4R12X42Plane422Unorm3Pack16KHR,
    eG12X4B12X4R12X43Plane444Unorm3Pack16,
    eG12X4B12X4R12X43Plane444Unorm3Pack16KHR,
    eG16B16G16R16422Unorm,
    eG16B16G16R16422UnormKHR,
    eB16G16R16G16422Unorm,
    eB16G16R16G16422UnormKHR,
    eG16B16R163Plane420Unorm,
    eG16B16R163Plane420UnormKHR,
    eG16B16R162Plane420Unorm,
    eG16B16R162Plane420UnormKHR,
    eG16B16R163Plane422Unorm,
    eG16B16R163Plane422UnormKHR,
    eG16B16R162Plane422Unorm,
    eG16B16R162Plane422UnormKHR,
    eG16B16R163Plane444Unorm,
    eG16B16R163Plane444UnormKHR,
    eG8B8R82Plane444Unorm,
    eG8B8R82Plane444UnormEXT,
    eG10X6B10X6R10X62Plane444Unorm3Pack16,
    eG10X6B10X6R10X62Plane444Unorm3Pack16EXT,
    eG12X4B12X4R12X42Plane444Unorm3Pack16,
    eG12X4B12X4R12X42Plane444Unorm3Pack16EXT,
    eG16B16R162Plane444Unorm,
    eG16B16R162Plane444UnormEXT,
    eA4R4G4B4UnormPack16,
    eA4R4G4B4UnormPack16EXT,
    eA4B4G4R4UnormPack16,
    eA4B4G4R4UnormPack16EXT,
    eAstc4x4SfloatBlock,
    eAstc4x4SfloatBlockEXT,
    eAstc5x4SfloatBlock,
    eAstc5x4SfloatBlockEXT,
    eAstc5x5SfloatBlock,
    eAstc5x5SfloatBlockEXT,
    eAstc6x5SfloatBlock,
    eAstc6x5SfloatBlockEXT,
    eAstc6x6SfloatBlock,
    eAstc6x6SfloatBlockEXT,
    eAstc8x5SfloatBlock,
    eAstc8x5SfloatBlockEXT,
    eAstc8x6SfloatBlock,
    eAstc8x6SfloatBlockEXT,
    eAstc8x8SfloatBlock,
    eAstc8x8SfloatBlockEXT,
    eAstc10x5SfloatBlock,
    eAstc10x5SfloatBlockEXT,
    eAstc10x6SfloatBlock,
    eAstc10x6SfloatBlockEXT,
    eAstc10x8SfloatBlock,
    eAstc10x8SfloatBlockEXT,
    eAstc10x10SfloatBlock,
    eAstc10x10SfloatBlockEXT,
    eAstc12x10SfloatBlock,
    eAstc12x10SfloatBlockEXT,
    eAstc12x12SfloatBlock,
    eAstc12x12SfloatBlockEXT,
    eA1B5G5R5UnormPack16,
    eA1B5G5R5UnormPack16KHR,
    eA8Unorm,
    eA8UnormKHR,
    ePvrtc12BppUnormBlockIMG,
    ePvrtc14BppUnormBlockIMG,
    ePvrtc22BppUnormBlockIMG,
    ePvrtc24BppUnormBlockIMG,
    ePvrtc12BppSrgbBlockIMG,
    ePvrtc14BppSrgbBlockIMG,
    ePvrtc22BppSrgbBlockIMG,
    ePvrtc24BppSrgbBlockIMG,
    eAstc3x3x3UnormBlockEXT,
    eAstc3x3x3SrgbBlockEXT,
    eAstc3x3x3SfloatBlockEXT,
    eAstc4x3x3UnormBlockEXT,
    eAstc4x3x3SrgbBlockEXT,
    eAstc4x3x3SfloatBlockEXT,
    eAstc4x4x3UnormBlockEXT,
    eAstc4x4x3SrgbBlockEXT,
    eAstc4x4x3SfloatBlockEXT,
    eAstc4x4x4UnormBlockEXT,
    eAstc4x4x4SrgbBlockEXT,
    eAstc4x4x4SfloatBlockEXT,
    eAstc5x4x4UnormBlockEXT,
    eAstc5x4x4SrgbBlockEXT,
    eAstc5x4x4SfloatBlockEXT,
    eAstc5x5x4UnormBlockEXT,
    eAstc5x5x4SrgbBlockEXT,
    eAstc5x5x4SfloatBlockEXT,
    eAstc5x5x5UnormBlockEXT,
    eAstc5x5x5SrgbBlockEXT,
    eAstc5x5x5SfloatBlockEXT,
    eAstc6x5x5UnormBlockEXT,
    eAstc6x5x5SrgbBlockEXT,
    eAstc6x5x5SfloatBlockEXT,
    eAstc6x6x5UnormBlockEXT,
    eAstc6x6x5SrgbBlockEXT,
    eAstc6x6x5SfloatBlockEXT,
    eAstc6x6x6UnormBlockEXT,
    eAstc6x6x6SrgbBlockEXT,
    eAstc6x6x6SfloatBlockEXT,
    eR8BoolARM,
    eR16SfloatFpencodingBfloat16ARM,
    eR8SfloatFpencodingFloat8E4M3ARM,
    eR8SfloatFpencodingFloat8E5M2ARM,
    eR16G16Sfixed5NV,
    eR16G16S105NV,
    eR10X6UintPack16ARM,
    eR10X6G10X6Uint2Pack16ARM,
    eR10X6G10X6B10X6A10X6Uint4Pack16ARM,
    eR12X4UintPack16ARM,
    eR12X4G12X4Uint2Pack16ARM,
    eR12X4G12X4B12X4A12X4Uint4Pack16ARM,
    eR14X2UintPack16ARM,
    eR14X2G14X2Uint2Pack16ARM,
    eR14X2G14X2B14X2A14X2Uint4Pack16ARM,
    eR14X2UnormPack16ARM,
    eR14X2G14X2Unorm2Pack16ARM,
    eR14X2G14X2B14X2A14X2Unorm4Pack16ARM,
    eG14X2B14X2R14X22Plane420Unorm3Pack16ARM,
    eG14X2B14X2R14X22Plane422Unorm3Pack16ARM
    )
#pragma endregion /* SerdeEnumFormat */

SERDE_STRING_ENUM_NON_INTRUSIVE(SampleCountFlagBits,
    e1,
    e2,
    e4,
    e8,
    e16,
    e32,
    e64
    )
#pragma endregion /* SerdeEnums */

} /* ~Namespace vk */

template<> struct serde::is_stable_enum<vk::Format> : std::true_type {};
static_assert(sizeof(vk::Format) == 4);
template<> struct serde::is_stable_enum<vk::SampleCountFlagBits> : std::true_type {};
static_assert(sizeof(vk::SampleCountFlagBits) == 4);

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
