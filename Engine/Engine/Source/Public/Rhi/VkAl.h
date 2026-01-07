// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"
#include "Rhi/RhiVendorInclude.h"

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

} /* ~Namespace Jafg::Detail */

//# A buffer located on the device.
struct LDeviceBuffer
{
    constexpr LDeviceBuffer() noexcept : Buffer{nullptr}, Allocation{nullptr} {}
    constexpr LDeviceBuffer(vk::Buffer InBuffer, LDeviceAllocation InAllocation) noexcept : Buffer{InBuffer}, Allocation{InAllocation} {}
    PROHIBIT_COPY(LDeviceBuffer)
    constexpr LDeviceBuffer(LDeviceBuffer&& Other) noexcept : Buffer{ Other.Buffer }, Allocation{ Other.Allocation }
    {
        Other.Buffer = nullptr;
        Other.Allocation = nullptr;
    }
    LDeviceBuffer& operator=(LDeviceBuffer&& Other) noexcept
    {
        check( this != &Other )

        Detail::FreeDeviceAllocation(this->Buffer, this->Allocation);

        this->Buffer = Other.Buffer;
        this->Allocation = Other.Allocation;

        Other.Buffer = nullptr;
        Other.Allocation = nullptr;

        return *this;
    }
    ~LDeviceBuffer() noexcept { Detail::FreeDeviceAllocation(this->Buffer, this->Allocation); }

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

    vk::Buffer Buffer;
    LDeviceAllocation Allocation;
};

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

struct LDevicePipelineConfig
{
};

/* TODO: We can probably solve this with reflection in C++26? Bombastic sideeye */
template<typename T>
concept CDeviceVertexInput = requires (T&& t)
{
    { t.GetBindingDescriptions().data() } -> std::same_as<vk::VertexInputBindingDescription const*>;
    { t.GetBindingDescriptions().size() } -> std::same_as<LSize>;
    { t.GetAttributeDescriptions().data() } -> std::same_as<vk::VertexInputAttributeDescription const*>;
    { t.GetAttributeDescriptions().size() } -> std::same_as<LSize>;
};

struct LDevicePipelineFactory
{
    explicit LDevicePipelineFactory(LSurface const& InSurface) noexcept : Surface{ InSurface } {}

    PROHIBIT_REALLOC_OF_ANY_FORM(LDevicePipelineFactory)

    decltype(auto) Push(this auto&& Self, vk::PushConstantRange&& Range)
    {
        check( Self.Range.has_value() == false )
        Self.Range = std::move(Range);
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) Shader(this auto&& Self, LPath const& Path, vk::ShaderStageFlags Stages)
    {
        const auto Code{ Finder::ReadFileAsBinary(Path) };
        Self.ShaderModules.emplace_back(vk::raii::ShaderModule{
            Self.Surface.GetFrontend().GetVkDevice(),
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
    decltype(auto) VertexInput(this auto&& Self)
    {
        check( Self.VertexInputInfo.has_value() == false )
        Self.VertexInputInfo = {
            .vertexBindingDescriptionCount = static_cast<u32>(TDeviceVertexInput::GetBindingDescriptions().size()),
            .pVertexBindingDescriptions = TDeviceVertexInput::GetBindingDescriptions().data(),
            .vertexAttributeDescriptionCount = static_cast<u32>(TDeviceVertexInput::GetAttributeDescriptions().size()),
            .pVertexAttributeDescriptions = TDeviceVertexInput::GetAttributeDescriptions().data(),
            };

        return std::forward<decltype(Self)>(Self);
    }

    vk::raii::Pipeline Build()
    {
        auto& Frontend{ this->Surface.GetFrontend() };

        vk::raii::PipelineLayout Layout{
            Frontend.GetVkDevice(),
            vk::PipelineLayoutCreateInfo{
                .setLayoutCount = 0, // TODO
                .pSetLayouts = nullptr,
                .pushConstantRangeCount = static_cast<uint32_t>(this->Range.has_value() ? 1uz : 0uz),
                .pPushConstantRanges = this->Range.has_value() ? &*this->Range : nullptr
                }
            };

        vk::PipelineDynamicStateCreateInfo DynamicStateInfo{
            .dynamicStateCount = static_cast<u32>(this->DynamicStates.size()),
            .pDynamicStates = this->DynamicStates.data(),
            };

        vk::PipelineColorBlendStateCreateInfo ColorBlendInfo{
            .logicOpEnable = this->ColorBlendLogicOpEnable,
            .logicOp = this->ColorBlendLogicalOp,
            .attachmentCount = 1,
            .pAttachments = &this->ColorBlendAttachment
            };

        vk::PipelineMultisampleStateCreateInfo MultisamplingInfo{
            .rasterizationSamples = this->Surface.GetFrontend().GetMaxMsaaSamples(),
            .sampleShadingEnable = this->MultisamplingShadingEnable
            };

        vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> Chain{
            {
                .stageCount = static_cast<u32>(this->Shaders.size()),
                .pStages = this->Shaders.data(),
                .pVertexInputState = this->VertexInputInfo.has_value() ? &*this->VertexInputInfo : nullptr,
                .pInputAssemblyState = &this->InputAssemblyInfo,
                .pViewportState = &this->ViewportStateInfo,
                .pRasterizationState = &this->RasterizationInfo,
                .pMultisampleState   = &MultisamplingInfo,
                .pDepthStencilState  = &this->DepthStencilInfo,
                .pColorBlendState    = &ColorBlendInfo,
                .pDynamicState       = &DynamicStateInfo,
                .layout = Layout,
                .renderPass = nullptr,
            },
            {
                .colorAttachmentCount = 1,
                .pColorAttachmentFormats = &this->Surface.GetVkSwapchainSurfaceFormat().format,
                .depthAttachmentFormat = Frontend.FindDepthFormat(),
            }
        };

        return vk::raii::Pipeline{
            Frontend.GetVkDevice(),
            nullptr,
            Chain.get<vk::GraphicsPipelineCreateInfo>()
            };
    }

    LSurface const& Surface;
    TArray<vk::raii::ShaderModule> ShaderModules;
    TArray<vk::PipelineShaderStageCreateInfo> Shaders;
    std::optional<vk::PipelineVertexInputStateCreateInfo> VertexInputInfo;
    std::optional<vk::PushConstantRange> Range;

    vk::PipelineInputAssemblyStateCreateInfo InputAssemblyInfo{
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = vk::False
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

    vk::PipelineViewportStateCreateInfo ViewportStateInfo{
        .viewportCount = 1,
        .scissorCount = 1,
        };

    vk::PipelineDepthStencilStateCreateInfo DepthStencilInfo{
        .depthTestEnable       = vk::True,
        .depthWriteEnable      = vk::True,
        .depthCompareOp        = vk::CompareOp::eLess,
        .depthBoundsTestEnable = vk::False,
        .stencilTestEnable     = vk::False
        };

    vk::PipelineColorBlendAttachmentState ColorBlendAttachment{
        .blendEnable = vk::False,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
                        | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};

    vk::Bool32 ColorBlendLogicOpEnable{ vk::False };
    vk::LogicOp ColorBlendLogicalOp{ vk::LogicOp::eCopy };

    std::array<vk::DynamicState, 2> DynamicStates{
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
        };
};

} /* ~Namespace Jafg */
