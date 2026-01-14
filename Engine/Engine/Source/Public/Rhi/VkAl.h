// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"
#include "Rhi/VkForward.h"
#include "Rhi/RhiVendorInclude.h"
#include "Framework/FrontendForward.h"

//#
//# File:
//#     Vulkan Abstraction Layer
//#
#if PREPROCESSOR_EXCLUDE_FF
#endif /* PREPROCESSOR_EXCLUDE_FF */

namespace Jafg
{

struct LRenderInfo;
struct LGraphicsDevicePipeline;

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
    static_assert(sizeof(T) == POINTER_BYTE_SIZE);

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

    FORCEINLINE constexpr T GetBuffer() const noexcept { return this->Buffer; }
    FORCEINLINE constexpr LDeviceAllocation GetAllocation() const noexcept { return this->Allocation; }

    constexpr inline void Release() noexcept
    {
        this->Buffer = nullptr;
        this->Allocation = nullptr;
    }

    inline void Free() noexcept
    {
        Detail::FreeDeviceAllocation(this->Buffer, this->Allocation);
        this->Buffer = nullptr;
        this->Allocation = nullptr;
    }

private:

    T Buffer;
    LDeviceAllocation Allocation;
};

typedef TGenericDeviceBuffer<vk::Buffer> LDeviceBuffer;
typedef TGenericDeviceBuffer<vk::Image> LDeviceImage;

//# A device buffer with detailed allocation info.
struct LDetailedDeviceBuffer final : private LDeviceBuffer
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

    FORCEINLINE constexpr vk::Buffer GetBuffer() const noexcept { return LDeviceBuffer::GetBuffer(); }
    FORCEINLINE constexpr LDeviceAllocation GetAllocation() const noexcept { return LDeviceBuffer::GetAllocation(); }
    FORCEINLINE constexpr LDeviceAllocationInfo const& GetAllocationInfo() const noexcept { return this->Info; }

    constexpr inline void Release() noexcept
    {
        LDeviceBuffer::Release();
        this->Info = {};
    }

    inline void Free() noexcept
    {
        LDeviceBuffer::Free();
        this->Info = {};
    }

private:

    LDeviceAllocationInfo Info;
};

//# A device buffer that is mapped to host visible memory.
struct LMappedDeviceBuffer final : private LDeviceBuffer
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

    FORCEINLINE constexpr vk::Buffer GetBuffer() const noexcept { return LDeviceBuffer::GetBuffer(); }
    FORCEINLINE constexpr LDeviceAllocation GetAllocation() const noexcept { return LDeviceBuffer::GetAllocation(); }
    FORCEINLINE constexpr void* GetData() const noexcept { return this->Data; }

    constexpr inline void Release() noexcept
    {
        LDeviceBuffer::Release();
        this->Data = nullptr;
    }

    inline void Free() noexcept
    {
        LDeviceBuffer::Free();
        this->Data = nullptr;
    }

private:

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
concept CDeviceVertexInput = std::is_standard_layout_v<T> && requires
{
    { T::BindingDescriptions().data() } -> std::same_as<vk::VertexInputBindingDescription const*>;
    { T::BindingDescriptions().size() } -> std::same_as<LSize>;
    { T::AttributeDescriptions().data() } -> std::same_as<vk::VertexInputAttributeDescription const*>;
    { T::AttributeDescriptions().size() } -> std::same_as<LSize>;
};

template<typename T>
concept CDeviceLayout = std::is_standard_layout_v<T> && requires
{
    { T::Bindings().data() } -> std::same_as<vk::DescriptorSetLayoutBinding const*>;
    { T::Bindings().size() } -> std::same_as<LSize>;
};

template<typename T>
concept CUniformBufferObject = std::is_standard_layout_v<T> && requires
{
    // { T::Binding() } -> std::same_as<u32>;
    { T::Flags() } -> std::same_as<vk::ShaderStageFlags>;
};

//# Concept for push constant structures. It must fulfill these requirements.
template<typename T>
concept CPushConstant = std::is_standard_layout_v<T> && requires
{
    { T::Flags() } -> std::same_as<vk::ShaderStageFlags>;
};

//#
//# A generic push constant structure.
//# Inherit from this to create push constant structures for specific shader stages.
//#
template<typename T>
struct TPushConstant
{
    inline void Push(LRenderInfo const& Info, LGraphicsDevicePipeline const& Pipeline, u32 Offset = 0) noexcept;
};

//# Push constant structure for vertex shaders.
template<typename T>
struct TVertexPushConstant : public TPushConstant<T>
{
    static vk::ShaderStageFlags Flags() noexcept { return vk::ShaderStageFlagBits::eVertex; }
};

//# Push constant structure for fragment shaders.
template<typename T>
struct TFragmentPushConstant : public TPushConstant<T>
{
    static vk::ShaderStageFlags Flags() noexcept { return vk::ShaderStageFlagBits::eFragment; }
};

struct LGraphicsDevicePipeline
{
    void Free() noexcept
    {
        this->Pipeline.clear();
        this->Layout.clear();
        this->DescriptorSetLayout.clear();
    }

    inline decltype(auto) operator*() const & noexcept { return *this->Pipeline; }

    vk::raii::Pipeline Pipeline{ nullptr };
    vk::raii::PipelineLayout Layout{ nullptr };
    vk::raii::DescriptorSetLayout DescriptorSetLayout{ nullptr };
};

struct LDevicePipelineFactory
{
    explicit LDevicePipelineFactory(LFrontend const& InFrontend) noexcept : Frontend{ InFrontend } {}

    PROHIBIT_REALLOC_OF_ANY_FORM(LDevicePipelineFactory)

    decltype(auto) Shader(this auto&& Self, LPath const& Path, vk::ShaderStageFlags Stages)
    {
        const auto Code{ Finder::ReadFileAsBinary(Path) };
        Self.ShaderModules.emplace_back(vk::raii::ShaderModule{
            Self.Frontend.Vk_GetDevice(),
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
            Self.Frontend.Vk_GetDevice(),
            vk::DescriptorSetLayoutCreateInfo{
                .bindingCount = static_cast<u32>(TDeviceLayout::Bindings().size()),
                .pBindings = TDeviceLayout::Bindings().data(),
                }
            };

        return std::forward<decltype(Self)>(Self);
    }

    template<CPushConstant TPushConstant>
    decltype(auto) Push(this auto&& Self) noexcept
    {
        check( Self.Range.has_value() == false )
        Self.Range = vk::PushConstantRange{
            .stageFlags = TPushConstant::Flags(),
            .offset = 0,
            .size = sizeof(TPushConstant),
            };
        return std::forward<decltype(Self)>(Self);
    }

    ENGINE_API LGraphicsDevicePipeline Build();

    LFrontend const& Frontend;
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

inline LSize Vk_GetChannelsPerPixel(vk::Format Format) noexcept
{
    switch (Format)
    {
    case vk::Format::eR8G8B8A8Srgb:
    {
        return 4;
    }
    default:
    {
        panicMsgf( "Unsupported or non-linear format [{}] for channels per pixel query.", vk::to_string(Format) )
    }
    }
}

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
