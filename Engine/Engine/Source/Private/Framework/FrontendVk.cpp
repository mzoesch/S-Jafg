// Copyright mzoesch. All rights reserved.

#define VMA_IMPLEMENTATION
#include "Framework/FrontendVk.h"

#if PLATFORM_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
#endif /* PLATFORM_WINDOWS */

#include <GLFW/glfw3.h>

#include "Stats/Stats.h"

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

void Jafg::LFrontendVk::Initialize(LClassOuter* Outer)
{
    STAT_CYCLE_FUNCTION()

    LOG_VERBOSE(LogSurface, "Initializing glfw.")

    check( Tasks::IsOnMasterThread() )

    if (!glfwInit())
    {
        panic( "Failed to initialize glfw." )
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwSetErrorCallback(::GlfwErrorCallback);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

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

    LOG_VERBOSE(LogVulkan, "Initializing Vk.")
    this->FetchAndCheckInstanceExtensions();
    this->FetchAndCheckInstanceLayers();
    this->CreateInstance();
#if !IN_SHIPPING
    this->SetupDebugUtilsMessenger();
#endif /* !IN_SHIPPING */

    LFrontendBase::Initialize(Outer);
    check( this->GetSurfaces().empty() == false )

    this->PickPhysicalDevice();
    this->CreateLogicalDevice();

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

    LOG_VERBOSE(LogSurface, "Terminating glfw.")
    glfwTerminate();

    return;
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
        .apiVersion = vk::ApiVersion14
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

    this->AvailablePhysicalDevices = vk::raii::PhysicalDevices{ this->VkMyInstance };

    if (this->AvailablePhysicalDevices.empty())
    {
        panic( "Failed to find any physical devices with Vulkan support." )
    }

    auto RankedPhysicalDevices = this->RankPhysicalDevices(this->AvailablePhysicalDevices);
    LOG_VERBOSE(LogVulkan, "Available physical devices ranked by suitability:")
    for (auto const& [Rating, PhysicalDevice] : RankedPhysicalDevices)
    {
        auto Properties = PhysicalDevice.getProperties();
        LOG_VERBOSE(LogVulkan, "    [{}] rated [{}]: {} (API v{}.{}.{}), Driver v{}.{}.{}",
            reinterpret_cast<void const*>(&*PhysicalDevice),
            Rating,
            LStringView{Properties.deviceName},
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

    this->VkMyPhysicalDevice = RankedPhysicalDevices.rbegin()->second;
    LOG_VERBOSE(LogVulkan, "Selected physical device [{}].",
        reinterpret_cast<void const*>(&*this->VkMyPhysicalDevice)
        )

    return;
}

void Jafg::LFrontendVk::CreateLogicalDevice()
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan logical device.")

    check( this->GetSurfaceCount() == 1 && "To create a logical device, exactly one surface is required at this time." )

    std::vector<vk::QueueFamilyProperties> QueueFamilyProperties = this->VkMyPhysicalDevice.getQueueFamilyProperties();

    auto GraphicsQueueFamilyProperty = algo::find_if(QueueFamilyProperties,  [](auto const& Qfp)
    {
        return (Qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
    });
    if (GraphicsQueueFamilyProperty == QueueFamilyProperties.end())
    {
        panic( "Failed to find a suitable graphics queue family." )
    }
    u32 GraphicsQueueFamilyIndex = static_cast<u32>(algo::distance(QueueFamilyProperties.begin(), GraphicsQueueFamilyProperty));
    if (GraphicsQueueFamilyIndex == QueueFamilyProperties.size())
    {
        panic( "Failed to find a suitable graphics queue family index." )
    }

    /* We prefer a combined graphics+present queue (because performance), but also separate ones are ok. */
    u32 PresentQueueFamilyIndex = this->VkMyPhysicalDevice.getSurfaceSupportKHR(GraphicsQueueFamilyIndex, *this->GetSurfaces().front()->GetVkSurface())
        ? GraphicsQueueFamilyIndex
        : static_cast<u32>(QueueFamilyProperties.size());

    if (PresentQueueFamilyIndex == QueueFamilyProperties.size())
    {
        /* Now try really hard to find a combined queue. */
        for (LSize Idx{ 0 }; Idx < QueueFamilyProperties.size(); ++Idx)
        {
            if (   (QueueFamilyProperties[Idx].queueFlags & vk::QueueFlagBits::eGraphics)
                && this->VkMyPhysicalDevice.getSurfaceSupportKHR(static_cast<u32>( Idx ), *this->GetSurfaces().front()->GetVkSurface())
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
            for (LSize Idx{ 0 }; Idx < QueueFamilyProperties.size(); ++Idx)
            {
                if (this->VkMyPhysicalDevice.getSurfaceSupportKHR(static_cast<u32>(Idx), *this->GetSurfaces().front()->GetVkSurface()))
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
        > FeaturesChain = {
        {}, // vk::PhysicalDeviceFeatures2
        {.shaderDrawParameters = VK_TRUE }, // vk::PhysicalDeviceVulkan11Features
        {.synchronization2 = VK_TRUE, .dynamicRendering = VK_TRUE}, // vk::PhysicalDeviceVulkan13Features
        {.extendedDynamicState = VK_TRUE} // vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
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

    this->VkMyDevice = vk::raii::Device{ this->VkMyPhysicalDevice, DeviceCreateInfo };
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
    VmaVulkanFunctions VmaVulkanFunc{
        .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr = vkGetDeviceProcAddr
        };

    VmaAllocatorCreateInfo VmaAllocatorCreateInfo{
        .physicalDevice = *this->VkMyPhysicalDevice,
        .device = *this->VkMyDevice,
        .pVulkanFunctions = &VmaVulkanFunc,
        .instance = *this->VkMyInstance
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

        bSupportsRequiredFeatures = Features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters
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
