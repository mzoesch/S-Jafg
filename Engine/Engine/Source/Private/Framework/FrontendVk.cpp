// Copyright mzoesch. All rights reserved.

/* Included for vkGetInstanceProcAddr and vkGetDeviceProcAddr */
#include <vulkan/vulkan.h>

#define VMA_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "Framework/FrontendVk.h"

#include <GLFW/glfw3.h>
#if PLATFORM_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
#endif /* PLATFORM_WINDOWS */

#include "Platform/PlatformMisc.h"
#include "Stats/Stats.h"
#include "Engine/Engine.h"
#include "User/UserPreferences.h"
#include "Rhi/VkAl.h"

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

    if (Severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
    {
        LOG_ERROR(LogVulkan, "[{}] Validation Layer [{}]: {}", CallbackData->messageIdNumber, CallbackData->pMessageIdName, CallbackData->pMessage)
    }
    else if (Severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
    {
        LOG_WARNING(LogVulkan, "[{}] Validation Layer [{}]: {}", CallbackData->messageIdNumber, CallbackData->pMessageIdName, CallbackData->pMessage)
    }
    else if (Type & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
    {
        LOG_WARNING(LogVulkan, "[{}] Performance Layer [{}]: {}", CallbackData->messageIdNumber, CallbackData->pMessageIdName, CallbackData->pMessage)
    }
    else if (Severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose)
    {
        LOG_VERBOSE(LogVulkan, "[{}] Info (Verbose) [{}]: {}", CallbackData->messageIdNumber, CallbackData->pMessageIdName, CallbackData->pMessage)
    }
    else
    {
        LOG_INFO(LogVulkan, "[{}] Info [{}]: {}", CallbackData->messageIdNumber, CallbackData->pMessageIdName, CallbackData->pMessage)
    }

    return VK_FALSE;
}
#endif /* !IN_SHIPPING */

void Jafg::Detail::FreeDeviceAllocation(vk::Buffer Handle, LDeviceAllocation Allocation) noexcept
{
    if (LAL_LIKELY(GEngine))
    {
        static VmaAllocator Vma{ nullptr };
        if (LAL_LIKELY(Vma))
        {
            check( GEngine->GetLocalEgo().GetFrontend().GetVma() == Vma )
        }
        else
        {
            Vma = GEngine->GetLocalEgo().GetFrontend().GetVma();
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
    if (LAL_LIKELY(GEngine))
    {
        static VmaAllocator Vma{ nullptr };
        if (LAL_LIKELY(Vma))
        {
            check( GEngine->GetLocalEgo().GetFrontend().GetVma() == Vma )
        }
        else
        {
            Vma = GEngine->GetLocalEgo().GetFrontend().GetVma();
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
        glfwGetMonitorPhysicalSize(Monitor, &Pv.SizeMm.X, &Pv.SizeMm.Y);
        glfwGetMonitorContentScale(Monitor, &Pv.ContentScale.X, &Pv.ContentScale.Y);
        glfwGetMonitorWorkarea(Monitor,
            &Pv.WorkareaOffsetPx.X, &Pv.WorkareaOffsetPx.Y,
            &Pv.WorkareaPx.X, &Pv.WorkareaPx.Y
            );
        Pv.Prefix = Lal::SprintF("{}-", MonitorIndex);
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
        Pv.Bits.X = VidMode->redBits;
        Pv.Bits.Y = VidMode->greenBits;
        Pv.Bits.Z = VidMode->blueBits;

        Pv.RefreshRateHz = VidMode->refreshRate;

        LOG_VERBOSE(LogSurface, "    Physical Monitor [{}{}]: {}x{}px @ {}hz, {}x{}mm, RGB=[{}|{}|{}]",
            Pv.Prefix, Pv.Name,
            Pv.WorkareaPx.X, Pv.WorkareaPx.Y,
            Pv.RefreshRateHz,
            Pv.SizeMm.X, Pv.SizeMm.Y,
            Pv.Bits.X, Pv.Bits.Y, Pv.Bits.Z
            )
        this->UsablePhysicalViewports.emplace_back(std::move(Pv));
    }

    LOG_VERBOSE(LogVulkan, "Initializing vulkan.")

    LOG_VERBOSE(LogVulkan, "Setting up Vulkan dynamic dispatch loader.")
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    this->FetchAndCheckInstanceExtensions();
    this->FetchAndCheckInstanceLayers();
    this->CreateInstance();
#if !IN_SHIPPING
    this->SetupDebugUtilsMessenger();
#endif /* !IN_SHIPPING */

    LOG_VERBOSE(LogVulkan, "Initializing Vulkan dispatch loader with instance.")
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*this->VkMyInstance);

    LFrontendBase::Initialize(Outer);
    check( this->GetSurfaces().empty() == false )

    this->PickPhysicalDevice();
    this->PickMaxMsaaSamples();
    this->CreateLogicalDevice();

    LOG_VERBOSE(LogVulkan, "Initializing Vulkan dispatch loader with device.")
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*this->VkMyDevice);

    this->CreateVma();

    LOG_VERBOSE(LogVulkan, "Creating transient command pool.")
    this->Vk_TransientCommandPool = vk::raii::CommandPool{this->VkMyDevice, {
        .flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = this->VkMyGraphicsQueueFamilyIndex // TODO: Queue that supports VK_QUEUE_TRANSFER_BIT
        }};

    LSlangCompilationRequest Req{};
    Req.In = LPath{ "/home/mzoesch/EDev/S-Jafg/Content/Shaders/Slang/Test.slang" };
    Req.Out = LPath{ "/home/mzoesch/EDev/S-Jafg/Content/Shaders/Spir-V/Test.spv" };
    Req.EntryPoints.reflexive_emplace_back("vertMain").emplace_back("fragMain");
    if (auto Rc{ this->HandleCompilationRequest_viaSlang(Req) }; Rc != 0)
    {
        panicMsgf( "Failed to compile shader [{}] via Slang [{}].", Req.In, Rc )
    }

    Req.In = LPath{ "Content/Shaders/Slang/VisualBox.slang" };
    Req.Out = LPath{ "Content/Shaders/Spir-V/VisualBox.spv" };
    if (auto Rc{ this->HandleCompilationRequest_viaSlang(Req) }; Rc != 0)
    {
        panicMsgf( "Failed to compile shader [{}] via Slang [{}].", Req.In, Rc )
    }

    for (auto& Surface : this->GetSurfaces())
    {
        Surface->LateSetupVk();
        continue;
    }

    return;
}

void Jafg::LFrontendVk::TearDown()
{
    LFrontendBase::TearDown();

    LOG_VERBOSE(LogVulkan, "Destroying VMA.")
    vmaDestroyAllocator(this->VmaMyAllocator);

    LOG_VERBOSE(LogSurface, "Terminating glfw.")
    glfwTerminate();

    return;
}

vk::raii::CommandBuffer Jafg::LFrontendVk::VkBeginSingleTimeCommands(vk::CommandPool Pool) const
{
    vk::CommandBufferAllocateInfo allocInfo{
        .commandPool = Pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
        };

    vk::raii::CommandBuffer commandBuffer = std::move(this->VkMyDevice.allocateCommandBuffers(allocInfo).front());

    vk::CommandBufferBeginInfo beginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
    commandBuffer.begin(beginInfo);

    return commandBuffer;
}

void Jafg::LFrontendVk::VkEndSingleTimeCommands(vk::CommandBuffer CommandBuffer) const
{
    CommandBuffer.end();

    vk::SubmitInfo submitInfo{ .commandBufferCount = 1, .pCommandBuffers = &CommandBuffer };
    this->VkMyGraphicsQueue.submit(submitInfo, nullptr);
    this->VkMyGraphicsQueue.waitIdle();

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
        this->VmaMyAllocator,
        Info,
        &AllocationCreateInfo,
        &Buffer,
        &Allocation,
        nullptr
        )};
    check( Res == VK_SUCCESS )

    return LDeviceBuffer{ Buffer, Allocation };
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
        this->VmaMyAllocator,
        Info,
        &AllocationCreateInfo,
        &Buffer,
        &Allocation,
        &AllocationInfo
        )};
    check( Res == VK_SUCCESS )

    return LDetailedDeviceBuffer{ Buffer, Allocation, std::move(AllocationInfo) };
}

Jafg::LMappedDeviceBuffer Jafg::LFrontendVk::Vk_CreateMappedBuffer(
      vk::BufferCreateInfo Info
    , vk::MemoryPropertyFlags Flags
    , VmaAllocationCreateFlags VmaFlags /* = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT */
    , VmaMemoryUsage Usage /* = VMA_MEMORY_USAGE_AUTO */
    ) const
{
    VkBuffer Buffer;
    VmaAllocation Allocation;
    VmaAllocationCreateInfo AllocationCreateInfo{
        .flags = VmaFlags,
        .usage = Usage,
        .requiredFlags = static_cast<VkMemoryPropertyFlags>(Flags),
        };

    VmaAllocationInfo AllocationInfo{};
    auto Res = vmaCreateBuffer(
        this->VmaMyAllocator,
        Info,
        &AllocationCreateInfo,
        &Buffer,
        &Allocation,
        &AllocationInfo
        );
    check( Res == VK_SUCCESS )
    check( AllocationInfo.pMappedData )

    return LMappedDeviceBuffer{ Buffer, Allocation, AllocationInfo.pMappedData };
}

void Jafg::LFrontendVk::Vk_CopyBuffer(vk::Buffer Src, vk::Buffer Dst, vk::BufferCopy BufferCopy, vk::CommandPool Pool /* = nullptr */) const
{
    check( Src && Dst )

    auto Buffer{this->VkBeginSingleTimeCommands(Pool ? Pool : this->Vk_TransientCommandPool)};
    Buffer.copyBuffer(Src, Dst, BufferCopy);
    this->VkEndSingleTimeCommands(Buffer);

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
        this->VmaMyAllocator,
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
        this->VmaMyAllocator,
        DeviceBufferCreateInfo,
        &DeviceAllocInfo,
        &DeviceBuffer,
        &DeviceAllocation,
        nullptr
        );
    check( Res == VK_SUCCESS )

    this->Vk_CopyBuffer(StagingBuffer, DeviceBuffer, Info.BufferCopy);

    vmaDestroyBuffer(this->VmaMyAllocator, StagingBuffer, StagingAllocation);

    return LDeviceBuffer{ DeviceBuffer, DeviceAllocation };
}

Jafg::LDeviceImage Jafg::LFrontendVk::VkCreateImage(vk::ImageCreateInfo const& Info, VmaAllocationCreateInfo const& AllocationCreateInfo)
{
    VkImage Image;
    VmaAllocation Allocation;

    auto Res = this->VmaMyAllocator->CreateImage(
        Info,
        &AllocationCreateInfo,
        nullptr,
        &Image,
        &Allocation,
        nullptr
        );
    check( Res == VK_SUCCESS )

    return LDeviceImage{ Image, Allocation };
}

Jafg::LDeviceImage Jafg::LFrontendVk::VkStage2dImage(i32 texWidth, i32 texHeight, i32 texChannels, stbi_uc* pixels, u32 MipLevels, vk::CommandPool Pool)
{
    check( pixels )

    vk::DeviceSize ImageSize{ static_cast<vk::DeviceSize>(texWidth) * static_cast<vk::DeviceSize>(texHeight) * static_cast<vk::DeviceSize>(texChannels) };

    auto StagingBuffer = this->Vk_CreateMappedBuffer(
            vk::BufferCreateInfo{
                .size = ImageSize,
                .usage = vk::BufferUsageFlagBits::eTransferSrc
            },
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
            );
    std::memcpy(StagingBuffer.Data, pixels, static_cast<size_t>(ImageSize));

    vk::ImageCreateInfo ImageCreateInfo{
        .flags = {},
        .imageType = vk::ImageType::e2D,
        .format = vk::Format::eR8G8B8A8Srgb,
        .extent = { static_cast<u32>(texWidth), static_cast<u32>(texHeight), 1 },
        .mipLevels = MipLevels,
        .arrayLayers = 1,
        .samples = vk::SampleCountFlagBits::e1,
        .tiling = vk::ImageTiling::eOptimal,
        .usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        .sharingMode = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
        };

    LDeviceImage Image = this->VkCreateDeviceLocalImage(ImageCreateInfo, vk::MemoryPropertyFlagBits::eDeviceLocal);

    this->VkTransitionImageLayout(Image.Buffer, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, MipLevels, Pool);
    this->CopyBufferToImage(StagingBuffer.Buffer, Image.Buffer, texWidth, texHeight, Pool);
    this->VkTransitionImageLayout(Image.Buffer, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, MipLevels, Pool);

    return Image;
}

void Jafg::LFrontendVk::CopyBufferToImage(vk::Buffer Buffer, vk::Image Image, u32 Width, u32 Height, vk::CommandPool Pool)
{
    auto CommandBuffer = this->VkBeginSingleTimeCommands(Pool);

    vk::BufferImageCopy Region{
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {vk::ImageAspectFlagBits::eColor, 0, 0, 1},
        .imageOffset = {0, 0, 0},
        .imageExtent = {Width, Height, 1}
        };

    CommandBuffer.copyBufferToImage(Buffer, Image, vk::ImageLayout::eTransferDstOptimal, Region);

    this->VkEndSingleTimeCommands(CommandBuffer);

    return;
}

Jafg::LDeviceImage Jafg::LFrontendVk::VkCreateDeviceLocalImage(vk::ImageCreateInfo const& InInfo, vk::MemoryPropertyFlags Properties)
{
    VkImage Image;
    VmaAllocation Allocation;
    VmaAllocationCreateInfo AllocationCreateInfo{
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        .requiredFlags = static_cast<VkMemoryPropertyFlags>(Properties),
        };

    auto Res = this->VmaMyAllocator->CreateImage(
        InInfo,
        &AllocationCreateInfo,
        nullptr,
        &Image,
        &Allocation,
        nullptr
        );
    check( Res == VK_SUCCESS )

    return { Image, Allocation };
}

void Jafg::LFrontendVk::VkTransitionImageLayout(vk::Image Image, vk::ImageLayout OldLayout, vk::ImageLayout NewLayout, u32 MipLevels
    , vk::CommandPool Pool)
{
    auto Buffer = this->VkBeginSingleTimeCommands(Pool);

    vk::ImageMemoryBarrier Barrier{
        .oldLayout = OldLayout,
        .newLayout = NewLayout,
        .image = Image,
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = MipLevels,
            .baseArrayLayer = 0,
            .layerCount = 1
            }
        };

    vk::PipelineStageFlags SourceStage;
    vk::PipelineStageFlags DestinationStage;

    if (OldLayout == vk::ImageLayout::eUndefined && NewLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        Barrier.srcAccessMask = {};
        Barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        SourceStage      = vk::PipelineStageFlagBits::eTopOfPipe;
        DestinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (OldLayout == vk::ImageLayout::eTransferDstOptimal && NewLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        Barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        Barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        SourceStage      = vk::PipelineStageFlagBits::eTransfer;
        DestinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else
    {
        panicMsgf( "Unsupported layout transition [{} => {}].", vk::to_string(OldLayout), vk::to_string(NewLayout) )
    }

    Buffer.pipelineBarrier(SourceStage, DestinationStage, {}, {}, nullptr, Barrier);

    this->VkEndSingleTimeCommands(Buffer);

    return;
}

vk::raii::ImageView Jafg::LFrontendVk::CreateImageView2D(vk::Image Image, vk::Format Format, vk::ImageAspectFlags AspectFlags, u32 MipLevels)
{
    vk::ImageViewCreateInfo CreateInfo{
        .image = Image,
        .viewType = vk::ImageViewType::e2D,
        .format = Format,
        .subresourceRange = {
            .aspectMask = AspectFlags,
            .baseMipLevel = 0,
            .levelCount = MipLevels,
            .baseArrayLayer = 0,
            .layerCount = 1
            }
        };

    return vk::raii::ImageView{ this->VkMyDevice, CreateInfo };
}

vk::Format Jafg::LFrontendVk::FindSupportedFormat(TArray<vk::Format> const& Candidates, vk::ImageTiling Tiling,
        vk::FormatFeatureFlags Features) const
{
    for (const auto format : Candidates)
    {
        vk::FormatProperties props = this->Vk_PhysicalDevice.getFormatProperties(format);

        if (Tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & Features) == Features) {
            return format;
        }
        if (Tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & Features) == Features) {
            return format;
        }

        continue;
    }

    panic( "Failed to find supported format." )
}

vk::Format Jafg::LFrontendVk::FindDepthFormat() const
{
    return this->FindSupportedFormat(
     {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
         vk::ImageTiling::eOptimal,
         vk::FormatFeatureFlagBits::eDepthStencilAttachment
     );
}

bool Jafg::LFrontendVk::HasStencilComponent(vk::Format Format) const
{
    return Format == vk::Format::eD32SfloatS8Uint || Format == vk::Format::eD24UnormS8Uint;
}

vk::SampleCountFlagBits Jafg::LFrontendVk::CalculateMaxUsableSampleCount() const
{
    vk::PhysicalDeviceProperties PhysicalDeviceProperties{ this->Vk_PhysicalDevice.getProperties() };

    vk::SampleCountFlags Counts{ PhysicalDeviceProperties.limits.framebufferColorSampleCounts & PhysicalDeviceProperties.limits.framebufferDepthSampleCounts };
    if (Counts & vk::SampleCountFlagBits::e64) { return vk::SampleCountFlagBits::e64; }
    if (Counts & vk::SampleCountFlagBits::e32) { return vk::SampleCountFlagBits::e32; }
    if (Counts & vk::SampleCountFlagBits::e16) { return vk::SampleCountFlagBits::e16; }
    if (Counts & vk::SampleCountFlagBits::e8)  { return vk::SampleCountFlagBits::e8; }
    if (Counts & vk::SampleCountFlagBits::e4)  { return vk::SampleCountFlagBits::e4; }
    if (Counts & vk::SampleCountFlagBits::e2)  { return vk::SampleCountFlagBits::e2; }

    return vk::SampleCountFlagBits::e1;
}

u32 Jafg::LFrontendVk::Vk_FindMemoryType(u32 Filter, vk::MemoryPropertyFlags Properties) const
{
    for (u32 Idx{ 0 }; Idx < this->Vk_PhysicalDeviceMemoryProperties.memoryTypeCount; ++Idx)
    {
        if ((Filter & (1 << Idx)) && (this->Vk_PhysicalDeviceMemoryProperties.memoryTypes[Idx].propertyFlags & Properties) == Properties)
        {
            return Idx;
        }
    }

    panic( "Failed to find suitable memory type." )
}

i64 Jafg::LFrontendVk::HandleCompilationRequest_viaSlang(LSlangCompilationRequest const& Request)
{
    return this->HandleCompilationRequest_viaSlang(
        Lal::SprintF("Content/.Slang{}_{}/bin/slangc{}",
            PlatformMisc::GetTargetPlatform(),
            PlatformMisc::GetTargetArchitecture(),
#if PLATFORM_WINDOWS
            ".exe"
#else /* PLATFORM_WINDOWS */
            ""
#endif /* !PLATFORM_WINDOWS */
            ),
        Request
        );
}

i64 Jafg::LFrontendVk::HandleCompilationRequest_viaSlang(LPath const& Slang, LSlangCompilationRequest const& Request)
{
    std::ostringstream SS;
    SS << Slang.string();
    SS << " " << Request.In.string();
    SS << " -o " << Request.Out.string();
    SS << " -target " << Request.Target;
    SS << " -profile " << Request.Profile;
    SS << " -emit-spirv-directly";
    if (Request.EntryPoints.empty() == false)
    {
        SS << " -fvk-use-entrypoint-name";
        for (auto const& EntryPoint : Request.EntryPoints)
        {
            SS << " -entry " << EntryPoint;
        }
    }

    Finder::CreateDirectories(Request.Out.parent_path());

    LString CommandLine{ SS.str() };
    LOG_VERBOSE(LogSystem, "Executing: [{}].", CommandLine)

    return static_cast<i64>(std::system(CommandLine.c_str()));
}

void Jafg::LFrontendVk::FetchAndCheckInstanceExtensions()
{
    LOG_VERBOSE(LogVulkan, "Refetching available instance extensions.")

    this->AvailableInstanceExtensions = this->VkMyContext.enumerateInstanceExtensionProperties();
    LOG_VERBOSE(LogVulkan, "Available Vulkan instance extensions:")
    for (auto const& Extension : this->AvailableInstanceExtensions)
    {
        LOG_VERBOSE(LogVulkan, "    {} spec[{}]", LStringView{Extension.extensionName}, Extension.specVersion)
    }

    u32 Glfw3ExtensionCount{ 0 };
    auto Glfw3Extensions{ glfwGetRequiredInstanceExtensions(&Glfw3ExtensionCount) };
    for (u32 Idx{ 0 }; Idx < Glfw3ExtensionCount; ++Idx)
    {
        LString Glfw3ExtensionStr{ Glfw3Extensions[Idx] };
        if (algo::contains(this->RequiredInstanceExtensions, Glfw3ExtensionStr) == false)
        {
            this->RequiredInstanceExtensions.emplace_back(std::move(Glfw3ExtensionStr));
        }
        continue;
    }

    LOG_VERBOSE(LogVulkan, "Required Vulkan instance extensions:")
    for (auto const& Extension : this->RequiredInstanceExtensions)
    {
        LOG_VERBOSE(LogVulkan, "    {}", Extension)
    }

    for (LString const& Extension : this->RequiredInstanceExtensions)
    {
        bool bFound{ false };
        for (auto const& AvailableExtension : AvailableInstanceExtensions)
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

void Jafg::LFrontendVk::FetchAndCheckInstanceLayers()
{
    LOG_VERBOSE(LogVulkan, "Refetching available instance layers.")

    this->AvailableInstanceLayers = this->VkMyContext.enumerateInstanceLayerProperties();
    LOG_VERBOSE(LogVulkan, "Available Vulkan instance layers:")
    for (auto const& Layer : this->AvailableInstanceLayers)
    {
        LOG_VERBOSE(LogVulkan, "    {} spec[{}]", LStringView{Layer.layerName}, Layer.specVersion)
    }

    LOG_VERBOSE(LogVulkan, "Required Vulkan instance layers:")
    for (auto const& Layer : this->RequiredInstanceLayers)
    {
        LOG_VERBOSE(LogVulkan, "    {}", Layer)
    }

    for (LString const& Layer : this->RequiredInstanceLayers)
    {
        bool bFound{ false };
        for (auto const& AvailableLayer : AvailableInstanceLayers)
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

void Jafg::LFrontendVk::CreateInstance()
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan instance.")

    constexpr vk::ApplicationInfo Info{
        .pApplicationName = "S-Jafg @mzoesch",
        .applicationVersion = VK_MAKE_VERSION( PRIVATE_ENGINE_VERSION_MAJOR, PRIVATE_ENGINE_VERSION_MINOR, PRIVATE_ENGINE_VERSION_PATCH ),
        .pEngineName = "Jafg Engine",
        .engineVersion = VK_MAKE_VERSION( PRIVATE_ENGINE_VERSION_MAJOR, PRIVATE_ENGINE_VERSION_MINOR, PRIVATE_ENGINE_VERSION_PATCH ),
        .apiVersion = ::Vk_ApiVersion
        };

    TArray<char const*> RequiredInstanceExtensions_c_str; RequiredInstanceExtensions_c_str.reserve(this->RequiredInstanceExtensions.size());
    algo::for_each(this->RequiredInstanceExtensions, [&RequiredInstanceExtensions_c_str](LString const& Extension)
    {
        RequiredInstanceExtensions_c_str.emplace_back(Extension.c_str());
    });

    TArray<char const*> RequiredInstanceLayers_c_str; RequiredInstanceLayers_c_str.reserve(this->RequiredInstanceLayers.size());
    algo::for_each(this->RequiredInstanceLayers, [&RequiredInstanceLayers_c_str](LString const& Layer)
    {
        RequiredInstanceLayers_c_str.emplace_back(Layer.c_str());
    });

    vk::InstanceCreateInfo CreateInfo{
        .pApplicationInfo = &Info,
        .enabledLayerCount = static_cast<u32>(RequiredInstanceLayers_c_str.size()),
        .ppEnabledLayerNames = RequiredInstanceLayers_c_str.data(),
        .enabledExtensionCount = static_cast<u32>(RequiredInstanceExtensions_c_str.size()),
        .ppEnabledExtensionNames = RequiredInstanceExtensions_c_str.data(),
        };

    this->VkMyInstance = vk::raii::Instance{ this->VkMyContext, CreateInfo };

    return;
}

#if !IN_SHIPPING
void Jafg::LFrontendVk::SetupDebugUtilsMessenger()
{
    LOG_VERBOSE(LogVulkan, "Setting up Vulkan debug utils messenger ext.")

    vk::DebugUtilsMessageSeverityFlagsEXT SeverityFlags =
          vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

    vk::DebugUtilsMessageTypeFlagsEXT TypeFlags =
          vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
        | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        // | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
        ;

    vk::DebugUtilsMessengerCreateInfoEXT CreateInfo{
        .messageSeverity = SeverityFlags,
        .messageType = TypeFlags,
        .pfnUserCallback = &::Hermes,
        .pUserData = nullptr
        };

    this->VkMyDebugUtilsMessenger = this->VkMyInstance.createDebugUtilsMessengerEXT(CreateInfo);

    return;
}
#endif /* !IN_SHIPPING */

void Jafg::LFrontendVk::PickPhysicalDevice()
{
    LOG_VERBOSE(LogVulkan, "Picking Vulkan physical device.")

    this->Vk_PhysicalDevice = nullptr;

    this->AvailablePhysicalDevices = this->VkMyInstance.enumeratePhysicalDevices();

    if (this->AvailablePhysicalDevices.empty())
    {
        panic( "Failed to find any physical devices with Vulkan support." )
    }

    auto RankedPhysicalDevices = this->RankPhysicalDevices(this->AvailablePhysicalDevices);
    LOG_VERBOSE(LogVulkan, "Available physical devices ranked by suitability:")
    for (auto const& [Rating, PhysicalDevice] : RankedPhysicalDevices)
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

    if (RankedPhysicalDevices.rbegin()->first == 0)
    {
        panicMsgf( "Failed to find a suitable physical device." )
    }

    if (const auto Prefs{ GetDefault<JUserPreferences>() }; Prefs->PreferredPhysicalDevice.empty() == false)
    {
        for (auto const& [Rating, PhysicalDevice] : RankedPhysicalDevices)
        {
            auto Properties = PhysicalDevice.getProperties();
            if (Prefs->PreferredPhysicalDevice == Properties.deviceName)
            {
                if (Rating == 0)
                {
                    LOG_WARNING(LogVulkan, "Preferred physical device [{}] found but is no longer suitable. Clearing user prefs and falling back to best rated device.",
                        Prefs->PreferredPhysicalDevice
                        )
                    GetMutableDefault<JUserPreferences>()->PreferredPhysicalDevice.clear();
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
            Prefs->PreferredPhysicalDevice
            )
        GetMutableDefault<JUserPreferences>()->PreferredPhysicalDevice.clear();
    }

    if (!*this->Vk_PhysicalDevice)
    {
        this->Vk_PhysicalDevice = RankedPhysicalDevices.rbegin()->second;
        LOG_VERBOSE(LogVulkan, "Selected physical device [{}].",
            LStringView{this->Vk_PhysicalDevice.getProperties().deviceName}
            )
    }

    this->Vk_PhysicalDeviceMemoryProperties = this->Vk_PhysicalDevice.getMemoryProperties();
    LOG_VERBOSE(LogVulkan, "Physical device memory properties:")
    for (u32 Idx{ 0 }; Idx < this->Vk_PhysicalDeviceMemoryProperties.memoryTypeCount; ++Idx)
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

void Jafg::LFrontendVk::PickMaxMsaaSamples()
{
    this->VkMsaaSamples = this->CalculateMaxUsableSampleCount();
    LOG_VERBOSE(LogVulkan, "Max usable sample count: [{}].", static_cast<u32>(this->VkMsaaSamples))

    return;
}

void Jafg::LFrontendVk::CreateLogicalDevice()
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan logical device.")

    // TODO: This is no longer a requirement, right??
    check( this->GetSurfaceCount() == 1 && "To create a logical device, exactly one surface is required at this time." )

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
    if (LAL_UNLIKELY(GraphicsQueueFamilyIndex == QueueFamilyProperties.size()))
    {
        panic( "Failed to find a suitable graphics queue family." )
    }

    /* We prefer a combined graphics+present queue (because performance), but also separate ones are ok. */
    u32 PresentQueueFamilyIndex = this->Vk_PhysicalDevice.getSurfaceSupportKHR(GraphicsQueueFamilyIndex, *this->GetSurfaces().front()->Vk_GetSurface())
        ? GraphicsQueueFamilyIndex
        : static_cast<u32>(QueueFamilyProperties.size());

    if (PresentQueueFamilyIndex == QueueFamilyProperties.size())
    {
        /* Now try really hard to find a combined queue. */
        for (auto Idx{ 0uz }; Idx < QueueFamilyProperties.size(); ++Idx)
        {
            if (   (QueueFamilyProperties[Idx].queueFlags & vk::QueueFlagBits::eGraphics)
                && this->Vk_PhysicalDevice.getSurfaceSupportKHR(static_cast<u32>( Idx ), *this->GetSurfaces().front()->Vk_GetSurface())
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
                if (this->Vk_PhysicalDevice.getSurfaceSupportKHR(static_cast<u32>(Idx), *this->GetSurfaces().front()->Vk_GetSurface()))
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
        .enabledExtensionCount = static_cast<u32>(this->RequiredDeviceExtensions.size()),
        .ppEnabledExtensionNames = this->RequiredDeviceExtensions.data(),
        };

    this->VkMyDevice = vk::raii::Device{ this->Vk_PhysicalDevice, DeviceCreateInfo };
    this->VkMyGraphicsQueue = vk::raii::Queue{ this->VkMyDevice, GraphicsQueueFamilyIndex, 0 };
    this->VkMyPresentQueue = vk::raii::Queue{ this->VkMyDevice, PresentQueueFamilyIndex, 0 };

    this->VkMyGraphicsQueueFamilyIndex = GraphicsQueueFamilyIndex;
    this->VkMyPresentQueueFamilyIndex = PresentQueueFamilyIndex;

    LOG_VERBOSE(LogVulkan, "Finished loading logical device.")

    LOG_VERBOSE(LogVulkan, "Graphics Queue Family Index: [{}].",  GraphicsQueueFamilyIndex)
    LOG_VERBOSE(LogVulkan, "Present  Queue Family Index: [{}].",  PresentQueueFamilyIndex)

    return;
}

void Jafg::LFrontendVk::CreateVma()
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
        .device = *this->VkMyDevice,
        .preferredLargeHeapBlockSize = 0,
        .pAllocationCallbacks = nullptr,
        .pDeviceMemoryCallbacks = nullptr,
        .pHeapSizeLimit = nullptr,
        .pVulkanFunctions = &VulkanFunctions,
        .instance = *this->VkMyInstance,
        .vulkanApiVersion = VK_API_VERSION_1_4,
        .pTypeExternalMemoryHandleTypes = nullptr,
        };

    if (vmaCreateAllocator(&VmaAllocatorCreateInfo, &this->VmaMyAllocator) != VK_SUCCESS)
    {
        panic( "Failed to create VMA allocator." )
    }

    return;
}

std::multimap<u64, vk::raii::PhysicalDevice> Jafg::LFrontendVk::RankPhysicalDevices(TArray<vk::raii::PhysicalDevice> const& PhysicalDevices) const
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

        bSupportsRequiredExtensions = algo::all_of(this->RequiredDeviceExtensions, [&Extensions](auto const& RequiredPhysicalDeviceExtensions)
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
