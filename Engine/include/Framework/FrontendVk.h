// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Frontend.h"
#if JAFG_PLATFORM_DESKTOP
    #include "Framework/FrontendNativeDesktop.h"
#endif /* JAFG_PLATFORM_DESKTOP */
#include "Rhi/RendererCore.h"
#include "Rhi/Bindless.h"

namespace Jafg
{

struct LTexture2;

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

class LFrontendVk final :
#if JAFG_PLATFORM_DESKTOP
    public LFrontendNativeDesktop
#else /* JAFG_PLATFORM_DESKTOP */
    public LFrontendBase
#endif /* !JAFG_PLATFORM_DESKTOP */
{
public:

    struct LRankedPhysicalDevice
    {
        u64 Rating;
        vk::raii::PhysicalDevice PhysicalDevice;
    };

    void Initialize(LClassOuter* Outer);
    void TearDown();

    //# TODO: Remove this... this has nothing to do with VK.
    NODISCARD ENGINE_API std::optional<LPhysicalKey> GetPhysicalKey(ELogicalKey LogicalKey) const noexcept;
    NODISCARD ENGINE_API std::optional<LString> Glfw_GetPhysicalKeyLocalizedRepr(LPhysicalKey Key, i32 Hint) const noexcept;
    LString Glfw_GetSanitizedPhysicalKeyLocalizedRepr(LPhysicalKey Key, i32 Hint) const noexcept
    {
        if (auto Result{this->Glfw_GetPhysicalKeyLocalizedRepr(Key, Hint)}; Result.has_value())
        {
            return std::move(Result).value();
        }
        return algo::sprintf("SC: {}", Key.Scancode);
    }

    ENGINE_API void _RefreshUsablePhysicalViewports();

    NODISCARD FORCEINLINE rhi::framework Vk_GetFramework() const noexcept { return this->Vk_Framework; }
    NODISCARD ENGINE_API std::optional<rhi::present_mode> Vk_GetFirstSurfacePresentMode() const noexcept;

    FORCEINLINE auto const& Vk_GetContext() const noexcept { return this->Vk_Context; }

    FORCEINLINE auto const& Vk_GetAvailableInstanceExtensions() const noexcept { return this->Vk_AvailableInstanceExtensions; }
    FORCEINLINE auto const& Vk_GetRequiredInstanceExtensions() const noexcept { return this->Vk_RequiredInstanceExtensions; }
    FORCEINLINE auto&       Vk_GetMutableRequiredInstanceExtensions() noexcept { return this->Vk_RequiredInstanceExtensions; }

    FORCEINLINE auto const& Vk_GetAvailableInstanceLayers() const noexcept { return this->Vk_AvailableInstanceLayers; }
    FORCEINLINE auto const& Vk_GetRequiredInstanceLayers() const noexcept { return this->Vk_RequiredInstanceLayers; }
    FORCEINLINE auto&       Vk_GetMutableRequiredInstanceLayers() noexcept { return this->Vk_RequiredInstanceLayers; }

    FORCEINLINE auto const& Vk_GetInstance() const noexcept { return this->Vk_Instance; }

#if !JAFG_IN_SHIPPING
    FORCEINLINE auto const& Vk_GetDebugUtilsMessenger() const noexcept { return this->Vk_DebugUtilsMessenger; }
#endif /* !JAFG_IN_SHIPPING */

    NODISCARD FORCEINLINE auto const& Vk_GetAvailablePhysicalDevices() const noexcept { return this->Vk_AvailablePhysicalDevices; }
    NODISCARD FORCEINLINE auto const& Vk_GetPhysicalDevice() const noexcept { return this->Vk_PhysicalDevice; }
    NODISCARD FORCEINLINE auto const& Vk_GetPhysicalDeviceMemoryProperties() const noexcept { return this->Vk_PhysicalDeviceMemoryProperties; }

    FORCEINLINE vk::SampleCountFlagBits Vk_GetMaxMsaaSampleCount() const noexcept { return this->Vk_MaxMsaaSampleCount; }
    FORCEINLINE vk::SampleCountFlags Vk_GetMsaaSampleLimits() const noexcept { return this->Vk_MsaaSampleLimits; }

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
    FORCEINLINE auto const& Vk_GetSurfaceFormat() const noexcept { check(this->Vk_SurfaceFormat.format != vk::Format::eUndefined) return this->Vk_SurfaceFormat; }

    FORCEINLINE auto Vk_GetNumberOfFramesInFlight() const noexcept { return this->Vk_FramesInFlight; }
    //# Public internal method to Jafg. To not use.
    void _Vk_ReportFramesInFlight(u32 FramesInFlight) noexcept
    {
        if (this->Vk_FramesInFlight != 0)
        {
            if (this->Vk_FramesInFlight != FramesInFlight)
            {
                LOG_FATAL(LogVulkan, "Number of frames in flight [{}] does not match the number of frames in flight reported by another surface [{}].", this->Vk_FramesInFlight, FramesInFlight)
            }
        }
        this->Vk_FramesInFlight = FramesInFlight;
        if (this->Vk_FramesInFlight < 1 || this->Vk_FramesInFlight > rhi::max_frames_in_flight)
        {
            LOG_FATAL(LogVulkan, "Number of frames in flight [{}] is invalid. Must be between 1 and {}.", this->Vk_FramesInFlight, rhi::max_frames_in_flight)
        }
    }

    //# @return A descriptor pool that lives for as long the frontend lives.
    FORCEINLINE auto const& Vk_GetDescriptorPool() const noexcept { check(*this->Vk_DescriptorPool) return this->Vk_DescriptorPool; }

    //# By providing no pool this method will fall back to its internal transient command pool (recommended).
    NODISCARD ENGINE_API vk::raii::CommandBuffer Vk_BeginSingleTimeCommands(vk::CommandPool Pool = nullptr) const;
    ENGINE_API void Vk_EndSingleTimeCommands(vk::raii::CommandBuffer CommandBuffer) const;

    //# Create any buffer through VMA.
    NODISCARD ENGINE_API rhi::device_buffer Vk_CreateBuffer(
          vk::BufferCreateInfo Info
        , vk::MemoryPropertyFlags Flags
        , VmaMemoryUsage Usage = VMA_MEMORY_USAGE_AUTO) const;
    NODISCARD ENGINE_API rhi::detailed_device_buffer Vk_CreateDetailedBuffer(
          vk::BufferCreateInfo Info
        , vk::MemoryPropertyFlags Flags
        , VmaMemoryUsage Usage = VMA_MEMORY_USAGE_AUTO) const;
    NODISCARD ENGINE_API rhi::mapped_device_buffer Vk_CreateMappedBuffer(vk::BufferCreateInfo Info) const;

    NODISCARD rhi::frame_array<rhi::mapped_device_buffer> Vk_CreateFrequentMappedBuffer(vk::BufferCreateInfo Info) const
    {
        check(this->Vk_FramesInFlight > 0u)
        rhi::frame_array<rhi::mapped_device_buffer> Result;
        for (auto Idx{0uz}; Idx < this->Vk_FramesInFlight; ++Idx)
        {
            Result[Idx] = this->Vk_CreateMappedBuffer(Info);
        }
        return Result;
    }

    //# By providing no pool this method will fall back to its internal transient command pool (recommended).
    ENGINE_API void Vk_CopyBuffer(vk::Buffer Src, vk::Buffer Dst, vk::BufferCopy BufferCopy, vk::CommandPool Pool = nullptr) const;

    //#
    //# Stage a buffer to the device.
    //# Usage: Frontend.Vk_StageBuffer(LStageBufferCreateInfo::<Type>({.BufferCopy = ..., .Data = ...}));
    //#
    //# @see LStageBufferCreateInfo
    //#
    ENGINE_API rhi::device_buffer Vk_StageBuffer(LStageBufferCreateInfo const& Info);

    ENGINE_API rhi::device_image Vk_CreateImage(vk::ImageCreateInfo const& Info, VmaAllocationCreateInfo const& AllocationCreateInfo) const;
    ENGINE_API rhi::device_image Vk_CreateDeviceLocalImage(vk::ImageCreateInfo const& Info) const;

    //# The resulting image will be in optimal shader read only layout.
    ENGINE_API rhi::device_image Vk_StageLinearImage(LStageLinearImageCreateInfo const& Info) const;

    //# Transitions an image layout. !!This is not for flight frame command buffers!!
    ENGINE_API void Vk_TransitionImageLayout(vk::ImageMemoryBarrier2 const& Barrier) const;

    ENGINE_API void Vk_SetSurfaceFormat(vk::SurfaceFormatKHR Format);

    //# Public private function!!! NEVER use. For internal stuff only!!!!!!!!
    ENGINE_API void _Vk_WaitIdle();
#if JAFG_WITH_EDITOR
    //# You may use this in the editor only. This event is traced.
    ENGINE_API void Vk_EditorWaitIdle();
#endif /* JAFG_WITH_EDITOR */

private:

    void Vk_FetchAndCheckInstanceExtensions();
    void Vk_FetchAndCheckInstanceLayers();
    void Vk_CreateInstance();
#if !JAFG_IN_SHIPPING
    void Vk_SetupDebugUtilsMessenger();
#endif /* !JAFG_IN_SHIPPING */
    void Vk_PickPhysicalDevice();
    void Vk_SetMaxMsaaSamples();
    void Vk_CreateLogicalDevice(LSurface const& QuerySurface);
    void Vk_CreateVma();

    std::optional<vk::Format> Vk_FindSupportedFormat(
          TArray<vk::Format> const& Candidates
        , vk::ImageTiling Tiling
        , vk::FormatFeatureFlags Features
        ) const;

    //# A rating of zero means the device is not suitable.
    std::multimap<u64, vk::raii::PhysicalDevice> Vk_RankPhysicalDevices(TArray<vk::raii::PhysicalDevice> const& PhysicalDevices) const;

    //# @note All mip levels (including zero) will be in the optimal shader read only layout after this method completes.
    void Vk_Generate2DMipMaps(vk::Image Image, vk::Format Format, vk::Extent2D Extent, u32 MipLevels) const;

    rhi::framework Vk_Framework{ rhi::framework::Identity };

    vk::raii::Context Vk_Context;

    TArray<vk::ExtensionProperties> Vk_AvailableInstanceExtensions;
    TArray<LString> Vk_RequiredInstanceExtensions{
        vk::KHRSurfaceExtensionName,
#if !JAFG_IN_SHIPPING
        vk::EXTDebugUtilsExtensionName,
        // VK_EXT_DEVICE_ADDRESS_BINDING_REPORT_EXTENSION_NAME,
#endif /* !JAFG_IN_SHIPPING */
        };

    TArray<vk::LayerProperties> Vk_AvailableInstanceLayers;
    TArray<LString> Vk_RequiredInstanceLayers;

    vk::raii::Instance Vk_Instance{ nullptr };

#if !JAFG_IN_SHIPPING
    vk::raii::DebugUtilsMessengerEXT Vk_DebugUtilsMessenger{ nullptr };
#endif /* !JAFG_IN_SHIPPING */

    TArray<LRankedPhysicalDevice> Vk_AvailablePhysicalDevices;
    vk::raii::PhysicalDevice Vk_PhysicalDevice{ nullptr };
    vk::PhysicalDeviceMemoryProperties Vk_PhysicalDeviceMemoryProperties;

    vk::SampleCountFlagBits Vk_MaxMsaaSampleCount{ vk::SampleCountFlagBits::e1 };
    vk::SampleCountFlags Vk_MsaaSampleLimits{};

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

    vk::raii::DescriptorPool Vk_DescriptorPool{ nullptr };
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
