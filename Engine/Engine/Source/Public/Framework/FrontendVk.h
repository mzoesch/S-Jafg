// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/FrontendForward.h"
#include "Rhi/VkCommon.h"
#include "Framework/Frontend.h"

namespace Jafg
{

class LFrontendVk final : public LFrontendBase
{
public:

    void Initialize(LClassOuter* Outer);
    void TearDown();

    FORCEINLINE auto const& GetVkContext() const noexcept { return this->VkMyContext; }
    FORCEINLINE auto const& GetVkInstance() const noexcept { return this->VkMyInstance; }
    FORCEINLINE auto const& GetVkPhysicalDevice() const noexcept { return this->VkMyPhysicalDevice; }
    FORCEINLINE auto const& GetVkDevice() const noexcept { return this->VkMyDevice; }
    FORCEINLINE auto        GetVkGraphicsQueueFamilyIndex() const noexcept { return this->VkMyGraphicsQueueFamilyIndex; }
    FORCEINLINE auto        GetVkPresentQueueFamilyIndex() const noexcept { return this->VkMyPresentQueueFamilyIndex; }
    FORCEINLINE auto const& GetVkGraphicsQueue() const noexcept { return this->VkMyGraphicsQueue; }
    FORCEINLINE auto const& GetVkPresentQueue() const noexcept { return this->VkMyPresentQueue; }

    FORCEINLINE auto const& GetVma() const noexcept { return this->VmaMyAllocator; }

    ENGINE_API vk::raii::CommandBuffer VkBeginSingleTimeCommands(vk::CommandPool Pool) const;
    ENGINE_API void VkEndSingleTimeCommands(vk::CommandBuffer CommandBuffer) const;

    ENGINE_API LVmaBuffer VkCreateBuffer(vk::BufferCreateInfo CreateInfo, vk::MemoryPropertyFlags Flags, VmaMemoryUsage Usage = VMA_MEMORY_USAGE_AUTO);
    ENGINE_API LVmaDetailedBuffer VkCreateDetailedBuffer(vk::BufferCreateInfo CreateInfo, vk::MemoryPropertyFlags Flags, VmaMemoryUsage Usage = VMA_MEMORY_USAGE_AUTO);
    ENGINE_API LVmaMappedBuffer VkCreateMappedBuffer(vk::BufferCreateInfo CreateInfo, vk::MemoryPropertyFlags Flags, VmaAllocationCreateFlags VmaFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, VmaMemoryUsage Usage = VMA_MEMORY_USAGE_AUTO);

    ENGINE_API void VkCopyBuffer(vk::CommandPool Pool, vk::Buffer SrcBuffer, vk::Buffer DstBuffer, vk::BufferCopy BufferCopy) const;

    //# TODO: VkCreateStagingBuffer function.

    ENGINE_API  LVmaBuffer VkStageData(vk::BufferCopy BufferCopy, void const* Data, vk::BufferUsageFlags Usage, vk::CommandPool Pool /* just temp... */);
    FORCEINLINE LVmaBuffer VkStageVertexBuffer(vk::BufferCopy BufferCopy, void const* Data, vk::CommandPool Pool /* just temp... */,
        vk::BufferUsageFlags Usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer
        ) { return this->VkStageData(BufferCopy, Data, Usage, Pool); }
    FORCEINLINE LVmaBuffer VkStageIndexBuffer(vk::BufferCopy BufferCopy, void const* Data, vk::CommandPool Pool /* just temp... */,
        vk::BufferUsageFlags Usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer
        ) { return this->VkStageData(BufferCopy, Data, Usage, Pool); }
    FORCEINLINE LVmaBuffer VkStageUniformBuffer(vk::BufferCopy BufferCopy, void const* Data, vk::CommandPool Pool /* just temp... */,
        vk::BufferUsageFlags Usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer
        ) { return this->VkStageData(BufferCopy, Data, Usage, Pool); }

    ENGINE_API LVmaImage VkStage2dImage(
          i32 texWidth, i32 texHeight, i32 texChannels
        , stbi_uc* pixels
        , vk::CommandPool Pool /* just temp... */
        );

    ENGINE_API void CopyBufferToImage(vk::Buffer Buffer, vk::Image Image, u32 Width, u32 Height, vk::CommandPool Pool /* just temp... */);
    ENGINE_API LVmaImage VkCreateDeviceLocalImage(vk::ImageCreateInfo const& InInfo, vk::MemoryPropertyFlags Properties = vk::MemoryPropertyFlagBits::eDeviceLocal);

    ENGINE_API void VkTransitionImageLayout(vk::Image Image, vk::ImageLayout OldLayout, vk::ImageLayout NewLayout, vk::CommandPool Pool /* just temp... */);

    ENGINE_API vk::raii::ImageView CreateImageView(vk::Image Image, vk::Format Format);

private:

    void FetchAndCheckInstanceExtensions();
    void FetchAndCheckInstanceLayers();
    void CreateInstance();
#if !IN_SHIPPING
    void SetupDebugUtilsMessenger();
#endif /* !IN_SHIPPING */
    void PickPhysicalDevice();
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
    vk::raii::PhysicalDevice VkMyPhysicalDevice{ nullptr };

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
};

} /* ~Namespace Jafg */
