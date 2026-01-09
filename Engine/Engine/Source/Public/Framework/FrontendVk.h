// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/FrontendForward.h"
#include "Framework/Frontend.h"
#include "Rhi/VkForward.h"
#include "Rhi/VkAl.h"

namespace Jafg
{

struct LSlangCompilationRequest
{
    LPath In;
    LPath Out;
    LString Target{ "spirv" };
    LString Profile{ "spirv_1_5" };
    TArray<LString> EntryPoints;
};

struct LStageBufferCreateInfo
{
    vk::BufferCopy BufferCopy;
    void const* Data;
    vk::BufferUsageFlags Usage;

    struct LGenericBufferCreateInfo
    {
        vk::BufferCopy BufferCopy;
        void const* Data;
    };

    typedef LGenericBufferCreateInfo LVertexCreateInfo;
    typedef LGenericBufferCreateInfo LIndexCreateInfo;
    typedef LGenericBufferCreateInfo LUniformCreateInfo;

    FORCEINLINE static LStageBufferCreateInfo Vertex(LVertexCreateInfo const& Info) noexcept;
    FORCEINLINE static LStageBufferCreateInfo Index(LIndexCreateInfo const& Info) noexcept;
    FORCEINLINE static LStageBufferCreateInfo Uniform(LUniformCreateInfo const& Info) noexcept;
};

class LFrontendVk final : public LFrontendBase
{
public:

    void Initialize(LClassOuter* Outer);
    void TearDown();

    FORCEINLINE auto const& GetVkContext() const noexcept { return this->VkMyContext; }
    FORCEINLINE auto const& GetVkInstance() const noexcept { return this->VkMyInstance; }
    FORCEINLINE auto const& GetVkPhysicalDevice() const noexcept { return this->Vk_PhysicalDevice; }
    FORCEINLINE auto const& GetVkDevice() const noexcept { return this->VkMyDevice; }
    FORCEINLINE auto        GetVkGraphicsQueueFamilyIndex() const noexcept { return this->VkMyGraphicsQueueFamilyIndex; }
    FORCEINLINE auto        GetVkPresentQueueFamilyIndex() const noexcept { return this->VkMyPresentQueueFamilyIndex; }
    FORCEINLINE auto const& GetVkGraphicsQueue() const noexcept { return this->VkMyGraphicsQueue; }
    FORCEINLINE auto const& GetVkPresentQueue() const noexcept { return this->VkMyPresentQueue; }

    FORCEINLINE auto const& GetVma() const noexcept { return this->VmaMyAllocator; }

    ENGINE_API vk::raii::CommandBuffer VkBeginSingleTimeCommands(vk::CommandPool Pool) const;
    ENGINE_API void VkEndSingleTimeCommands(vk::CommandBuffer CommandBuffer) const;

    //# Create any buffer through VMA.
    ENGINE_API LDeviceBuffer Vk_CreateBuffer(
          vk::BufferCreateInfo Info
        , vk::MemoryPropertyFlags Flags
        , VmaMemoryUsage Usage = VMA_MEMORY_USAGE_AUTO) const;
    ENGINE_API LDetailedDeviceBuffer Vk_CreateDetailedBuffer(
          vk::BufferCreateInfo Info
        , vk::MemoryPropertyFlags Flags
        , VmaMemoryUsage Usage = VMA_MEMORY_USAGE_AUTO) const;
    ENGINE_API LMappedDeviceBuffer Vk_CreateMappedBuffer(
          vk::BufferCreateInfo Info
        , vk::MemoryPropertyFlags Flags
        , VmaAllocationCreateFlags VmaFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
        , VmaMemoryUsage Usage = VMA_MEMORY_USAGE_AUTO) const;

    //# By providing no pool this method will fall back to its internal transient command pool.
    ENGINE_API void Vk_CopyBuffer(vk::Buffer Src, vk::Buffer Dst, vk::BufferCopy BufferCopy, vk::CommandPool Pool = nullptr) const;

    //#
    //# Stage a buffer to the device.
    //# Usage: Frontend.Vk_StageBuffer(LStageBufferCreateInfo::<Type>({.BufferCopy = ..., .Data = ...}));
    //#
    //# @see LStageBufferCreateInfo
    //#
    ENGINE_API LDeviceBuffer Vk_StageBuffer(LStageBufferCreateInfo const& Info);










    ENGINE_API LDeviceImage VkCreateImage(vk::ImageCreateInfo const& Info, VmaAllocationCreateInfo const& AllocationCreateInfo);

    ENGINE_API LDeviceImage VkStage2dImage(
          i32 texWidth, i32 texHeight, i32 texChannels
        , stbi_uc* pixels
        , u32 MipLevels
        , vk::CommandPool Pool /* just temp... */
        );

    ENGINE_API void CopyBufferToImage(vk::Buffer Buffer, vk::Image Image, u32 Width, u32 Height, vk::CommandPool Pool /* just temp... */);
    ENGINE_API LDeviceImage VkCreateDeviceLocalImage(vk::ImageCreateInfo const& InInfo, vk::MemoryPropertyFlags Properties = vk::MemoryPropertyFlagBits::eDeviceLocal);

    ENGINE_API void VkTransitionImageLayout(vk::Image Image, vk::ImageLayout OldLayout, vk::ImageLayout NewLayout, u32 MipLevels
        , vk::CommandPool Pool /* just temp... */);

    // DEPRECATED
    ENGINE_API vk::raii::ImageView CreateImageView2D(
          vk::Image Image
        , vk::Format Format
        , vk::ImageAspectFlags AspectFlags
        , u32 MipLevels
        );


    FORCEINLINE vk::raii::ImageView CreateImageView(vk::ImageViewCreateInfo const& Info) const noexcept{ return vk::raii::ImageView{this->VkMyDevice, Info}; }


    ENGINE_API vk::Format FindSupportedFormat(
          TArray<vk::Format> const& Candidates
        , vk::ImageTiling Tiling
        , vk::FormatFeatureFlags Features
        ) const;
    ENGINE_API vk::Format FindDepthFormat() const;

    ENGINE_API bool HasStencilComponent(vk::Format Format) const;

    vk::SampleCountFlagBits CalculateMaxUsableSampleCount() const;
    FORCEINLINE vk::SampleCountFlagBits GetMaxMsaaSamples() const noexcept { return this->VkMsaaSamples; }

    ENGINE_API u32 Vk_FindMemoryType(u32 Filter, vk::MemoryPropertyFlags Properties) const;

    FORCEINLINE auto const& Vk_GetTransientCommandPool() const noexcept { return this->Vk_TransientCommandPool; }

    ENGINE_API i64 HandleCompilationRequest_viaSlang(LSlangCompilationRequest const& Request);
    ENGINE_API i64 HandleCompilationRequest_viaSlang(LPath const& Slang, LSlangCompilationRequest const& Request);

private:

    void FetchAndCheckInstanceExtensions();
    void FetchAndCheckInstanceLayers();
    void CreateInstance();
#if !IN_SHIPPING
    void SetupDebugUtilsMessenger();
#endif /* !IN_SHIPPING */
    void PickPhysicalDevice();
    void PickMaxMsaaSamples();
    void CreateLogicalDevice();
    void CreateVma();

    //# A rating of zero means the device is not suitable.
    std::multimap<u64, vk::raii::PhysicalDevice> RankPhysicalDevices(TArray<vk::raii::PhysicalDevice> const& PhysicalDevices) const;

    vk::raii::Context VkMyContext;
    vk::raii::Instance VkMyInstance{ nullptr };

    TArray<vk::ExtensionProperties> AvailableInstanceExtensions;
    TArray<LString> RequiredInstanceExtensions{
        vk::KHRSurfaceExtensionName,
#if !IN_SHIPPING
        vk::EXTDebugUtilsExtensionName,
        // VK_EXT_DEVICE_ADDRESS_BINDING_REPORT_EXTENSION_NAME,
#endif /* !IN_SHIPPING */
        };

    TArray<vk::LayerProperties> AvailableInstanceLayers;
    TArray<LString> RequiredInstanceLayers{
#if !IN_SHIPPING
        "VK_LAYER_KHRONOS_validation",
#endif /* !IN_SHIPPING */
        };

    vk::raii::DebugUtilsMessengerEXT VkMyDebugUtilsMessenger{ nullptr };

    TArray<vk::raii::PhysicalDevice> AvailablePhysicalDevices;
    vk::raii::PhysicalDevice Vk_PhysicalDevice{ nullptr };
    vk::PhysicalDeviceMemoryProperties Vk_PhysicalDeviceMemoryProperties;

    TArray<char const*> RequiredDeviceExtensions{
        vk::KHRSwapchainExtensionName,
        vk::KHRSpirv14ExtensionName,
        vk::KHRSynchronization2ExtensionName,
        vk::KHRCreateRenderpass2ExtensionName,
        };
    vk::raii::Device VkMyDevice{ nullptr };

    u32 VkMyGraphicsQueueFamilyIndex{ 0 };
    u32 VkMyPresentQueueFamilyIndex{ 0 };
    vk::raii::Queue VkMyGraphicsQueue{ nullptr };
    vk::raii::Queue VkMyPresentQueue{ nullptr };

    VmaAllocator VmaMyAllocator{ nullptr };

    vk::SampleCountFlagBits VkMsaaSamples{ vk::SampleCountFlagBits::e1 };

    vk::raii::CommandPool Vk_TransientCommandPool{ nullptr };
};

FORCEINLINE LStageBufferCreateInfo LStageBufferCreateInfo::Vertex(LVertexCreateInfo const& Info) noexcept
{
    return {
        .BufferCopy = Info.BufferCopy,
        .Data = Info.Data,
        .Usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        };
}

FORCEINLINE LStageBufferCreateInfo LStageBufferCreateInfo::Index(LIndexCreateInfo const& Info) noexcept
{
    return {
        .BufferCopy = Info.BufferCopy,
        .Data = Info.Data,
        .Usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        };
}

FORCEINLINE LStageBufferCreateInfo LStageBufferCreateInfo::Uniform(LUniformCreateInfo const& Info) noexcept
{
    return {
        .BufferCopy = Info.BufferCopy,
        .Data = Info.Data,
        .Usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer,
        };
}

inline LDevicePipeline LDevicePipelineFactory::Build()
{
    auto& Frontend{ this->Surface.GetFrontend() };

    vk::PipelineMultisampleStateCreateInfo MultisamplingInfo{
        .rasterizationSamples = this->Surface.GetFrontend().GetMaxMsaaSamples(),
        .sampleShadingEnable = this->MultisamplingShadingEnable
        };

    vk::PipelineColorBlendStateCreateInfo ColorBlendInfo{
        .logicOpEnable = this->ColorBlendLogicOpEnable,
        .logicOp = this->ColorBlendLogicalOp,
        .attachmentCount = 1,
        .pAttachments = &this->ColorBlendAttachmentState
        };

    vk::PipelineDynamicStateCreateInfo DynamicStateInfo{
        .dynamicStateCount = static_cast<u32>(this->DynamicStateInfo.size()),
        .pDynamicStates = this->DynamicStateInfo.data(),
        };

    vk::raii::PipelineLayout Layout{
        Frontend.GetVkDevice(),
        vk::PipelineLayoutCreateInfo{
            .setLayoutCount = (*this->DescriptorSetLayout) ? 1u : 0u,
            .pSetLayouts = (*this->DescriptorSetLayout) ? &*this->DescriptorSetLayout : nullptr,
            .pushConstantRangeCount = static_cast<uint32_t>(this->Range.has_value() ? 1uz : 0uz),
            .pPushConstantRanges = this->Range.has_value() ? &*this->Range : nullptr
            }
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
            .pColorAttachmentFormats = &this->Surface.Vk_GetSurfaceFormat().format,
            .depthAttachmentFormat = Frontend.FindDepthFormat(),
        }
    };

    auto Pipeline{vk::raii::Pipeline{
        Frontend.GetVkDevice(),
        nullptr,
        Chain.get<vk::GraphicsPipelineCreateInfo>()
        }};

    return LDevicePipeline{
        .Pipeline = std::move(Pipeline),
        .PipelineLayout = std::move(Layout),
        .DescriptorSetLayout = std::move(this->DescriptorSetLayout),
        };
}

} /* ~Namespace Jafg */
