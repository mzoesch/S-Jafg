// Copyright mzoesch. All rights reserved.

/* Included for vkGetInstanceProcAddr and vkGetDeviceProcAddr */
#include <vulkan/vulkan.h>

#define VMA_IMPLEMENTATION
#include "Framework/FrontendVk.h"

#include <GLFW/glfw3.h>
#if PLATFORM_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
#endif /* PLATFORM_WINDOWS */

#include <Rhi/Material.h>

#include "Framework/MeshSubsystem.h"
#include "Framework/TextureSubsystem.h"
#include "Platform/PlatformMisc.h"
#include "Stats/Stats.h"
#include "Engine/Engine.h"
#include "User/UserPreferences.h"
#include "Rhi/Rhi.h"
#include "Rhi/StaticMesh.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace
{

constexpr i32 GlfwContextVersionMajor{ 3 };
constexpr i32 GlfwContextVersionMinor{ 3 };
constexpr u32 Vk_ApiVersion{ vk::ApiVersion14 };

} /* ~Namespace <Anonymous> */

namespace
{

void GlfwErrorCallback(i32 Error, char const* Description)
{
    panicMsgf("GLFW Error (code {}): {}", Error, Description)
}

} /* ~Namespace <Anonymous> */

#if !IN_SHIPPING
static VKAPI_ATTR VkBool32 VKAPI_CALL Hermes(
      vk::DebugUtilsMessageSeverityFlagBitsEXT Severity
    , vk::DebugUtilsMessageTypeFlagsEXT Type
    , const vk::DebugUtilsMessengerCallbackDataEXT* CallbackData
    , void* UserData
    )
{
    (void)UserData;

    if (CallbackData == nullptr)
    {
        LOG_WARNING(LogVulkan, "Hermes invoked but no callback data provided.")
    }
    if (Severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
    {
        LOG_ERROR(LogVulkan, "[{}] Validation Layer [{}]: {}"
            , CallbackData->messageIdNumber
            , CallbackData->pMessageIdName ? CallbackData->pMessageIdName : "<missing-id-name>"
            , CallbackData->pMessage ? CallbackData->pMessage : "<missing-id-message>"
            )
    }
    else if (Severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
    {
        LOG_WARNING(LogVulkan, "[{}] Validation Layer [{}]: {}"
            , CallbackData->messageIdNumber
            , CallbackData->pMessageIdName ? CallbackData->pMessageIdName : "<missing-id-name>"
            , CallbackData->pMessage ? CallbackData->pMessage : "<missing-id-message>"
            )
    }
    else if (Type & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
    {
        LOG_WARNING(LogVulkan, "[{}] Performance Layer [{}]: {}"
            , CallbackData->messageIdNumber
            , CallbackData->pMessageIdName ? CallbackData->pMessageIdName : "<missing-id-name>"
            , CallbackData->pMessage ? CallbackData->pMessage : "<missing-id-message>"
            )
    }
    else if (Severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose)
    {
        LOG_VERBOSE(LogVulkan, "[{}] Info (Verbose) [{}]: {}"
            , CallbackData->messageIdNumber
            , CallbackData->pMessageIdName ? CallbackData->pMessageIdName : "<missing-id-name>"
            , CallbackData->pMessage ? CallbackData->pMessage : "<missing-id-message>"
            )
    }
    else
    {
        LOG_INFO(LogVulkan, "[{}] Info [{}]: {}"
            , CallbackData->messageIdNumber
            , CallbackData->pMessageIdName ? CallbackData->pMessageIdName : "<missing-id-name>"
            , CallbackData->pMessage ? CallbackData->pMessage : "<missing-id-message>"
            )
    }

    return VK_FALSE;
}
#endif /* !IN_SHIPPING */

void Jafg::Detail::FreeDeviceAllocation(vk::Buffer Handle, LDeviceAllocation Allocation) noexcept
{
    if (JAFG_LIKELY(GEngine))
    {
        static VmaAllocator Vma{ nullptr };
        if (JAFG_LIKELY(Vma))
        {
            check( GEngine->GetLocalEgo().GetFrontend().Vk_GetVmaAllocator() == Vma )
        }
        else
        {
            Vma = GEngine->GetLocalEgo().GetFrontend().Vk_GetVmaAllocator();
        }

        checkSlow( Vma )
        vmaDestroyBuffer(Vma, Handle, Allocation);
    }
    else if constexpr (IS_COMPILED_LOG(LogVulkan, Warning))
    {
        if (Handle || Allocation)
        {
            LOG_WARNING(LogVulkan, "VMA Device Buffer leaked during device buffer destruction.")
        }
    }

    return;
}

void Jafg::Detail::FreeDeviceAllocation(vk::Image Handle, LDeviceAllocation Allocation) noexcept
{
    if (JAFG_LIKELY(GEngine))
    {
        static VmaAllocator Vma{ nullptr };
        if (JAFG_LIKELY(Vma))
        {
            check( GEngine->GetLocalEgo().GetFrontend().Vk_GetVmaAllocator() == Vma )
        }
        else
        {
            Vma = GEngine->GetLocalEgo().GetFrontend().Vk_GetVmaAllocator();
        }

        checkSlow( Vma )
        vmaDestroyImage(Vma, Handle, Allocation);
    }
    else if constexpr (IS_COMPILED_LOG(LogVulkan, Warning))
    {
        if (Handle || Allocation)
        {
            LOG_WARNING(LogVulkan, "VMA device image buffer leaked during device buffer destruction.")
        }
    }

    return;
}

void Jafg::LFrontendVk::Initialize(LClassOuter* Outer)
{
    STAT_CYCLE_FUNCTION()

    LFrontendBase::Initialize(Outer);

    LOG_VERBOSE(LogSurface, "Initializing glfw.")

    check( Tasks::IsOnMasterThread() )

    // TODO: Do we want to use this sometimes/always? Or make a user flag for this??
    // glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);

    if (!glfwInit())
    {
        panic( "Failed to initialize glfw." )
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ::GlfwContextVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ::GlfwContextVersionMinor);

    glfwSetErrorCallback(::GlfwErrorCallback);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    const i32 Platform{ glfwGetPlatform() };
    if (Platform == GLFW_PLATFORM_WAYLAND)
    {
        LOG_VERBOSE(LogSurface, "Using Wayland platform.")
    }
    if (Platform == GLFW_PLATFORM_X11)
    {
        LOG_VERBOSE(LogSurface, "Using X11 platform.")
    }
    if (Platform == GLFW_PLATFORM_WIN32)
    {
        LOG_VERBOSE(LogSurface, "Using Win32 platform.")
    }

    i32 MonitorCount{ 0 };
    auto Monitors{ glfwGetMonitors(&MonitorCount) };
    if (MonitorCount < 1)
    {
        panic( "No suitable physical monitors detected." )
    }
    auto* PrimaryMonitor{ glfwGetPrimaryMonitor() };
    if (PrimaryMonitor == nullptr)
    {
        LOG_VERBOSE(LogSurface, "No primary monitor detected, picking first available monitor as primary.")
        /* Just pick the first one. */
        PrimaryMonitor = Monitors[0];
        check( PrimaryMonitor )
    }
    LOG_VERBOSE(LogSurface, "Found [{}] physical monitors connected.", MonitorCount)
    for (auto MonitorIndex{ 0uz }; MonitorIndex < static_cast<LSize>(MonitorCount); ++MonitorIndex)
    {
        GLFWmonitor* Monitor{ Monitors[MonitorIndex] };
        check( Monitor )

        LPhysicalViewport Pv{};
        Pv.Identifier = Monitor;
        glfwGetMonitorPhysicalSize(Monitor, &Pv.SizeMm.x, &Pv.SizeMm.y);
        glfwGetMonitorContentScale(Monitor, &Pv.ContentScale.x, &Pv.ContentScale.y);
        glfwGetMonitorWorkarea(Monitor,
            &Pv.WorkareaOffsetPx.x, &Pv.WorkareaOffsetPx.y,
            &Pv.WorkareaPx.x, &Pv.WorkareaPx.y
            );
        Pv.Prefix = Jafg::SprintF("{}-", MonitorIndex);
        Pv.Name = glfwGetMonitorName(Monitor);
        // if (Monitor == PrimaryMonitor)
        // {
        //     Pv.bPrimary = true;
        // }
        // else
        // {
        //     check( Pv.bPrimary == false )
        // }

        GLFWvidmode const* VidMode{ glfwGetVideoMode(Monitor) };
        check( VidMode )
        Pv.Bits.x = VidMode->redBits;
        Pv.Bits.y = VidMode->greenBits;
        Pv.Bits.z = VidMode->blueBits;

        Pv.RefreshRateHz = VidMode->refreshRate;

        LOG_VERBOSE(LogSurface, "    Physical Monitor [{}{}]: {}x{}px @ {}hz, {}x{}mm, RGB=[{}|{}|{}]",
            Pv.Prefix, Pv.Name,
            Pv.WorkareaPx.x, Pv.WorkareaPx.y,
            Pv.RefreshRateHz,
            Pv.SizeMm.x, Pv.SizeMm.y,
            Pv.Bits.x, Pv.Bits.y, Pv.Bits.z
            )
        this->UsablePhysicalViewports.emplace_back(std::move(Pv));
    }

    LOG_VERBOSE(LogVulkan, "Initializing Vulkan.")

    LOG_VERBOSE(LogVulkan, "Setting up Vulkan dynamic dispatch loader.")
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    check(glfwVulkanSupported())

    this->Vk_FetchAndCheckInstanceExtensions();
    this->Vk_FetchAndCheckInstanceLayers();
    this->Vk_CreateInstance();
#if !IN_SHIPPING
    this->Vk_SetupDebugUtilsMessenger();
#endif /* !IN_SHIPPING */

    LOG_VERBOSE(LogVulkan, "Initializing Vulkan dispatch loader with instance.")
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*this->Vk_Instance);

    this->Vk_PickPhysicalDevice();
    this->Vk_SetMaxMsaaSamples();

    /*
     * In order to create the best and optimal preferences for Vulkan, we first create a new surface to query
     * data from it. This will allow for some great optimizations...
     */
    LSurfaceCreateInfo SurfaceInfo{
#if !IN_SHIPPING
        /* For development purposes, we want a smaller window as it does not cover so much space. */
        .DesiredDimensionsPx = { 855, 475 },
#endif /* !IN_SHIPPING */
        .HumanReadableName = "Jafg - @mzoesch",
        };
    TUnique QuerySurface{ std::make_unique<LSurface>(SurfaceInfo) };

    this->Vk_CreateLogicalDevice(*QuerySurface);

    LOG_VERBOSE(LogVulkan, "Initializing Vulkan dispatch loader with device.")
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*this->Vk_Device);

    this->Vk_CreateVma();

    LOG_VERBOSE(LogVulkan, "Creating transient command pool.")
    this->Vk_TransientCommandPool = vk::raii::CommandPool{this->Vk_Device, {
        .flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = this->Vk_GraphicsQueueFamilyIndex // TODO: Queue that supports VK_QUEUE_TRANSFER_BIT.
        }};

    this->Vk_PreferredDepthFormat = this->Vk_FindSupportedFormat(
        {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
        ).value_or(vk::Format::eUndefined);
    if (this->Vk_PreferredDepthFormat == vk::Format::eUndefined)
    {
        panic("Failed to find a supported depth format.")
    }

    this->Vk_UpdateSamplers();

    std::array Sizes{
        vk::DescriptorPoolSize{
            .type = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = 2048, /* Completely arbitrary limit. */
            },
        vk::DescriptorPoolSize{
            .type = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = 2048, /* Completely arbitrary limit. */
            },
        };
    this->Vk_DescriptorPool = vk::raii::DescriptorPool{this->Vk_Device, vk::DescriptorPoolCreateInfo{
            .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, // TODO: Flags??
            .maxSets = 1024, /* Completely arbitrary limit. */
            .poolSizeCount = static_cast<uint32_t>(Sizes.size()),
            .pPoolSizes = Sizes.data(),
        }};

    this->AddSurface(std::move(QuerySurface), ENewSurfaceBehavior::FocusIfNonePresent);
    this->GetSurfaces().back()->LateSetupVk();

    this->Vk_PerspectiveCameraDescriptorSetLayout = vk::raii::DescriptorSetLayout{
        this->Vk_Device,
        vk::DescriptorSetLayoutCreateInfo{
            .bindingCount = static_cast<u32>(PerspectiveCameraSetLayout::Bindings().size()),
            .pBindings = PerspectiveCameraSetLayout::Bindings().data(),
            }
        };
    // this->Vk_DefaultMaterialDescriptorSetLayout = vk::raii::DescriptorSetLayout{
    //     this->Vk_Device,
    //     vk::DescriptorSetLayoutCreateInfo{
    //         .bindingCount = static_cast<u32>(DefaultMaterialSetLayout::Bindings().size()),
    //         .pBindings = DefaultMaterialSetLayout::Bindings().data(),
    //         }
    //     };

    return;
}

void Jafg::LFrontendVk::TearDown()
{
    LFrontendBase::TearDown();

    GetMutableSingleton<JMeshSubsystem>().PurgeUnused();

    this->Vk_DefaultSampler.clear();
    this->Vk_DescriptorPool.reset();

    LOG_VERBOSE(LogVulkan, "Destroying VMA.")
    vmaDestroyAllocator(this->Vk_VmaAllocator);

    LOG_VERBOSE(LogSurface, "Terminating glfw.")
    glfwTerminate();

    return;
}

vk::raii::CommandBuffer Jafg::LFrontendVk::Vk_BeginSingleTimeCommands(vk::CommandPool Pool) const
{
    if (Pool == nullptr)
    {
        Pool = this->Vk_TransientCommandPool;
    }

    vk::CommandBufferAllocateInfo AllocateInfo{
        .commandPool = Pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
        };

    vk::raii::CommandBuffer CommandBuffer{ std::move(this->Vk_Device.allocateCommandBuffers(AllocateInfo).front()) };

    vk::CommandBufferBeginInfo BeginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
    CommandBuffer.begin(BeginInfo);

    return CommandBuffer;
}

void Jafg::LFrontendVk::Vk_EndSingleTimeCommands(vk::raii::CommandBuffer CommandBuffer) const
{
    check( Tasks::IsOnMasterThread() )
    check( *CommandBuffer )

    CommandBuffer.end();

    vk::SubmitInfo SubmitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*CommandBuffer };
    //# TODO: Use a dedicated transfer queue if available.
    this->Vk_GraphicsQueue.submit(SubmitInfo, nullptr);
    this->Vk_GraphicsQueue.waitIdle();

    return;
}

Jafg::LDeviceBuffer Jafg::LFrontendVk::Vk_CreateBuffer(vk::BufferCreateInfo Info, vk::MemoryPropertyFlags Flags, VmaMemoryUsage Usage /* = VMA_MEMORY_USAGE_AUTO */) const
{
    VkBuffer Buffer;
    VmaAllocation Allocation;
    VmaAllocationCreateInfo AllocationCreateInfo{
        .usage = Usage,
        .requiredFlags = static_cast<VkMemoryPropertyFlags>(Flags),
        };

    auto Res{vmaCreateBuffer(
        this->Vk_VmaAllocator,
        Info,
        &AllocationCreateInfo,
        &Buffer,
        &Allocation,
        nullptr
        )};
    check( Res == VK_SUCCESS )

    return { Buffer, Allocation };
}

Jafg::LDetailedDeviceBuffer Jafg::LFrontendVk::Vk_CreateDetailedBuffer(vk::BufferCreateInfo Info, vk::MemoryPropertyFlags Flags, VmaMemoryUsage Usage /* = VMA_MEMORY_USAGE_AUTO */) const
{
    VkBuffer Buffer;
    VmaAllocation Allocation;
    VmaAllocationCreateInfo AllocationCreateInfo{
        .usage = Usage,
        .requiredFlags = static_cast<VkMemoryPropertyFlags>(Flags),
        };

    VmaAllocationInfo AllocationInfo{};
    auto Res{vmaCreateBuffer(
        this->Vk_VmaAllocator,
        Info,
        &AllocationCreateInfo,
        &Buffer,
        &Allocation,
        &AllocationInfo
        )};
    check( Res == VK_SUCCESS )

    return { Buffer, Allocation, std::move(AllocationInfo) };
}

Jafg::LMappedDeviceBuffer Jafg::LFrontendVk::Vk_CreateMappedBuffer(vk::BufferCreateInfo Info) const
{
    VkBuffer Buffer;
    VmaAllocation Allocation;
    VmaAllocationCreateInfo AllocationCreateInfo{
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

    VmaAllocationInfo AllocationInfo{};
    auto Res{vmaCreateBuffer(
        this->Vk_VmaAllocator,
        Info,
        &AllocationCreateInfo,
        &Buffer,
        &Allocation,
        &AllocationInfo
        )};
    check( Res == VK_SUCCESS )
    check( AllocationInfo.pMappedData )

    return { Buffer, Allocation, AllocationInfo.pMappedData };
}

void Jafg::LFrontendVk::Vk_CopyBuffer(vk::Buffer Src, vk::Buffer Dst, vk::BufferCopy BufferCopy, vk::CommandPool Pool /* = nullptr */) const
{
    check( Src && Dst )

    auto Buffer{this->Vk_BeginSingleTimeCommands(Pool)};
    Buffer.copyBuffer(Src, Dst, BufferCopy);
    this->Vk_EndSingleTimeCommands(std::move(Buffer));

    return;
}

Jafg::LDeviceBuffer Jafg::LFrontendVk::Vk_StageBuffer(LStageBufferCreateInfo const& Info)
{
    VkBuffer StagingBuffer;
    VmaAllocation StagingAllocation;
    VmaAllocationCreateInfo StagingAllocationCreateInfo{
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT
            // TODO: This is optional. Make configurable? What are the side effects?
            | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
        };
    vk::BufferCreateInfo StagingBufferCreateInfo{
        .size = Info.BufferCopy.size,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
        // TODO: This only works for one queue family. Make configurable?
        .sharingMode = vk::SharingMode::eExclusive
        };

    VmaAllocationInfo StagingAllocationInfo{};

    auto Res{vmaCreateBuffer(
        this->Vk_VmaAllocator,
        StagingBufferCreateInfo,
        &StagingAllocationCreateInfo,
        &StagingBuffer,
        &StagingAllocation,
        &StagingAllocationInfo
        )};
    check( Res == VK_SUCCESS )
    check( StagingAllocationInfo.pMappedData )

    // TODO: Check that HOST_VISIBLE | HOST_COHERENT is picked. Otherwise flush.
    std::memcpy(StagingAllocationInfo.pMappedData, Info.Data, static_cast<size_t>(Info.BufferCopy.size));

    VkBuffer DeviceBuffer;
    VmaAllocation DeviceAllocation;
    VmaAllocationCreateInfo DeviceAllocInfo{
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        };
    check( Info.Usage & vk::BufferUsageFlagBits::eTransferDst )
    vk::BufferCreateInfo DeviceBufferCreateInfo{
        .size = Info.BufferCopy.size,
        .usage = Info.Usage,
        };

    Res = vmaCreateBuffer(
        this->Vk_VmaAllocator,
        DeviceBufferCreateInfo,
        &DeviceAllocInfo,
        &DeviceBuffer,
        &DeviceAllocation,
        nullptr
        );
    check( Res == VK_SUCCESS )

    this->Vk_CopyBuffer(StagingBuffer, DeviceBuffer, Info.BufferCopy);

    vmaDestroyBuffer(this->Vk_VmaAllocator, StagingBuffer, StagingAllocation);

    return { DeviceBuffer, DeviceAllocation };
}

Jafg::LDeviceImage Jafg::LFrontendVk::Vk_CreateImage(vk::ImageCreateInfo const& Info, VmaAllocationCreateInfo const& AllocationCreateInfo)
{
    VkImage Image;
    VmaAllocation Allocation;

    auto Res{this->Vk_VmaAllocator->CreateImage(
        Info,
        &AllocationCreateInfo,
        nullptr,
        &Image,
        &Allocation,
        nullptr
        )};
    check( Res == VK_SUCCESS )

    return LDeviceImage{ Image, Allocation };
}

Jafg::LDeviceImage Jafg::LFrontendVk::Vk_CreateDeviceLocalImage(vk::ImageCreateInfo const& Info)
{
    VmaAllocationCreateInfo AllocationInfo{
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        };

    return this->Vk_CreateImage(Info, AllocationInfo);
}

Jafg::LDeviceImage Jafg::LFrontendVk::Vk_StageLinearImage(LStageLinearImageCreateInfo const& Info)
{
    check( Info.Data )

    auto N{ static_cast<size_t>(Vk_GetBytesPerPixel(Info.Info.format) * Info.Info.extent.width * Info.Info.extent.height) };

    auto StagingBuffer{this->Vk_CreateMappedBuffer({
        .size = N,
        .usage = vk::BufferUsageFlagBits::eTransferSrc
        })};
    std::memcpy(StagingBuffer.GetData(), Info.Data, N);

    auto Image{ this->Vk_CreateDeviceLocalImage(Info.Info) };

    this->Vk_TransitionImageLayout({
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eTransferDstOptimal,
        .image = Image.GetBuffer(),
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = Info.Info.mipLevels,
            .baseArrayLayer = 0,
            .layerCount = 1
            },
        });

    {
        auto CommandBuffer{ this->Vk_BeginSingleTimeCommands() };
        vk::BufferImageCopy Region{
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {vk::ImageAspectFlagBits::eColor, 0, 0, 1},
            .imageOffset = {0, 0, 0},
            .imageExtent = Info.Info.extent
            };
        CommandBuffer.copyBufferToImage(StagingBuffer.GetBuffer(), Image.GetBuffer(), vk::ImageLayout::eTransferDstOptimal, Region);
        this->Vk_EndSingleTimeCommands(std::move(CommandBuffer));
    }

    if (Info.Info.mipLevels > 1)
    {
        check( Info.Info.extent.depth == 1 && "Vk_StageLinearImage does currently only support 2D images with mipmaps." )
        this->Vk_Generate2DMipMaps(
              Image.GetBuffer(), Info.Info.format
            , vk::Extent2D{ Info.Info.extent.width, Info.Info.extent.height }
            , Info.Info.mipLevels
            );
    }
    else
    {
        this->Vk_TransitionImageLayout({
            .oldLayout = vk::ImageLayout::eTransferDstOptimal,
            .newLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
            .image = Image.GetBuffer(),
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = Info.Info.mipLevels,
                .baseArrayLayer = 0,
                .layerCount = 1
                },
            });
    }

    return Image;
}

void Jafg::LFrontendVk::Vk_TransitionImageLayout(vk::ImageMemoryBarrier2 const& Barrier)
{
    auto Buffer{ this->Vk_BeginSingleTimeCommands() };

    vk::DependencyInfo DependencyInfo{
        .dependencyFlags = {},
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &Barrier,
        };

    Buffer.pipelineBarrier2(DependencyInfo);

    this->Vk_EndSingleTimeCommands(std::move(Buffer));

    return;
}

void Jafg::LFrontendVk::Vk_SetSurfaceFormat(vk::SurfaceFormatKHR Format)
{
    if (this->Vk_SurfaceFormat.format == vk::Format::eUndefined)
    {
        check( Format.format != vk::Format::eUndefined )
        this->Vk_SurfaceFormat = Format;
    }
    else
    {
        jassert( this->Vk_SurfaceFormat.format == Format.format )
        jassert( this->Vk_SurfaceFormat.colorSpace == Format.colorSpace )
    }

    return;
}

void Jafg::LFrontendVk::_Vk_WaitIdle()
{
    STAT_CYCLE_FUNCTION()
    this->Vk_Device.waitIdle();
    return;
}

void Jafg::LFrontendVk::Vk_FetchAndCheckInstanceExtensions()
{
    LOG_VERBOSE(LogVulkan, "Refetching available instance extensions.")

    this->Vk_AvailableInstanceExtensions = this->Vk_Context.enumerateInstanceExtensionProperties();
    LOG_VERBOSE(LogVulkan, "Available Vulkan instance extensions:")
    for (auto const& Extension : this->Vk_AvailableInstanceExtensions)
    {
        LOG_VERBOSE(LogVulkan, "    {} spec[{}]", LStringView{Extension.extensionName}, Extension.specVersion)
    }

    u32 Glfw3ExtensionCount{ 0 };
    auto Glfw3Extensions{ glfwGetRequiredInstanceExtensions(&Glfw3ExtensionCount) };
    for (u32 Idx{ 0 }; Idx < Glfw3ExtensionCount; ++Idx)
    {
        LString Glfw3ExtensionStr{ Glfw3Extensions[Idx] };
        if (algo::contains(this->Vk_RequiredInstanceExtensions, Glfw3ExtensionStr) == false)
        {
            this->Vk_RequiredInstanceExtensions.emplace_back(std::move(Glfw3ExtensionStr));
        }
        continue;
    }

    LOG_VERBOSE(LogVulkan, "Required Vulkan instance extensions:")
    for (auto const& Extension : this->Vk_RequiredInstanceExtensions)
    {
        LOG_VERBOSE(LogVulkan, "    {}", Extension)
    }

    for (LString const& Extension : this->Vk_RequiredInstanceExtensions)
    {
        bool bFound{ false };
        for (auto const& AvailableExtension : Vk_AvailableInstanceExtensions)
        {
            if (Extension == AvailableExtension.extensionName)
            {
                bFound = true;
                break;
            }
        }

        if (bFound == false)
        {
            panicMsgf("Required Vulkan instance extension [{}] is not available.", Extension)
        }
    }
    LOG_VERBOSE(LogVulkan, "All required instance extensions are available. Proceeding.")

    return;
}

void Jafg::LFrontendVk::Vk_FetchAndCheckInstanceLayers()
{
    LOG_VERBOSE(LogVulkan, "Refetching available instance layers.")

    this->Vk_AvailableInstanceLayers = this->Vk_Context.enumerateInstanceLayerProperties();
    LOG_VERBOSE(LogVulkan, "Available Vulkan instance layers:")
    for (auto const& Layer : this->Vk_AvailableInstanceLayers)
    {
        LOG_VERBOSE(LogVulkan, "    {} spec[{}]", LStringView{Layer.layerName}, Layer.specVersion)
    }

#if !IN_SHIPPING
    if (algo::contains(this->Vk_AvailableInstanceLayers, "VK_LAYER_KHRONOS_validation", [](vk::LayerProperties const& Layer)
        {
            return LStringView{Layer.layerName};
        }) == false)
    {
        LOG_WARNING(LogVulkan, "No such layer [VK_LAYER_KHRONOS_validation]. Validation layers will be disabled.")
    }
    else
    {
        if (algo::contains(this->Vk_RequiredInstanceLayers, "VK_LAYER_KHRONOS_validation") == false)
        {
            this->Vk_RequiredInstanceLayers.emplace_back("VK_LAYER_KHRONOS_validation");
        }
    }
#endif /* !IN_SHIPPING */

    // if (algo::contains(this->Vk_AvailableInstanceLayers, "VK_LAYER_RENDERDOC_Capture", [](vk::LayerProperties const& Layer)
    // {
    //     return LStringView{Layer.layerName};
    // }) == false)
    // {
    //     LOG_WARNING(LogVulkan, "No such layer [VK_LAYER_RENDERDOC_Capture]. Validation layers will be disabled.")
    // }
    // else
    // {
    //     if (algo::contains(this->Vk_RequiredInstanceLayers, "VK_LAYER_RENDERDOC_Capture") == false)
    //     {
    //         this->Vk_RequiredInstanceLayers.emplace_back("VK_LAYER_RENDERDOC_Capture");
    //     }
    // }

    LOG_VERBOSE(LogVulkan, "Required Vulkan instance layers:")
    for (auto const& Layer : this->Vk_RequiredInstanceLayers)
    {
        LOG_VERBOSE(LogVulkan, "    {}", Layer)
    }

    for (LString const& Layer : this->Vk_RequiredInstanceLayers)
    {
        bool bFound{ false };
        for (auto const& AvailableLayer : this->Vk_AvailableInstanceLayers)
        {
            if (Layer == AvailableLayer.layerName)
            {
                bFound = true;
                break;
            }
        }

        if (bFound == false)
        {
            panicMsgf("Required Vulkan instance layer [{}] is not available.", Layer)
        }
    }
    LOG_VERBOSE(LogVulkan, "All required instance layers are available. Proceeding.")

    return;
}

void Jafg::LFrontendVk::Vk_CreateInstance()
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan instance.")

    if (auto SupportedVersion{vk::enumerateInstanceVersion()}; SupportedVersion < ::Vk_ApiVersion)
    {
        panicMsgf(
            "Vulkan API version [{}.{}.{}] is not supported. Supported version is [{}.{}.{}].",
            VK_VERSION_MAJOR(::Vk_ApiVersion),
            VK_VERSION_MINOR(::Vk_ApiVersion),
            VK_VERSION_PATCH(::Vk_ApiVersion),
            VK_VERSION_MAJOR(SupportedVersion),
            VK_VERSION_MINOR(SupportedVersion),
            VK_VERSION_PATCH(SupportedVersion)
            )
    }
    else
    {
        LOG_VERBOSE(LogVulkan, "Max supported Vulkan API version is [{}.{}.{}].",
            VK_VERSION_MAJOR(SupportedVersion),
            VK_VERSION_MINOR(SupportedVersion),
            VK_VERSION_PATCH(SupportedVersion)
            )
    }

    constexpr vk::ApplicationInfo ApplicationInfo{
        .pApplicationName = "S-Jafg @mzoesch",
        .applicationVersion = VK_MAKE_VERSION( PRIVATE_ENGINE_VERSION_MAJOR, PRIVATE_ENGINE_VERSION_MINOR, PRIVATE_ENGINE_VERSION_PATCH ),
        .pEngineName = "Jafg Engine",
        .engineVersion = VK_MAKE_VERSION( PRIVATE_ENGINE_VERSION_MAJOR, PRIVATE_ENGINE_VERSION_MINOR, PRIVATE_ENGINE_VERSION_PATCH ),
        .apiVersion = ::Vk_ApiVersion
        };

    TArray<char const*> RequiredInstanceExtensions_c_str; RequiredInstanceExtensions_c_str.reserve(this->Vk_RequiredInstanceExtensions.size());
    algo::for_each(this->Vk_RequiredInstanceExtensions, [&RequiredInstanceExtensions_c_str](LString const& Extension)
    {
        RequiredInstanceExtensions_c_str.emplace_back(Extension.c_str());
    });

    TArray<char const*> RequiredInstanceLayers_c_str; RequiredInstanceLayers_c_str.reserve(this->Vk_RequiredInstanceLayers.size());
    algo::for_each(this->Vk_RequiredInstanceLayers, [&RequiredInstanceLayers_c_str](LString const& Layer)
    {
        RequiredInstanceLayers_c_str.emplace_back(Layer.c_str());
    });

    vk::InstanceCreateInfo CreateInfo{
        .pApplicationInfo = &ApplicationInfo,
        .enabledLayerCount = static_cast<u32>(RequiredInstanceLayers_c_str.size()),
        .ppEnabledLayerNames = RequiredInstanceLayers_c_str.data(),
        .enabledExtensionCount = static_cast<u32>(RequiredInstanceExtensions_c_str.size()),
        .ppEnabledExtensionNames = RequiredInstanceExtensions_c_str.data(),
        };

    this->Vk_Instance = vk::raii::Instance{ this->Vk_Context, CreateInfo };

    return;
}

#if !IN_SHIPPING
void Jafg::LFrontendVk::Vk_SetupDebugUtilsMessenger()
{
    LOG_VERBOSE(LogVulkan, "Setting up Vulkan debug utils messenger ext.")

    vk::DebugUtilsMessageSeverityFlagsEXT SeverityFlags{
          vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        };

    vk::DebugUtilsMessageTypeFlagsEXT TypeFlags{
          vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
        | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        // | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
        };

    vk::DebugUtilsMessengerCreateInfoEXT CreateInfo{
        .messageSeverity = SeverityFlags,
        .messageType = TypeFlags,
        .pfnUserCallback = &::Hermes,
        .pUserData = this
        };

    this->Vk_DebugUtilsMessenger = this->Vk_Instance.createDebugUtilsMessengerEXT(CreateInfo);

    return;
}
#endif /* !IN_SHIPPING */

void Jafg::LFrontendVk::Vk_PickPhysicalDevice()
{
    LOG_VERBOSE(LogVulkan, "Picking Vulkan physical device.")

    this->Vk_PhysicalDevice = nullptr;
    algo::orphan(&this->Vk_AvailablePhysicalDevices);

    TArray<vk::raii::PhysicalDevice> AvailablePhysicalDevices;
    AvailablePhysicalDevices = this->Vk_Instance.enumeratePhysicalDevices();

    if (AvailablePhysicalDevices.empty())
    {
        panic("Failed to find any physical devices with Vulkan support.")
    }

    for (auto RankedPhysicalDevices{this->Vk_RankPhysicalDevices(AvailablePhysicalDevices)};
         auto const& [Rating, PhysicalDevice] : RankedPhysicalDevices)
    {
        this->Vk_AvailablePhysicalDevices.emplace_back(Rating, PhysicalDevice);
    }
    algo::sort(this->Vk_AvailablePhysicalDevices, algo::greater{}, &LRankedPhysicalDevice::Rating);
    LOG_VERBOSE(LogVulkan, "Available physical devices ranked by suitability:")
    for (auto const& [Rating, PhysicalDevice] : this->Vk_AvailablePhysicalDevices)
    {
        auto Properties = PhysicalDevice.getProperties();
        LOG_VERBOSE(LogVulkan, "    [{}] rated [{}]: API v{}.{}.{}, Driver v{}.{}.{}",
            LStringView{Properties.deviceName},
            Rating,
            VK_VERSION_MAJOR(Properties.apiVersion),
            VK_VERSION_MINOR(Properties.apiVersion),
            VK_VERSION_PATCH(Properties.apiVersion),
            VK_VERSION_MAJOR(Properties.driverVersion),
            VK_VERSION_MINOR(Properties.driverVersion),
            VK_VERSION_PATCH(Properties.driverVersion)
            )
        continue;
    }

    check(this->Vk_AvailablePhysicalDevices.empty() == false)
    if (this->Vk_AvailablePhysicalDevices[0].Rating == 0)
    {
        panic("Failed to find a suitable physical device.")
    }

    if (const auto& Prefs{GetSingleton<JUserPreferences>()}; Prefs.PreferredPhysicalDevice.empty() == false)
    {
        for (auto const& [Rating, PhysicalDevice] : this->Vk_AvailablePhysicalDevices)
        {
            if (auto Properties{ PhysicalDevice.getProperties() }; Prefs.PreferredPhysicalDevice == Properties.deviceName)
            {
                if (Rating == 0)
                {
                    LOG_WARNING(LogVulkan, "Preferred physical device [{}] found but is no longer suitable. Clearing user prefs and falling back to best rated device.",
                        Prefs.PreferredPhysicalDevice
                        )
                    algo::orphan(&GetMutableSingleton<JUserPreferences>().PreferredPhysicalDevice);
                }
                else
                {
                    this->Vk_PhysicalDevice = PhysicalDevice;
                    LOG_VERBOSE(LogVulkan, "Selected preferred (by user) physical device [{}].",
                        LStringView{Properties.deviceName}
                        )
                }
                break;
            }
            continue;
        }

        LOG_WARNING(LogVulkan, "Preferred physical device [{}] not found among available devices. Clearing user prefs and falling back to best rated device.",
            Prefs.PreferredPhysicalDevice
            )
        algo::orphan(&GetMutableSingleton<JUserPreferences>().PreferredPhysicalDevice);
    }

    if (!*this->Vk_PhysicalDevice)
    {
        this->Vk_PhysicalDevice = this->Vk_AvailablePhysicalDevices[0].PhysicalDevice;
        LOG_VERBOSE(LogVulkan, "Selected physical device [{}].",
            LStringView{this->Vk_PhysicalDevice.getProperties().deviceName}
            )
    }

    this->Vk_PhysicalDeviceMemoryProperties = this->Vk_PhysicalDevice.getMemoryProperties();
    LOG_VERBOSE(LogVulkan, "Physical device memory properties:")
    for (auto Idx{ 0uz }; Idx < this->Vk_PhysicalDeviceMemoryProperties.memoryTypeCount; ++Idx)
    {
        auto const& MemType{ this->Vk_PhysicalDeviceMemoryProperties.memoryTypes[Idx] };
        auto const& MemHeap{ this->Vk_PhysicalDeviceMemoryProperties.memoryHeaps[MemType.heapIndex] };
        LOG_VERBOSE(LogVulkan, "    Type[{}]: Heap[{}] Size[{}MB] PropertyFlags[{}]",
            Idx,
            MemType.heapIndex,
            MemHeap.size / (1024 * 1024),
            vk::to_string(MemType.propertyFlags)
            )
    }

    return;
}

void Jafg::LFrontendVk::Vk_SetMaxMsaaSamples()
{
    LOG_VERBOSE(LogVulkan, "Determining max usable MSAA sample count.")

    check( *this->Vk_PhysicalDevice )

    const vk::PhysicalDeviceProperties PhysicalDeviceProperties{ this->Vk_PhysicalDevice.getProperties() };
    const vk::SampleCountFlags Counts
    {
        PhysicalDeviceProperties.limits.framebufferColorSampleCounts & PhysicalDeviceProperties.limits.framebufferDepthSampleCounts
    };

    this->Vk_MaxMsaaSampleCount = Jafg::Vk_GetMaxMsaaSamples(Counts);

    LOG_VERBOSE(LogVulkan, "Max usable sample count: [{}].", vk::to_string(this->Vk_MaxMsaaSampleCount))

    return;
}

void Jafg::LFrontendVk::Vk_CreateLogicalDevice(LSurface const& QuerySurface)
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan logical device.")

    auto QueueFamilyProperties{ this->Vk_PhysicalDevice.getQueueFamilyProperties() };
    auto GraphicsQueueFamilyProperty{ algo::find_if(QueueFamilyProperties, [](auto const& Qfp)
    {
        return (Qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
    })};
    if (GraphicsQueueFamilyProperty == QueueFamilyProperties.end())
    {
        panic( "Failed to find a suitable graphics queue family." )
    }
    u32 GraphicsQueueFamilyIndex{ static_cast<u32>(algo::distance(QueueFamilyProperties.begin(), GraphicsQueueFamilyProperty)) };
    if (JAFG_UNLIKELY(GraphicsQueueFamilyIndex == QueueFamilyProperties.size()))
    {
        panic( "Failed to find a suitable graphics queue family." )
    }

    /* We prefer a combined graphics+present queue (because performance), but also separate ones are ok. */
    u32 PresentQueueFamilyIndex = this->Vk_PhysicalDevice.getSurfaceSupportKHR(GraphicsQueueFamilyIndex, *QuerySurface.Vk_GetSurface())
        ? GraphicsQueueFamilyIndex
        : static_cast<u32>(QueueFamilyProperties.size());

    if (PresentQueueFamilyIndex == QueueFamilyProperties.size())
    {
        /* Now try really hard to find a combined queue. */
        for (auto Idx{ 0uz }; Idx < QueueFamilyProperties.size(); ++Idx)
        {
            if (   (QueueFamilyProperties[Idx].queueFlags & vk::QueueFlagBits::eGraphics)
                && this->Vk_PhysicalDevice.getSurfaceSupportKHR(static_cast<u32>( Idx ), *QuerySurface.Vk_GetSurface())
            )
            {
                GraphicsQueueFamilyIndex = static_cast<u32>(Idx);
                PresentQueueFamilyIndex  = GraphicsQueueFamilyIndex;
                break;
            }

            continue;
        }

        /* Yikes, ig we now have to tile this. */
        if (PresentQueueFamilyIndex == QueueFamilyProperties.size())
        {
            for (auto Idx{ 0uz }; Idx < QueueFamilyProperties.size(); ++Idx)
            {
                if (this->Vk_PhysicalDevice.getSurfaceSupportKHR(static_cast<u32>(Idx), *QuerySurface.Vk_GetSurface()))
                {
                    PresentQueueFamilyIndex = static_cast<u32>(Idx);
                    break;
                }

                continue;
            }
        }
    }

    if (GraphicsQueueFamilyIndex == QueueFamilyProperties.size())
    {
        panic( "Failed to find a suitable graphics queue family index." )
    }
    if (PresentQueueFamilyIndex == QueueFamilyProperties.size())
    {
        panic( "Failed to find a suitable present queue family index." )
    }

    vk::StructureChain<
          vk::PhysicalDeviceFeatures2
        , vk::PhysicalDeviceVulkan11Features
        , vk::PhysicalDeviceVulkan13Features
        , vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
        > FeaturesChain{
        {.features = { .samplerAnisotropy = VK_TRUE } }, /* vk::PhysicalDeviceFeatures2 */
        {.shaderDrawParameters = VK_TRUE }, /* vk::PhysicalDeviceVulkan11Features */
        {.synchronization2 = VK_TRUE, .dynamicRendering = VK_TRUE}, /* vk::PhysicalDeviceVulkan13Features */
        {.extendedDynamicState = VK_TRUE} /* vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT */
        };

    f32 QueuePriority{ 1.0f };
    vk::DeviceQueueCreateInfo DeviceQueueCreateInfo{
        .queueFamilyIndex = GraphicsQueueFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = &QueuePriority
        };

    vk::DeviceCreateInfo DeviceCreateInfo{
        .pNext = &FeaturesChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &DeviceQueueCreateInfo,
        .enabledExtensionCount = static_cast<u32>(this->Vk_RequiredDeviceExtensions.size()),
        .ppEnabledExtensionNames = this->Vk_RequiredDeviceExtensions.data(),
        };

    this->Vk_Device = vk::raii::Device{ this->Vk_PhysicalDevice, DeviceCreateInfo };
    this->Vk_GraphicsQueue = vk::raii::Queue{ this->Vk_Device, GraphicsQueueFamilyIndex, 0 };
    this->Vk_PresentQueue = vk::raii::Queue{ this->Vk_Device, PresentQueueFamilyIndex, 0 };

    this->Vk_GraphicsQueueFamilyIndex = GraphicsQueueFamilyIndex;
    this->Vk_PresentQueueFamilyIndex = PresentQueueFamilyIndex;

    LOG_VERBOSE(LogVulkan, "Finished loading logical device.")
    LOG_VERBOSE(LogVulkan, "    Graphics Queue Family Index: [{}].",  GraphicsQueueFamilyIndex)
    LOG_VERBOSE(LogVulkan, "    Present  Queue Family Index: [{}].",  PresentQueueFamilyIndex)

    return;
}

void Jafg::LFrontendVk::Vk_CreateVma()
{
    LOG_VERBOSE(LogVulkan, "Creating VMA.")

    VmaVulkanFunctions VulkanFunctions{
        .vkGetInstanceProcAddr = &vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr   = &vkGetDeviceProcAddr,
        };

    VmaAllocatorCreateInfo VmaAllocatorCreateInfo{
        .flags = {}
            //  VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT
            // | VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT
            // | VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT
            // | VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT
            // | VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_EXTENSION_BIT
            ,
        .physicalDevice = *this->Vk_PhysicalDevice,
        .device = *this->Vk_Device,
        .preferredLargeHeapBlockSize = 0,
        .pAllocationCallbacks = nullptr,
        .pDeviceMemoryCallbacks = nullptr,
        .pHeapSizeLimit = nullptr,
        .pVulkanFunctions = &VulkanFunctions,
        .instance = *this->Vk_Instance,
        .vulkanApiVersion = VK_API_VERSION_1_4,
        .pTypeExternalMemoryHandleTypes = nullptr,
        };

    if (vmaCreateAllocator(&VmaAllocatorCreateInfo, &this->Vk_VmaAllocator) != VK_SUCCESS)
    {
        panic( "Failed to create VMA allocator." )
    }

    return;
}

void Jafg::LFrontendVk::Vk_UpdateSamplers()
{
    LOG_VERBOSE(LogVulkan, "Updating Vulkan samplers.")

    this->Vk_DefaultSampler = vk::raii::Sampler{this->Vk_Device, {
        .magFilter = vk::Filter::eLinear, .minFilter = vk::Filter::eLinear,
        .mipmapMode = vk::SamplerMipmapMode::eLinear,
        .addressModeU = vk::SamplerAddressMode::eRepeat, .addressModeV = vk::SamplerAddressMode::eRepeat, .addressModeW = vk::SamplerAddressMode::eRepeat,
        .mipLodBias = 0.0f,
        .anisotropyEnable = vk::True, .maxAnisotropy = this->Vk_PhysicalDevice.getProperties().limits.maxSamplerAnisotropy,
        .compareEnable = vk::False, .compareOp = vk::CompareOp::eAlways,
        .minLod = 0.0f, // Increase for worse texture quality.
        .maxLod = VK_LOD_CLAMP_NONE,
        .borderColor = vk::BorderColor::eIntOpaqueBlack,
        }};

    return;
}

TOptional<vk::Format> Jafg::LFrontendVk::Vk_FindSupportedFormat(TArray<vk::Format> const& Candidates, vk::ImageTiling Tiling, vk::FormatFeatureFlags Features) const
{
    for (const auto Format : Candidates)
    {
        vk::FormatProperties Props{ this->Vk_PhysicalDevice.getFormatProperties(Format) };

        if (Tiling == vk::ImageTiling::eLinear && ((Props.linearTilingFeatures & Features) == Features))
        {
            return Format;
        }

        if (Tiling == vk::ImageTiling::eOptimal && ((Props.optimalTilingFeatures & Features) == Features))
        {
            return Format;
        }

        continue;
    }

    if (Tiling == vk::ImageTiling::eDrmFormatModifierEXT)
    {
        LOG_WARNING(LogVulkan, "Vk_FindSupportedFormat does not support checking for drm format modifier tiling. Querying will always fail.")
    }

    return {};
}

std::multimap<u64, vk::raii::PhysicalDevice> Jafg::LFrontendVk::Vk_RankPhysicalDevices(TArray<vk::raii::PhysicalDevice> const& PhysicalDevices) const
{
    std::multimap<u64, vk::raii::PhysicalDevice> Out;

    for (auto const& PhysicalDevice : PhysicalDevices)
    {
        u64 Rating{ 0 };

        bool bSupportsGeometryShaders{ false };
        bool bSupportsVulkan14{ false };
        bool bSupportsGraphicsQueue{ false };
        // bool bSupportsPresentQueue{ false }; // TODO: How can we check this? Or is this obsolete?
        bool bSupportsRequiredExtensions{ false };
        bool bSupportsRequiredFeatures{ false };

        auto Properties = PhysicalDevice.getProperties();
        auto Features = PhysicalDevice.template getFeatures2<
              vk::PhysicalDeviceFeatures2
            , vk::PhysicalDeviceVulkan11Features
            , vk::PhysicalDeviceVulkan13Features
            , vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
            >();
        auto Families = PhysicalDevice.getQueueFamilyProperties();
        auto Extensions = PhysicalDevice.enumerateDeviceExtensionProperties();

        if (Properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
        {
            Rating += 16'384;
        }
        else if (Properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
        {
            Rating += 8'192;
        }
        Rating += Properties.limits.maxImageDimension2D;

        if (PhysicalDevice.getFeatures().geometryShader == VK_TRUE)
        {
            bSupportsGeometryShaders = true;
        }

        if (Properties.apiVersion >= VK_API_VERSION_1_4)
        {
            bSupportsVulkan14 = true;
        }

        bSupportsGraphicsQueue = algo::any_of(Families, [](auto const& Qfp)
        {
            return !!(Qfp.queueFlags & vk::QueueFlagBits::eGraphics);
        });

        bSupportsRequiredExtensions = algo::all_of(this->Vk_RequiredDeviceExtensions, [&Extensions](auto const& RequiredPhysicalDeviceExtensions)
        {
            return algo::any_of(Extensions, [RequiredPhysicalDeviceExtensions](auto const& AvailablePhysicalDeviceExtension)
            {
                return std::strcmp(AvailablePhysicalDeviceExtension.extensionName, RequiredPhysicalDeviceExtensions);
            });
        });

        bSupportsRequiredFeatures = Features.template get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy
                                 && Features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters
                                 && Features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering
                                 && Features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

        if (   !bSupportsGeometryShaders
            || !bSupportsVulkan14
            || !bSupportsGraphicsQueue
            // || !bSupportsPresentQueue
            || !bSupportsRequiredExtensions
            || !bSupportsRequiredFeatures
        )
        {
            Rating = 0;
        }

        Out.insert(std::make_pair(Rating, PhysicalDevice));

        continue;
    }

    return Out;
}

void Jafg::LFrontendVk::Vk_Generate2DMipMaps(vk::Image Image, vk::Format Format, vk::Extent2D Extent, u32 MipLevels)
{
    check( MipLevels > 1 )

    if (vk::FormatProperties FormatProperties{ this->Vk_PhysicalDevice.getFormatProperties(Format) };
        !(FormatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
    {
        panicMsgf( "The texture image format [{}] does not support linear blitting.", vk::to_string(Format) )
    }

    auto Buffer{ this->Vk_BeginSingleTimeCommands() };

    vk::ImageMemoryBarrier Barrier{
        .srcAccessMask = vk::AccessFlagBits::eTransferWrite, .dstAccessMask = vk::AccessFlagBits::eTransferRead,
        .oldLayout = vk::ImageLayout::eTransferDstOptimal, .newLayout = vk::ImageLayout::eTransferSrcOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored, .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = Image,
        .subresourceRange =  {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = static_cast<uint32_t>(INDEX_NONE),
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
            },
        };

    for (auto Cursor{ 1uz }; Cursor < MipLevels; ++Cursor)
    {
        Barrier.subresourceRange.baseMipLevel = Cursor - 1;
        Barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
        Barrier.newLayout                     = vk::ImageLayout::eTransferSrcOptimal;
        Barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
        Barrier.dstAccessMask                 = vk::AccessFlagBits::eTransferRead;

        Buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, Barrier);

        vk::ArrayWrapper1D<vk::Offset3D, 2> offsets, dstOffsets;
        offsets[0]          = vk::Offset3D(0, 0, 0);
        offsets[1]          = vk::Offset3D(Extent.width, Extent.height, 1);
        dstOffsets[0]       = vk::Offset3D(0, 0, 0);
        dstOffsets[1]       = vk::Offset3D(Extent.width > 1 ? Extent.width / 2 : 1, Extent.height > 1 ? Extent.height / 2 : 1, 1);
        vk::ImageBlit blit  = {.srcSubresource = {}, .srcOffsets = offsets, .dstSubresource = {}, .dstOffsets = dstOffsets};
        blit.srcSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, Cursor - 1, 0, 1);
        blit.dstSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, Cursor, 0, 1);

        Buffer.blitImage(Image, vk::ImageLayout::eTransferSrcOptimal, Image, vk::ImageLayout::eTransferDstOptimal, {blit}, vk::Filter::eLinear);

        Barrier.oldLayout     = vk::ImageLayout::eTransferSrcOptimal;
        Barrier.newLayout     = vk::ImageLayout::eShaderReadOnlyOptimal;
        Barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        Barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        Buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, Barrier);

        if (Extent.width > 1)
        {
            Extent.width /= 2;
        }
        if (Extent.height > 1)
        {
            Extent.height /= 2;
        }

        continue;
    }

    Barrier.subresourceRange.baseMipLevel = MipLevels - 1;
    Barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
    Barrier.newLayout                     = vk::ImageLayout::eShaderReadOnlyOptimal;
    Barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
    Barrier.dstAccessMask                 = vk::AccessFlagBits::eShaderRead;

    Buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, Barrier);

    this->Vk_EndSingleTimeCommands(std::move(Buffer));

    return;
}

Jafg::LGraphicsDevicePipeline Jafg::LDevicePipelineFactory::Build()
{
    vk::PipelineMultisampleStateCreateInfo MultisamplingInfo{
        .rasterizationSamples = this->Frontend.Vk_GetMaxMsaaSampleCount(),
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

    TArray<vk::DescriptorSetLayout> DescriptorSetLayouts;
    for (auto Idx{0uz}; Idx < this->SharedDescriptorSetLayouts.size() + this->UniqueDescriptorSetLayouts.size(); ++Idx)
    {
        if (auto It{algo::find(this->SharedDescriptorSetLayouts, Idx, &TDescriptorSetLayout<vk::DescriptorSetLayout>::Binding)};
            It != this->SharedDescriptorSetLayouts.end())
        {
            DescriptorSetLayouts.emplace_back(It->DescriptorSetLayout);
            continue;
        }

        if (auto It{algo::find(this->UniqueDescriptorSetLayouts, Idx, &TDescriptorSetLayout<vk::raii::DescriptorSetLayout>::Binding)};
            It != this->UniqueDescriptorSetLayouts.end())
        {
            DescriptorSetLayouts.emplace_back(*It->DescriptorSetLayout);
            continue;
        }

        LOG_FATAL(LogRhi, "Failed to find descriptor set layout binding [{}].", Idx)
    }

    vk::raii::PipelineLayout Layout{
        Frontend.Vk_GetDevice(),
        vk::PipelineLayoutCreateInfo{
            .setLayoutCount = static_cast<u32>(DescriptorSetLayouts.size()),
            .pSetLayouts = DescriptorSetLayouts.data(),
            .pushConstantRangeCount = static_cast<u32>(this->PushConstantRange.size()),
            .pPushConstantRanges = this->PushConstantRange.data(),
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
            .pColorAttachmentFormats = &this->Frontend.Vk_GetSurfaceFormat().format,
            .depthAttachmentFormat = Frontend.Vk_GetPreferredDepthFormat(),
        }
    };

    TArray<vk::raii::DescriptorSetLayout> Temp; Temp.reserve(this->UniqueDescriptorSetLayouts.size());
    for (auto& DescriptorSetLayout : this->UniqueDescriptorSetLayouts)
    {
        Temp.emplace_back(std::move(DescriptorSetLayout.DescriptorSetLayout));
    }
    algo::orphan(&this->UniqueDescriptorSetLayouts);

    return LGraphicsDevicePipeline{
        .Pipeline = vk::raii::Pipeline{
            Frontend.Vk_GetDevice(),
            nullptr,
            Chain.get<vk::GraphicsPipelineCreateInfo>()
            },
        .Layout = std::move(Layout),
        .DescriptorSetLayouts = std::move(DescriptorSetLayouts),
        ._UniqueDescriptorSetLayout = std::move(Temp),
        };
}
