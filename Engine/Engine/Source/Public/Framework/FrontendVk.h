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
        VK_KHR_SURFACE_EXTENSION_NAME,
#if !IN_SHIPPING
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
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
