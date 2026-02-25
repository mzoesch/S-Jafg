// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
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

typedef vk::PipelineVertexInputStateCreateInfo(*LDeviceVertexInputStateCreateProviderSig)();

template<typename T> requires CDeviceVertexInput<T>
struct TDeviceVertexInputStateCreateProviderFn
{
    static vk::PipelineVertexInputStateCreateInfo operator()() noexcept
    {
        return vk::PipelineVertexInputStateCreateInfo{
            .vertexBindingDescriptionCount = static_cast<u32>(T::BindingDescriptions().size()),
            .pVertexBindingDescriptions = T::BindingDescriptions().data(),
            .vertexAttributeDescriptionCount = static_cast<u32>(T::AttributeDescriptions().size()),
            .pVertexAttributeDescriptions = T::AttributeDescriptions().data(),
            };
    }
};
template<typename T> requires CDeviceVertexInput<T>
inline constexpr TDeviceVertexInputStateCreateProviderFn<T> DeviceVertexInputStateCreateProvider{};

namespace Detail
{

extern void AddVertexProviderImpl(LString Name, LDeviceVertexInputStateCreateProviderSig Sig) noexcept;
template<typename TVertexInput> requires CDeviceVertexInput<TVertexInput>
inline void AddVertexProvider() noexcept
{
    AddVertexProviderImpl(LString{GetTypeName<TVertexInput>()}, &DeviceVertexInputStateCreateProvider<TVertexInput>.operator());
}

} /* ~Namespace Detail */

template<typename T> requires CDeviceVertexInput<T>
struct LRegisterDeviceVertexInput final
{
    inline LRegisterDeviceVertexInput() noexcept
    {
        Detail::AddVertexProvider<T>();
    }
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

namespace Detail
{

struct LPushConstantInfo
{
    vk::ShaderStageFlags StageFlags;
    u32 Size;
};

} /* ~Namespace Detail */

typedef Detail::LPushConstantInfo(*LPushConstantProviderSig)();

template<typename T> requires CPushConstant<T>
struct TPushConstantProviderFn
{
    static Detail::LPushConstantInfo operator()() noexcept
    {
        return Detail::LPushConstantInfo{
            .StageFlags = T::Flags(),
            .Size = sizeof(T)
            };
    }
};
template<typename T> requires CPushConstant<T>
inline constexpr TPushConstantProviderFn<T> PushConstantProvider{};

namespace Detail
{

extern void AddPushConstantProviderImpl(LString Name, LPushConstantProviderSig Sig) noexcept;
template<typename TPushConstant> requires CPushConstant<TPushConstant>
inline void AddPushConstantProvider() noexcept
{
    AddPushConstantProviderImpl(LString{GetTypeName<TPushConstant>()}, &PushConstantProvider<TPushConstant>.operator());
}

} /* ~Namespace Detail */

template<typename T> requires CPushConstant<T>
struct LRegisterPushConstant final
{
    inline LRegisterPushConstant() noexcept
    {
        Detail::AddPushConstantProvider<T>();
    }
};

struct LGraphicsDevicePipeline
{
    void Free() noexcept
    {
        this->Pipeline.clear();
        this->Layout.clear();
        algo::orphan(&this->DescriptorSetLayouts);
        algo::orphan(&this->_UniqueDescriptorSetLayout);
    }

    inline decltype(auto) operator*() const & noexcept { return *this->Pipeline; }

    vk::raii::Pipeline Pipeline{ nullptr };
    vk::raii::PipelineLayout Layout{ nullptr };

    TArray<vk::DescriptorSetLayout> DescriptorSetLayouts;
    TArray<vk::raii::DescriptorSetLayout> _UniqueDescriptorSetLayout;
};

struct LShaderEntrypoint
{
    vk::ShaderStageFlagBits Stage;
    LString Name;
};

struct LDevicePipelineFactory
{
    explicit LDevicePipelineFactory(LFrontend const& InFrontend) noexcept : Frontend{ InFrontend } {}

    PROHIBIT_REALLOC_OF_ANY_FORM(LDevicePipelineFactory)

    decltype(auto) Shader(this auto&& Self, LPath const& Path, TArray<LShaderEntrypoint> InEntrypoints)
    {
        check(Self.ShaderEntrypoints.contains(Path) == false)
        Self.ShaderEntrypoints[Path] = std::move(InEntrypoints);
        auto& Entrypoints{Self.ShaderEntrypoints[Path]};

        const auto Code{Finder::ReadFileAsBinary(Path)};
        Self.ShaderModules.emplace_back(vk::raii::ShaderModule{
            Self.Frontend.Vk_GetDevice(),
            vk::ShaderModuleCreateInfo{
                .codeSize = Code.size() * sizeof(u8),
                .pCode = reinterpret_cast<u32 const*>(Code.data())
                }
            });
        auto ShaderModuleHandle{*Self.ShaderModules.back()};

        vk::ShaderStageFlags Stages{};
        algo::for_each(Entrypoints, [&Stages](auto const& Entrypoint)
        {
            Stages |= Entrypoint.Stage;
        });

        if (Stages & vk::ShaderStageFlagBits::eVertex)
        {
            Self.Shaders.emplace_back(vk::PipelineShaderStageCreateInfo{
                .stage = vk::ShaderStageFlagBits::eVertex,
                .module = ShaderModuleHandle,
                .pName = algo::find(Entrypoints, vk::ShaderStageFlagBits::eVertex, &LShaderEntrypoint::Stage)->Name.c_str(),
                });
        }

        if (Stages & vk::ShaderStageFlagBits::eFragment)
        {
            Self.Shaders.emplace_back(vk::PipelineShaderStageCreateInfo{
                .stage = vk::ShaderStageFlagBits::eFragment,
                .module = ShaderModuleHandle,
                .pName = algo::find(Entrypoints, vk::ShaderStageFlagBits::eFragment, &LShaderEntrypoint::Stage)->Name.c_str(),
                });
        }

        check( (Stages & ~(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)) == vk::ShaderStageFlags{}
            && "These stage flags are not yet supported." )

        return std::forward<decltype(Self)>(Self);
    }

    template<CDeviceVertexInput TDeviceVertexInput>
    decltype(auto) VertexInput(this auto&& Self) noexcept
    {
        check(Self.VertexInputInfo.has_value() == false)
        Self.VertexInputInfo = {
            .vertexBindingDescriptionCount = static_cast<u32>(TDeviceVertexInput::BindingDescriptions().size()),
            .pVertexBindingDescriptions = TDeviceVertexInput::BindingDescriptions().data(),
            .vertexAttributeDescriptionCount = static_cast<u32>(TDeviceVertexInput::AttributeDescriptions().size()),
            .pVertexAttributeDescriptions = TDeviceVertexInput::AttributeDescriptions().data(),
            };

        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) VertexInput(this auto&& Self, vk::PipelineVertexInputStateCreateInfo&& Info)
    {
        check(Self.VertexInputInfo.has_value() == false)
        Self.VertexInputInfo = std::move(Info);
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

    decltype(auto) SharedLayout(this auto&& Self, vk::DescriptorSetLayout SharedDescriptorSetLayout) noexcept
    {
        Self.SharedDescriptorSetLayouts.emplace_back(Self.GetCurrentNumberOfLayouts(), SharedDescriptorSetLayout);
        return std::forward<decltype(Self)>(Self);
    }

    template<CDeviceLayout TDeviceLayout>
    decltype(auto) UniqueLayout(this auto&& Self) noexcept
    {
        Self.UniqueDescriptorSetLayouts.emplace_back(Self.GetCurrentNumberOfLayouts(), vk::raii::DescriptorSetLayout{
            Self.Frontend.Vk_GetDevice(),
            vk::DescriptorSetLayoutCreateInfo{
                .bindingCount = static_cast<u32>(TDeviceLayout::Bindings().size()),
                .pBindings = TDeviceLayout::Bindings().data(),
                }
            });

        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) UniqueLayout(this auto&& Self, vk::DescriptorSetLayoutCreateInfo const& Layout) noexcept
    {
        Self.UniqueDescriptorSetLayouts.emplace_back(Self.GetCurrentNumberOfLayouts(), vk::raii::DescriptorSetLayout{
            Self.Frontend.Vk_GetDevice(),
            Layout
            });
        return std::forward<decltype(Self)>(Self);
    }

    template<CPushConstant TPushConstant>
    decltype(auto) PushConstant(this auto&& Self) noexcept
    {
        Self.PushConstantRange.emplace_back(vk::PushConstantRange{
            .stageFlags = TPushConstant::Flags(),
            .offset = static_cast<u32>(Self.PushConstantRange.size()),
            .size = sizeof(TPushConstant),
            });
        return std::forward<decltype(Self)>(Self);
    }

    decltype(auto) PushConstant(this auto&& Self, Detail::LPushConstantInfo const& Info) noexcept
    {
        Self.PushConstantRange .emplace_back(vk::PushConstantRange{
            .stageFlags = Info.StageFlags,
            .offset = static_cast<u32>(Self.PushConstantRange.size()),
            .size = Info.Size
            });
        return std::forward<decltype(Self)>(Self);
    }

    ENGINE_API LGraphicsDevicePipeline Build();

    inline u32 GetCurrentNumberOfLayouts() const noexcept
    {
        return static_cast<u32>(this->SharedDescriptorSetLayouts.size() + this->UniqueDescriptorSetLayouts.size());
    }

    template<typename T>
    struct TDescriptorSetLayout
    {
        u64 Binding;
        T DescriptorSetLayout;
    };

    LFrontend const& Frontend;
    TArray<vk::raii::ShaderModule> ShaderModules;
    TArray<vk::PipelineShaderStageCreateInfo> Shaders;
    std::unordered_map<LPath, TArray<LShaderEntrypoint>> ShaderEntrypoints;
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
    TArray<TDescriptorSetLayout<vk::DescriptorSetLayout>> SharedDescriptorSetLayouts;
    TArray<TDescriptorSetLayout<vk::raii::DescriptorSetLayout>> UniqueDescriptorSetLayouts;
    vk::raii::PipelineLayout PipelineLayout{ nullptr };
    TArray<vk::PushConstantRange> PushConstantRange;
};

inline vk::Format Vk_StringToFormat(LStringView String) noexcept
{
    if (String == "eR8G8B8A8Srgb") { return vk::Format::eR8G8B8A8Srgb; }
    if (String == "eR8G8B8Srgb") { return vk::Format::eR8G8B8Srgb; }

    return vk::Format::eUndefined;
}

inline vk::SampleCountFlagBits Vk_StringToSampleCountFlagBits(LStringView String) noexcept
{
    if (String == "e1") { return vk::SampleCountFlagBits::e1; }
    if (String == "e2") { return vk::SampleCountFlagBits::e2; }
    if (String == "e4") { return vk::SampleCountFlagBits::e4; }
    if (String == "e8") { return vk::SampleCountFlagBits::e8; }
    if (String == "e16") { return vk::SampleCountFlagBits::e16; }
    if (String == "e32") { return vk::SampleCountFlagBits::e32; }
    if (String == "e64") { return vk::SampleCountFlagBits::e64; }

    LOG_FATAL(LogVulkan, "Unsupported sample count string [{}] for conversion to sample count flag bits.", String)
}

inline vk::ShaderStageFlagBits Vk_StringToShaderStageFlagBits(LStringView String) noexcept
{
    if (String == "eVertex") { return vk::ShaderStageFlagBits::eVertex; }
    if (String == "eFragment") { return vk::ShaderStageFlagBits::eFragment; }

    LOG_FATAL(LogVulkan, "Unsupported shader stage string [{}] for conversion to shader stage flag bits.", String)
}

inline vk::DescriptorType Vk_StringToDescriptorType(LStringView String) noexcept
{
    if (String == "eCombinedImageSampler") { return vk::DescriptorType::eCombinedImageSampler; }

    LOG_FATAL(LogVulkan, "Unsupported descriptor type string [{}] for conversion to descriptor type.", String)
}

inline constexpr LSize Vk_GetChannelsPerPixel(vk::Format Format) noexcept
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
        panicMsgf( "Unsupported or non-linear format [{}] for channels per pixel query.", vk::to_string(Format) )
    }
    }
}

inline constexpr LSize Vk_GetBytesPerPixel(vk::Format Format) noexcept
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
        panicMsgf( "Unsupported or non-linear format [{}] for bytes per pixel query.", vk::to_string(Format) )
    }
    }
}

inline constexpr vk::SampleCountFlagBits Vk_GetMaxMsaaSamples(vk::SampleCountFlags Counts) noexcept
{
    if (Counts & vk::SampleCountFlagBits::e64)
    {
        return vk::SampleCountFlagBits::e64;
    }
    if (Counts & vk::SampleCountFlagBits::e32)
    {
        return vk::SampleCountFlagBits::e32;
    }
    if (Counts & vk::SampleCountFlagBits::e16)
    {
        return vk::SampleCountFlagBits::e16;
    }
    if (Counts & vk::SampleCountFlagBits::e8)
    {
        return vk::SampleCountFlagBits::e8;
    }
    if (Counts & vk::SampleCountFlagBits::e4)
    {
        return vk::SampleCountFlagBits::e4;
    }
    if (Counts & vk::SampleCountFlagBits::e2)
    {
        return vk::SampleCountFlagBits::e2;
    }
    if (Counts & vk::SampleCountFlagBits::e1)
    {
        return vk::SampleCountFlagBits::e1;
    }

    panicMsgf("Unsupported sample count flags [{}] for max MSAA samples query.", vk::to_string(Counts))
}

} /* ~Namespace Jafg */
