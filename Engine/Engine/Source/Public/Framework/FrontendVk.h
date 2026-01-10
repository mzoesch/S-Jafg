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

    FORCEINLINE auto Vk_GetMaxMsaaSamples() const noexcept { return this->Vk_MaxMsaaSamples; }

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
    ENGINE_API LMappedDeviceBuffer Vk_CreateMappedBuffer(
          vk::BufferCreateInfo Info
        , vk::MemoryPropertyFlags Flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        , VmaAllocationCreateFlags VmaFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
        , VmaMemoryUsage Usage = VMA_MEMORY_USAGE_AUTO) const;

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

    ENGINE_API i64 HandleSlangCompilationRequest(LSlangCompilationRequest const& Request);
    ENGINE_API i64 HandleSlangCompilationRequest(LPath const& Slang, LSlangCompilationRequest const& Request);

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
    TArray<LString> Vk_RequiredInstanceLayers{
#if !IN_SHIPPING
        "VK_LAYER_KHRONOS_validation",
#endif /* !IN_SHIPPING */
        };

    vk::raii::Instance Vk_Instance{ nullptr };

#if !IN_SHIPPING
    vk::raii::DebugUtilsMessengerEXT Vk_DebugUtilsMessenger{ nullptr };
#endif /* !IN_SHIPPING */

    TArray<LRankedPhysicalDevice> Vk_AvailablePhysicalDevices;
    vk::raii::PhysicalDevice Vk_PhysicalDevice{ nullptr };
    vk::PhysicalDeviceMemoryProperties Vk_PhysicalDeviceMemoryProperties;

    vk::SampleCountFlagBits Vk_MaxMsaaSamples{ vk::SampleCountFlagBits::e1 };

    TArray<char const*> Vk_RequiredDeviceExtensions{
        vk::KHRSwapchainExtensionName,
        vk::KHRSpirv14ExtensionName,
        vk::KHRSynchronization2ExtensionName,
        vk::KHRCreateRenderpass2ExtensionName,
        };
    vk::raii::Device Vk_Device{ nullptr };
    vk::raii::Queue Vk_GraphicsQueue{ nullptr };
    vk::raii::Queue Vk_PresentQueue{ nullptr };
    u32 Vk_GraphicsQueueFamilyIndex{ 0 };
    u32 Vk_PresentQueueFamilyIndex{ 0 };

    VmaAllocator Vk_VmaAllocator{ nullptr };
    vk::raii::CommandPool Vk_TransientCommandPool{ nullptr };

    vk::Format Vk_PreferredDepthFormat{ vk::Format::eUndefined  };
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
