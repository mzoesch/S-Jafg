// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/FrontendForward.h"
#include "Framework/Frontend.h"
#include "Rhi/ImmutableBuffer.h"

namespace Jafg
{

struct LSlangCompilationRequest
{
    LPath In;
    LPath Out;
    LString Target{ "spirv" };
    LString Profile{ "spirv_1_5" };
    TArray<LString> IncludeDirectories;
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

struct LStageLinearImageCreateInfo
{
    void const* Data;
    vk::ImageCreateInfo Info;
};

class LFrontendVk final : public LFrontendBase
{
public:

    struct LRankedPhysicalDevice
    {
        u64 Rating;
        vk::raii::PhysicalDevice PhysicalDevice;
    };

    void Initialize(LClassOuter* Outer);
    void TearDown();

    FORCEINLINE auto const& Vk_GetContext() const noexcept { return this->Vk_Context; }

    FORCEINLINE auto const& Vk_GetAvailableInstanceExtensions() const noexcept { return this->Vk_AvailableInstanceExtensions; }
    FORCEINLINE auto const& Vk_GetRequiredInstanceExtensions() const noexcept { return this->Vk_RequiredInstanceExtensions; }
    FORCEINLINE auto&       Vk_GetMutableRequiredInstanceExtensions() noexcept { return this->Vk_RequiredInstanceExtensions; }

    FORCEINLINE auto const& Vk_GetAvailableInstanceLayers() const noexcept { return this->Vk_AvailableInstanceLayers; }
    FORCEINLINE auto const& Vk_GetRequiredInstanceLayers() const noexcept { return this->Vk_RequiredInstanceLayers; }
    FORCEINLINE auto&       Vk_GetMutableRequiredInstanceLayers() noexcept { return this->Vk_RequiredInstanceLayers; }

    FORCEINLINE auto const& Vk_GetInstance() const noexcept { return this->Vk_Instance; }

#if !IN_SHIPPING
    FORCEINLINE auto const& Vk_GetDebugUtilsMessenger() const noexcept { return this->Vk_DebugUtilsMessenger; }
#endif /* !IN_SHIPPING */

    FORCEINLINE auto const& Vk_GetAvailablePhysicalDevices() const noexcept { return this->Vk_AvailablePhysicalDevices; }
    FORCEINLINE auto const& Vk_GetPhysicalDevice() const noexcept { return this->Vk_PhysicalDevice; }
    FORCEINLINE auto const& Vk_GetPhysicalDeviceMemoryProperties() const noexcept { return this->Vk_PhysicalDeviceMemoryProperties; }

    FORCEINLINE auto Vk_GetMaxMsaaSampleCount() const noexcept { return this->Vk_MaxMsaaSampleCount; }

    FORCEINLINE auto const& Vk_GetRequiredDeviceExtensions() const noexcept { return this->Vk_RequiredDeviceExtensions; }
    FORCEINLINE auto&       Vk_GetMutableRequiredDeviceExtensions() noexcept { return this->Vk_RequiredDeviceExtensions; }
    FORCEINLINE auto const& Vk_GetDevice() const noexcept { return this->Vk_Device; }
    FORCEINLINE auto const& Vk_GetGraphicsQueue() const noexcept { return this->Vk_GraphicsQueue; }
    FORCEINLINE auto const& Vk_GetPresentQueue() const noexcept { return this->Vk_PresentQueue; }
    FORCEINLINE auto        Vk_GetGraphicsQueueFamilyIndex() const noexcept { return this->Vk_GraphicsQueueFamilyIndex; }
    FORCEINLINE auto        Vk_GetPresentQueueFamilyIndex() const noexcept { return this->Vk_PresentQueueFamilyIndex; }

    FORCEINLINE auto Vk_GetVmaAllocator() const noexcept { return this->Vk_VmaAllocator; }
    FORCEINLINE auto const& Vk_GetTransientCommandPool() const noexcept { return this->Vk_TransientCommandPool; }

    FORCEINLINE auto Vk_GetPreferredDepthFormat() const noexcept { return this->Vk_PreferredDepthFormat; }
    FORCEINLINE auto const& Vk_GetSurfaceFormat() const noexcept { check( this->Vk_SurfaceFormat.format != vk::Format::eUndefined ) return this->Vk_SurfaceFormat; }

    FORCEINLINE auto Vk_GetNumberOfFramesInFlight() const noexcept { return this->Vk_FramesInFlight; }
    void _Vk_ReportFramesInFlight(u32 FramesInFlight) noexcept
    {
        check(this->Vk_FramesInFlight == 0)
        this->Vk_FramesInFlight = FramesInFlight;
    }

    FORCEINLINE auto const& Vk_GetDefaultSampler() const noexcept { return this->Vk_DefaultSampler; }
    //# @return A descriptor pool that lives for as long the frontend lives.
    FORCEINLINE auto const& Vk_GetDescriptorPool() const noexcept { check(*this->Vk_DescriptorPool) return this->Vk_DescriptorPool; }
    FORCEINLINE auto const& Vk_GetDescriptorSetLayouts() const noexcept { return this->Vk_DescriptorSetLayouts; }
    FORCEINLINE auto&       Vk_GetMutableDescriptorSetLayouts() noexcept { return this->Vk_DescriptorSetLayouts; }
    FORCEINLINE auto const& Vk_GetImmutableBuffers() const noexcept { return this->Vk_ImmutableBuffers; }
    FORCEINLINE bool RegisterImmutableBuffers(LString const& Identifier, LImmutableBuffer&& Buffer) noexcept
    {
        if (this->Vk_ImmutableBuffers.contains(Identifier))
        {
            return false;
        }
        this->Vk_ImmutableBuffers.emplace(Identifier, std::move(Buffer));
        return true;
    }

    //# By providing no pool this method will fall back to its internal transient command pool (recommended).
    ENGINE_API vk::raii::CommandBuffer Vk_BeginSingleTimeCommands(vk::CommandPool Pool = nullptr) const;
    ENGINE_API void Vk_EndSingleTimeCommands(vk::raii::CommandBuffer CommandBuffer) const;

    //# Create any buffer through VMA.
    ENGINE_API LDeviceBuffer Vk_CreateBuffer(
          vk::BufferCreateInfo Info
        , vk::MemoryPropertyFlags Flags
        , VmaMemoryUsage Usage = VMA_MEMORY_USAGE_AUTO) const;
    ENGINE_API LDetailedDeviceBuffer Vk_CreateDetailedBuffer(
          vk::BufferCreateInfo Info
        , vk::MemoryPropertyFlags Flags
        , VmaMemoryUsage Usage = VMA_MEMORY_USAGE_AUTO) const;
    ENGINE_API LMappedDeviceBuffer Vk_CreateMappedBuffer(vk::BufferCreateInfo Info) const;

    //# By providing no pool this method will fall back to its internal transient command pool (recommended).
    ENGINE_API void Vk_CopyBuffer(vk::Buffer Src, vk::Buffer Dst, vk::BufferCopy BufferCopy, vk::CommandPool Pool = nullptr) const;

    //#
    //# Stage a buffer to the device.
    //# Usage: Frontend.Vk_StageBuffer(LStageBufferCreateInfo::<Type>({.BufferCopy = ..., .Data = ...}));
    //#
    //# @see LStageBufferCreateInfo
    //#
    ENGINE_API LDeviceBuffer Vk_StageBuffer(LStageBufferCreateInfo const& Info);

    ENGINE_API LDeviceImage Vk_CreateImage(vk::ImageCreateInfo const& Info, VmaAllocationCreateInfo const& AllocationCreateInfo);
    ENGINE_API LDeviceImage Vk_CreateDeviceLocalImage(vk::ImageCreateInfo const& Info);

    //# The resulting image will be in optimal shader read only layout.
    ENGINE_API LDeviceImage Vk_StageLinearImage(LStageLinearImageCreateInfo const& Info);

    //# Transitions an image layout. !!This is not for flight frame command buffers!!
    ENGINE_API void Vk_TransitionImageLayout(vk::ImageMemoryBarrier2 const& Barrier);

    ENGINE_API void Vk_SetSurfaceFormat(vk::SurfaceFormatKHR Format);

    //# Public private function!!! NEVER use. For internal stuff only!!!!!!!!
    ENGINE_API void _Vk_WaitIdle();

private:

    void Vk_FetchAndCheckInstanceExtensions();
    void Vk_FetchAndCheckInstanceLayers();
    void Vk_CreateInstance();
#if !IN_SHIPPING
    void Vk_SetupDebugUtilsMessenger();
#endif /* !IN_SHIPPING */
    void Vk_PickPhysicalDevice();
    void Vk_SetMaxMsaaSamples();
    void Vk_CreateLogicalDevice(LSurface const& QuerySurface);
    void Vk_CreateVma();
    void Vk_UpdateSamplers();

    TOptional<vk::Format> Vk_FindSupportedFormat(
          TArray<vk::Format> const& Candidates
        , vk::ImageTiling Tiling
        , vk::FormatFeatureFlags Features
        ) const;

    //# A rating of zero means the device is not suitable.
    std::multimap<u64, vk::raii::PhysicalDevice> Vk_RankPhysicalDevices(TArray<vk::raii::PhysicalDevice> const& PhysicalDevices) const;

    //# @note All mip levels (including zero) will be in the optimal shader read only layout after this method completes.
    void Vk_Generate2DMipMaps(vk::Image Image, vk::Format Format, vk::Extent2D Extent, u32 MipLevels);

    vk::raii::Context Vk_Context;

    TArray<vk::ExtensionProperties> Vk_AvailableInstanceExtensions;
    TArray<LString> Vk_RequiredInstanceExtensions{
        vk::KHRSurfaceExtensionName,
#if !IN_SHIPPING
        vk::EXTDebugUtilsExtensionName,
        // VK_EXT_DEVICE_ADDRESS_BINDING_REPORT_EXTENSION_NAME,
#endif /* !IN_SHIPPING */
        };

    TArray<vk::LayerProperties> Vk_AvailableInstanceLayers;
    TArray<LString> Vk_RequiredInstanceLayers;

    vk::raii::Instance Vk_Instance{ nullptr };

#if !IN_SHIPPING
    vk::raii::DebugUtilsMessengerEXT Vk_DebugUtilsMessenger{ nullptr };
#endif /* !IN_SHIPPING */

    TArray<LRankedPhysicalDevice> Vk_AvailablePhysicalDevices;
    vk::raii::PhysicalDevice Vk_PhysicalDevice{ nullptr };
    vk::PhysicalDeviceMemoryProperties Vk_PhysicalDeviceMemoryProperties;

    vk::SampleCountFlagBits Vk_MaxMsaaSampleCount{ vk::SampleCountFlagBits::e1 };

    TArray<char const*> Vk_RequiredDeviceExtensions{
        vk::KHRSwapchainExtensionName,
        vk::KHRSpirv14ExtensionName,
        vk::KHRSynchronization2ExtensionName,
        vk::KHRCreateRenderpass2ExtensionName,
        vk::EXTExtendedDynamicState3ExtensionName
        };
    vk::raii::Device Vk_Device{ nullptr };
    vk::raii::Queue Vk_GraphicsQueue{ nullptr };
    vk::raii::Queue Vk_PresentQueue{ nullptr };
    u32 Vk_GraphicsQueueFamilyIndex{ 0 };
    u32 Vk_PresentQueueFamilyIndex{ 0 };

    VmaAllocator Vk_VmaAllocator{ nullptr };
    vk::raii::CommandPool Vk_TransientCommandPool{ nullptr };

    vk::Format Vk_PreferredDepthFormat{ vk::Format::eUndefined  };
    vk::SurfaceFormatKHR Vk_SurfaceFormat{ vk::Format::eUndefined };

    u32 Vk_FramesInFlight{};

    vk::raii::Sampler Vk_DefaultSampler{ nullptr };
    vk::raii::DescriptorPool Vk_DescriptorPool{ nullptr };
    std::unordered_map<LString, vk::raii::DescriptorSetLayout> Vk_DescriptorSetLayouts;
    std::unordered_map<LString, LImmutableBuffer> Vk_ImmutableBuffers;
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

} /* ~Namespace Jafg */
