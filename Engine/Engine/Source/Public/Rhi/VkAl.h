// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"
#include "Rhi/VkForward.h"
#include "Rhi/RhiVendorInclude.h"
#include "Platform/SurfaceForward.h"

//#
//# File:
//#     Vulkan Abstraction Layer
//#
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

namespace Jafg
{

typedef VmaAllocation LDeviceAllocation;
typedef VmaAllocationInfo LDeviceAllocationInfo;

namespace Detail
{

ENGINE_API void FreeDeviceAllocation(vk::Buffer Handle, LDeviceAllocation Allocation) noexcept;
ENGINE_API void FreeDeviceAllocation(vk::Image Handle, LDeviceAllocation Allocation) noexcept;

} /* ~Namespace Jafg::Detail */

//# A generic buffer located on the device.
template<typename T>
struct TGenericDeviceBuffer
{
    constexpr TGenericDeviceBuffer() noexcept : Buffer{nullptr}, Allocation{nullptr} {}
    constexpr TGenericDeviceBuffer(T InBuffer, LDeviceAllocation InAllocation) noexcept : Buffer{InBuffer}, Allocation{InAllocation} {}
    PROHIBIT_COPY(TGenericDeviceBuffer)
    constexpr TGenericDeviceBuffer(TGenericDeviceBuffer&& Other) noexcept : Buffer{ Other.Buffer }, Allocation{ Other.Allocation }
    {
        Other.Buffer = nullptr;
        Other.Allocation = nullptr;
    }
    TGenericDeviceBuffer& operator=(TGenericDeviceBuffer&& Other) noexcept
    {
        check( this != &Other )

        Detail::FreeDeviceAllocation(this->Buffer, this->Allocation);

        this->Buffer = Other.Buffer;
        this->Allocation = Other.Allocation;

        Other.Buffer = nullptr;
        Other.Allocation = nullptr;

        return *this;
    }
    ~TGenericDeviceBuffer() noexcept { Detail::FreeDeviceAllocation(this->Buffer, this->Allocation); }

    constexpr void Release() noexcept
    {
        this->Buffer = nullptr;
        this->Allocation = nullptr;
    }

    void Free() noexcept
    {
        Detail::FreeDeviceAllocation(this->Buffer, this->Allocation);
        this->Buffer = nullptr;
        this->Allocation = nullptr;
    }

    T Buffer;
    LDeviceAllocation Allocation;
};

typedef TGenericDeviceBuffer<vk::Buffer> LDeviceBuffer;
typedef TGenericDeviceBuffer<vk::Image> LDeviceImage;

//# A device buffer with detailed allocation info.
struct LDetailedDeviceBuffer : public LDeviceBuffer
{
    constexpr LDetailedDeviceBuffer() noexcept
        : LDeviceBuffer{}, Info{} {}
    constexpr LDetailedDeviceBuffer(vk::Buffer InBuffer, LDeviceAllocation InAllocation, LDeviceAllocationInfo&& InInfo) noexcept
        : LDeviceBuffer{ InBuffer, InAllocation }, Info{ std::move(InInfo) } {}
    PROHIBIT_COPY(LDetailedDeviceBuffer)
    constexpr LDetailedDeviceBuffer(LDetailedDeviceBuffer&& Other) noexcept
        : LDeviceBuffer{ std::move(Other) }, Info{ Other.Info }
    {
        Other.Info = {};
    }
    LDetailedDeviceBuffer& operator=(LDetailedDeviceBuffer&& Other) noexcept
    {
        check( this != &Other )

        LDeviceBuffer::operator=(std::move(Other));
        this->Info = Other.Info;
        Other.Info = {};

        return *this;
    }

    constexpr void Release() noexcept
    {
        LDeviceBuffer::Release();
        this->Info = {};
    }

    void Free() noexcept
    {
        LDeviceBuffer::Free();
        this->Info = {};
    }

    LDeviceAllocationInfo Info;
};

//# A device buffer that is mapped to host visible memory.
struct LMappedDeviceBuffer : public LDeviceBuffer
{
    constexpr LMappedDeviceBuffer() noexcept
        : LDeviceBuffer{}, Data{nullptr} {}
    constexpr LMappedDeviceBuffer(vk::Buffer InBuffer, LDeviceAllocation InAllocation, void* InData) noexcept
        : LDeviceBuffer{ InBuffer, InAllocation }, Data{ InData } {}
    PROHIBIT_COPY(LMappedDeviceBuffer)
    constexpr LMappedDeviceBuffer(LMappedDeviceBuffer&& Other) noexcept
        : LDeviceBuffer{ std::move(Other) }, Data{ Other.Data }
    {
        Other.Data = nullptr;
    }
    LMappedDeviceBuffer& operator=(LMappedDeviceBuffer&& Other) noexcept
    {
        check( this != &Other )

        LDeviceBuffer::operator=(std::move(Other));
        this->Data = Other.Data;
        Other.Data = nullptr;

        return *this;
    }

    constexpr void Release() noexcept
    {
        LDeviceBuffer::Release();
        this->Data = nullptr;
    }

    void Free() noexcept
    {
        LDeviceBuffer::Free();
        this->Data = nullptr;
    }

    void* Data;
};

//# A device buffer holing vertices and indices.
struct LDeviceIndexVertexBuffer
{
    //# TODO Make this struct that can hold to buffers in one single vk::buffer? for better cache locality?
    // https://developer.nvidia.com/vulkan-memory-management
};

/* TODO: We can probably solve this with reflection in C++26? Bombastic sideeye */
template<typename T>
concept CDeviceVertexInput = std::is_standard_layout_v<T> && requires (T&& t)
{
    { t.BindingDescriptions().data() } -> std::same_as<vk::VertexInputBindingDescription const*>;
    { t.BindingDescriptions().size() } -> std::same_as<LSize>;
    { t.AttributeDescriptions().data() } -> std::same_as<vk::VertexInputAttributeDescription const*>;
    { t.AttributeDescriptions().size() } -> std::same_as<LSize>;
};

template<typename T>
concept CDeviceLayout = std::is_standard_layout_v<T> && requires (T&& t)
{
    { t.Bindings().data() } -> std::same_as<vk::DescriptorSetLayoutBinding const*>;
    { t.Bindings().size() } -> std::same_as<LSize>;
};

struct LDevicePipeline
{
    void Free() noexcept
    {
        this->Pipeline = nullptr;
        this->PipelineLayout = nullptr;
        this->DescriptorSetLayout = nullptr;
    }

    vk::raii::Pipeline Pipeline{ nullptr };
    vk::raii::PipelineLayout PipelineLayout{ nullptr };
    vk::raii::DescriptorSetLayout DescriptorSetLayout{ nullptr };
};

struct LDevicePipelineFactory
{
    explicit LDevicePipelineFactory(LSurface const& InSurface) noexcept : Surface{ InSurface } {}

    PROHIBIT_REALLOC_OF_ANY_FORM(LDevicePipelineFactory)

    decltype(auto) Shader(this auto&& Self, LPath const& Path, vk::ShaderStageFlags Stages)
    {
        const auto Code{ Finder::ReadFileAsBinary(Path) };
        Self.ShaderModules.emplace_back(vk::raii::ShaderModule{
            Self.Surface.GetFrontend().Vk_GetDevice(),
            vk::ShaderModuleCreateInfo{
                .codeSize = Code.size() * sizeof(u8),
                .pCode = reinterpret_cast<u32 const*>(Code.data())
                }
            });
        auto ShaderModuleHandle{ *Self.ShaderModules.back() };

        if (Stages & vk::ShaderStageFlagBits::eVertex)
        {
            Self.Shaders.emplace_back(vk::PipelineShaderStageCreateInfo{
                .stage = vk::ShaderStageFlagBits::eVertex,
                .module = ShaderModuleHandle,
                .pName = "vertMain",
                });
        }

        if (Stages & vk::ShaderStageFlagBits::eFragment)
        {
            Self.Shaders.emplace_back(vk::PipelineShaderStageCreateInfo{
                .stage = vk::ShaderStageFlagBits::eFragment,
                .module = ShaderModuleHandle,
                .pName = "fragMain",
                });
        }

        check( (Stages & ~(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)) == vk::ShaderStageFlags{}
            && "These stage flags are not yet supported." )

        return std::forward<decltype(Self)>(Self);
    }

    template<CDeviceVertexInput TDeviceVertexInput>
    decltype(auto) VertexInput(this auto&& Self) noexcept
    {
        check( Self.VertexInputInfo.has_value() == false )
        Self.VertexInputInfo = {
            .vertexBindingDescriptionCount = static_cast<u32>(TDeviceVertexInput::BindingDescriptions().size()),
            .pVertexBindingDescriptions = TDeviceVertexInput::BindingDescriptions().data(),
            .vertexAttributeDescriptionCount = static_cast<u32>(TDeviceVertexInput::AttributeDescriptions().size()),
            .pVertexAttributeDescriptions = TDeviceVertexInput::AttributeDescriptions().data(),
            };

        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) InputAssembly(this auto&& Self, vk::PipelineInputAssemblyStateCreateInfo&& Info) noexcept
    {
        Self.InputAssemblyInfo = std::move(Info);
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) ViewportState(this auto&& Self, vk::PipelineViewportStateCreateInfo&& Info) noexcept
    {
        Self.ViewportStateInfo = std::move(Info);
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) Rasterization(this auto&& Self, vk::PipelineRasterizationStateCreateInfo&& Info) noexcept
    {
        Self.RasterizationInfo = std::move(Info);
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) MultisamplingShading(this auto&& Self, vk::Bool32 Enable) noexcept
    {
        Self.MultisamplingShadingEnable = Enable;
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) DepthStencil(this auto&& Self, vk::PipelineDepthStencilStateCreateInfo&& Info) noexcept
    {
        Self.DepthStencilInfo = std::move(Info);
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) ColorBlending(this auto&& Self, vk::LogicOp Op) noexcept
    {
        Self.ColorBlendLogicOpEnable = vk::True;
        Self.ColorBlendLogicalOp = Op;
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) ColorBlendAttachment(this auto&& Self, vk::PipelineColorBlendAttachmentState&& Info) noexcept
    {
        Self.ColorBlendAttachment = std::move(Info);
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) DynamicStates(this auto&& Self, std::array<vk::DynamicState, 2>&& States) noexcept
    {
        Self.DynamicStates = std::move(States);
        return std::forward<decltype(Self)>(Self);
    }

    template<CDeviceLayout TDeviceLayout>
    decltype(auto) Layout(this auto&& Self) noexcept
    {
        check( *Self.DescriptorSetLayout == nullptr )

        Self.DescriptorSetLayout = vk::raii::DescriptorSetLayout{
            Self.Surface.GetFrontend().Vk_GetDevice(),
            vk::DescriptorSetLayoutCreateInfo{
                .bindingCount = static_cast<u32>(TDeviceLayout::Bindings().size()),
                .pBindings = TDeviceLayout::Bindings().data(),
                }
            };

        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) Push(this auto&& Self, vk::PushConstantRange&& Range) noexcept
    {
        check( Self.Range.has_value() == false )
        Self.Range = std::move(Range);
        return std::forward<decltype(Self)>(Self);
    }

    ENGINE_API LDevicePipeline Build();

    LSurface const& Surface;
    TArray<vk::raii::ShaderModule> ShaderModules;
    TArray<vk::PipelineShaderStageCreateInfo> Shaders;
    std::optional<vk::PipelineVertexInputStateCreateInfo> VertexInputInfo;
    vk::PipelineInputAssemblyStateCreateInfo InputAssemblyInfo{
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = vk::False
        };
    vk::PipelineViewportStateCreateInfo ViewportStateInfo{
        .viewportCount = 1,
        .scissorCount = 1,
        };
    vk::PipelineRasterizationStateCreateInfo RasterizationInfo{
        .depthClampEnable = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable = vk::False,
        .depthBiasSlopeFactor = 1.0f,
        .lineWidth = 1.0f
        };
    vk::Bool32 MultisamplingShadingEnable{ vk::False };
    vk::PipelineDepthStencilStateCreateInfo DepthStencilInfo{
        .depthTestEnable       = vk::True,
        .depthWriteEnable      = vk::True,
        .depthCompareOp        = vk::CompareOp::eLess,
        .depthBoundsTestEnable = vk::False,
        .stencilTestEnable     = vk::False
        };
    vk::Bool32 ColorBlendLogicOpEnable{ vk::False };
    vk::LogicOp ColorBlendLogicalOp{ vk::LogicOp::eCopy };
    vk::PipelineColorBlendAttachmentState ColorBlendAttachmentState{
        .blendEnable = vk::False,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
                        | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
        };
    std::array<vk::DynamicState, 2> DynamicStateInfo{
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
        };
    vk::raii::DescriptorSetLayout DescriptorSetLayout{ nullptr };
    vk::raii::PipelineLayout PipelineLayout{ nullptr };
    std::optional<vk::PushConstantRange> Range;
};

inline LSize Vk_GetBytesPerPixel(vk::Format Format) noexcept
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
        panicMsgf( "Unsupported or non-linear format [{}] for bytes per pixel query.", vk::to_string(Format) )
    }
    }
}

} /* ~Namespace Jafg */
