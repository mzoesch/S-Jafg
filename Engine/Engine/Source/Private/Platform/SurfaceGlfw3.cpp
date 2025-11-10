// Copyright mzoesch. All rights reserved.

#if JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER

#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "Engine/CoreGlobals.h"
#include "Engine/Engine.h"
#include "Async/TaskUtility.h"

#define VMA_IMPLEMENTATION
#include "Rhi/RhiVendorInclude.h"

#if PLATFORM_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
#endif /* PLATFORM_WINDOWS */

#include "User/Input/GlfwInputTranslation.h"
#include "Widgets/Viewport.h"
#include "Stats/Stats.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace
{

bool bInitializedGlfw{ false };

void GlfwErrorCallback(i32 Error, char const* Description)
{
    panicMsgf("GLFW Error (code {}): {}", Error, Description)
}

} /* ~Namespace <Anonymous> */

#if !IN_SHIPPING
static VKAPI_ATTR VkBool32 VKAPI_CALL Hermes(
    VkDebugUtilsMessageSeverityFlagBitsEXT Severity,
    VkDebugUtilsMessageTypeFlagsEXT Type,
    VkDebugUtilsMessengerCallbackDataEXT const* CallbackData,
    void* UserData
    )
{
    (void)UserData;

    if (Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        LOG_ERROR(LogVulkan, "[{}] Validation Layer [{}]: {}", CallbackData->messageIdNumber, CallbackData->pMessageIdName, CallbackData->pMessage)
    }
    else if (Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        LOG_WARNING(LogVulkan, "[{}] Validation Layer [{}]: {}", CallbackData->messageIdNumber, CallbackData->pMessageIdName, CallbackData->pMessage)
    }
    else if (Type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
    {
        LOG_WARNING(LogVulkan, "[{}] Performance Layer [{}]: {}", CallbackData->messageIdNumber, CallbackData->pMessageIdName, CallbackData->pMessage)
    }
    else if (Severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
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

namespace Jafg::Private
{

struct LGlfw3Bridge final
{
    LGlfw3Bridge() = delete;
    PROHIBIT_REALLOC_OF_ANY_FORM(LGlfw3Bridge)
    ~LGlfw3Bridge() = delete;

    static void WindowCloseCallback(GLFWwindow* Window)
    {
        glfwSetWindowShouldClose(Window, GLFW_TRUE);
    }

    static void WindowSizeCallback(GLFWwindow* Window, const i32 Width, const i32 Height)
    {
        LOG_WARNING(LogTemporal, "Width: {}, Height: {}", Width, Height)
    }

    static void WindowFocusCallback(GLFWwindow* Window, const i32 Focused)
    {
        LOG_WARNING(LogTemporal, "Focused: {}", Focused)
    }

    static void CharCallback(::GLFWwindow* Window, const u32 Codepoint)
    {
        LOG_WARNING(LogTemporal, "Codepoint: {}", Codepoint)
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->CharCallback(Codepoint);
    }

    static void KeyCallback(::GLFWwindow* Window, const i32 Key, const i32 Scancode, const i32 Action, const i32 Mods)
    {
        LOG_WARNING(LogTemporal, "Key: {}, Scancode: {}, Action: {}, Mods: {}", Key, Scancode, Action, Mods)
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->KeyCallback(Key, Scancode, Action, Mods);
    }

    static void CursorPosCallback(::GLFWwindow* Window, const f64 XPos, const f64 YPos)
    {
        LOG_WARNING(LogTemporal, "XPos: {}, YPos: {}", XPos, YPos)
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->MouseCallback(XPos, YPos);
    }

    static void MouseButtonCallback(::GLFWwindow* Window, const i32 Button, const i32 Action, const i32 Mods)
    {
        LOG_WARNING(LogTemporal, "Button: {}, Action: {}, Mods: {}", Button, Action, Mods)
    }

    static void CursorEnterCallback(::GLFWwindow* Window, const i32 Entered)
    {
        LOG_WARNING(LogTemporal, "Entered: {}", Entered)
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->MouseEnterCallback(Entered);
    }

    static void FramebufferSizeCallback(::GLFWwindow* Window, const i32 Width, const i32 Height)
    {
        LOG_WARNING(LogTemporal, "Width: {}, Height: {}", Width, Height)
        checkSlow( static_cast<::Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
        static_cast<Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->FramebufferSizeCallback(Width, Height);
    }

    static void ScrollCallback(::GLFWwindow* Window, const f64 XOffset, const f64 YOffset)
    {
        LOG_WARNING(LogTemporal, "XOffset: {}, YOffset: {}", XOffset, YOffset)
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->ScrollCallback(XOffset, YOffset);
    }
};

} /* ~Namespace Jafg::Private */

Jafg::LSurfaceGlfw3::~LSurfaceGlfw3()
{
    if (this->Handle)
    {
        this->TearDown();
    }

    return;
}

void Jafg::LSurfaceGlfw3::Initialize()
{
    STAT_CYCLE_FUNCTION()

    Super::Initialize();

    check( Tasks::IsOnMasterThread() )

    if (bInitializedGlfw == false)
    {
        if (!glfwInit())
        {
            panic( "Failed to initialize glfw." )
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        glfwSetErrorCallback(::GlfwErrorCallback);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        const int Platform{ glfwGetPlatform() };
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

        bInitializedGlfw = true;
    }

    if (glfwGetPlatform() == GLFW_PLATFORM_WAYLAND)
    {
        this->SetPlatformSupportsRepeatedKey(false);
    }

    if (this->GetHumanReadableName() == "Transient")
    {
        this->SetHumanReadableName("Jafg - @mzoesch");
    }

    {
        STAT_QUICK_CYCLE_START("Glfw3WindowCreation")
        // Min 640 475 - Default 1280 720
        this->Handle = glfwCreateWindow(855, 475, this->GetHumanReadableName().c_str(), nullptr, nullptr);
    }
    if (this->Handle == nullptr)
    {
        panic( "Failed to create glfw window." )
    }

    glfwSetWindowUserPointer(this->Handle, this);

    glfwSetWindowCloseCallback(this->Handle, Private::LGlfw3Bridge::WindowCloseCallback);
    glfwSetWindowSizeCallback(this->Handle, Private::LGlfw3Bridge::WindowSizeCallback);
    glfwSetWindowFocusCallback(this->Handle, Private::LGlfw3Bridge::WindowFocusCallback);
    glfwSetCharCallback(this->Handle, Private::LGlfw3Bridge::CharCallback);
    // if (this->IsPlatformSupportsRepeatedKey())
        glfwSetKeyCallback(this->Handle, Private::LGlfw3Bridge::KeyCallback);
    glfwSetCursorPosCallback(this->Handle, Private::LGlfw3Bridge::CursorPosCallback);
    glfwSetMouseButtonCallback(this->Handle, Private::LGlfw3Bridge::MouseButtonCallback);
    glfwSetCursorEnterCallback(this->Handle, Private::LGlfw3Bridge::CursorEnterCallback);
    glfwSetFramebufferSizeCallback(this->Handle, Private::LGlfw3Bridge::FramebufferSizeCallback);
    glfwSetScrollCallback(this->Handle, Private::LGlfw3Bridge::ScrollCallback);

    /* TODO: Do we really want this?? */
    glfwSetInputMode(this->Handle, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(this->Handle, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    if (GEngine)
    {
        this->SetVSync(GetDefault<JUserPreferences>());
    }
    else
    {
        Tasks::Make(ENamedThreads::Master, ETaskTime::AfterCorePackageLoad, [this](void)
        {
            this->SetVSync(GetDefault<JUserPreferences>()->bVSyncEnabled);
        });
    }

#if PLATFORM_WINDOWS
    const HWND NativeWindowHandle = glfwGetWin32Window(this->Handle);
    check( NativeWindowHandle )
    const u32 PlatformDpi = ::GetDpiForWindow(NativeWindowHandle);
#else /* PLATFORM_WINDOWS */
    const u32 PlatformDpi = 96; // Sketchy
#endif /* !PLATFORM_WINDOWS */
    this->GetViewport().SetPlatformDpi(static_cast<f32>(PlatformDpi));
    const LIntVector2 WindowDimensions = this->GetDimensions();
    // glViewport(0, 0, WindowDimensions.X, WindowDimensions.Y);
    this->GetViewport().ChangeDimensions(WindowDimensions);
    LOG_VERBOSE(LogSurface, "Glfw3 Window Created. Dimensions: [{}x{}], DPI: [{}]", WindowDimensions.X, WindowDimensions.Y, PlatformDpi)

    this->GetViewport().SetBackgroundColor(Lal::LLinearColor::Black);

    LOG_VERBOSE(LogRhi, "Initializing volk for Vulkan RHI.")
    if (volkInitialize() != VK_SUCCESS)
    {
        panic( "Failed to initialize volk." )
    }

    u32 AvailableExtensionCount{ 0 };
    if (vkEnumerateInstanceExtensionProperties(nullptr, &AvailableExtensionCount, nullptr))
    {
        panic( "Failed to enumerate instance extensions." )
    }
    LOG_VERBOSE(LogVulkan, "Instance Extension Count [{}]. Available extensions:", AvailableExtensionCount)
    TArray<VkExtensionProperties> AvailableExtensions(AvailableExtensionCount);
    if (vkEnumerateInstanceExtensionProperties(nullptr, &AvailableExtensionCount, AvailableExtensions.data()))
    {
        panic( "Failed to enumerate instance extensions." )
    }
    for (VkExtensionProperties const& Extension : AvailableExtensions)
    {
        LOG_VERBOSE(LogVulkan, "    {} spec[{}]", Extension.extensionName, Extension.specVersion)
    }

    TArray<LString> RequiredExtensions;
    RequiredExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);

#if !IN_SHIPPING
    RequiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    if (this->bVkMyInstanceLayerAddressBindings)
    {
        RequiredExtensions.emplace_back(VK_EXT_DEVICE_ADDRESS_BINDING_REPORT_EXTENSION_NAME);
    }
#endif /* !IN_SHIPPING */

    u32 glfw3ExtensionCount{ 0 };
    char const** glfw3Extensions{ glfwGetRequiredInstanceExtensions(&glfw3ExtensionCount) };
    for (u32 Idx{ 0 }; Idx < glfw3ExtensionCount; ++Idx)
    {
        LString Glfw3Extension{ glfw3Extensions[Idx] };
        if (algo::contains(RequiredExtensions, Glfw3Extension) == false)
        {
            RequiredExtensions.emplace_back(std::move(Glfw3Extension));
        }
        continue;
    }
    LOG_VERBOSE(LogVulkan, "Instance required extensions:")
    for (LString const& Extension : RequiredExtensions)
    {
        LOG_VERBOSE(LogVulkan, "    {}", Extension)
    }

    for (LString const& Extension : RequiredExtensions)
    {
        bool bFound{ false };
        for (auto const& AvailableExtension : AvailableExtensions)
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

    u32 InstanceLayerCount{ 0 };
    if (vkEnumerateInstanceLayerProperties(&InstanceLayerCount, nullptr) != VK_SUCCESS)
    {
        panic( "Failed to enumerate instance layers." )
    }
    LOG_VERBOSE(LogVulkan, "Instance Layer Count [{}]. Available layers:", InstanceLayerCount)
    TArray<VkLayerProperties> AvailableInstanceLayers(InstanceLayerCount);
    if (vkEnumerateInstanceLayerProperties(&InstanceLayerCount, AvailableInstanceLayers.data()) != VK_SUCCESS)
    {
        panic( "Failed to enumerate instance layers." )
    }
    for (VkLayerProperties const& InstanceLayer : AvailableInstanceLayers)
    {
        LOG_VERBOSE(LogVulkan, "    {} spec[{}]", InstanceLayer.layerName, InstanceLayer.specVersion)
    }

    TArray<LString> RequiredInstanceLayers;
#if !IN_SHIPPING
    RequiredInstanceLayers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif /* !IN_SHIPPING */
    LOG_VERBOSE(LogVulkan, "Instance required layers:")
    for (LString const& Layer : RequiredInstanceLayers)
    {
        LOG_VERBOSE(LogVulkan, "    {}", Layer)
    }

    for (LString const& Layer : RequiredInstanceLayers)
    {
        bool bFound{ false };
        for (auto const& AvailableInstanceLayer : AvailableInstanceLayers)
        {
            if (Layer == AvailableInstanceLayer.layerName)
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

    TArray<char const*> RequiredInstanceLayers_c_str; RequiredInstanceLayers_c_str.reserve(RequiredInstanceLayers.size());
    algo::for_each(RequiredInstanceLayers, [&RequiredInstanceLayers_c_str](LString const& Extension)
    {
        RequiredInstanceLayers_c_str.emplace_back(Extension.c_str());
    });
    TArray<char const*> RequiredExtensions_c_str; RequiredExtensions_c_str.reserve(RequiredExtensions.size());
    algo::for_each(RequiredExtensions, [&RequiredExtensions_c_str](LString const& Extension)
    {
        RequiredExtensions_c_str.emplace_back(Extension.c_str());
    });

    VkApplicationInfo App{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = this->GetHumanReadableName().c_str(),
        .pEngineName = "Jafg",
        .apiVersion = VK_API_VERSION_1_4
        };

    VkInstanceCreateInfo Instance{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &App,
        .enabledLayerCount = static_cast<u32>(RequiredInstanceLayers_c_str.size()),
        .ppEnabledLayerNames = reinterpret_cast<char const* const*>(RequiredInstanceLayers_c_str.data()),
        .enabledExtensionCount = static_cast<u32>(RequiredExtensions_c_str.size()),
        .ppEnabledExtensionNames = reinterpret_cast<char const* const*>(RequiredExtensions_c_str.data())
        };

#if !IN_SHIPPING
    VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessenger{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity =
              VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType =
              VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT

            ,
        .pfnUserCallback = Hermes
        };
    if (this->bVkMyInstanceLayerAddressBindings)
    {
        DebugUtilsMessenger.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
    }

    check( Instance.pNext == nullptr )
    Instance.pNext = &DebugUtilsMessenger;
#endif /* !IN_SHIPPING */

    if (vkCreateInstance(&Instance, nullptr, &this->VkMyInstance) != VK_SUCCESS)
    {
        panic( "Failed to create Vulkan instance." )
    }

    volkLoadInstance(this->VkMyInstance);

#if !IN_SHIPPING
    if (vkCreateDebugUtilsMessengerEXT(this->VkMyInstance, &DebugUtilsMessenger, nullptr, &this->VkMyHermes) != VK_SUCCESS)
    {
        panic( "Failed to create Vulkan debug messenger." )
    }
#endif /* !IN_SHIPPING */

    if (glfwCreateWindowSurface(this->VkMyInstance, this->Handle, nullptr, &this->VkMySurface) != VK_SUCCESS)
    {
        panic( "Failed to create Vulkan window surface." )
    }
    check( this->VkMySurface )

    LOG_VERBOSE(LogVulkan, "Loading vulkan physical device.")
    u32 PhysicalDeviceCount{ 0 };
    if (vkEnumeratePhysicalDevices(this->VkMyInstance, &PhysicalDeviceCount, nullptr) != VK_SUCCESS)
    {
        panic( "Failed to enumerate physical devices." )
    }
    if (PhysicalDeviceCount < 1)
    {
        panicMsgf( "Failed to find any physical devices with Vulkan support." )
    }

    TArray<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
    if (vkEnumeratePhysicalDevices(this->VkMyInstance, &PhysicalDeviceCount, PhysicalDevices.data()) != VK_SUCCESS)
    {
        panic( "Failed to enumerate physical devices." )
    }

    std::multimap<u64, VkPhysicalDevice> RankedPhysicalDevices;
    for (auto const& PhysicalDevice : PhysicalDevices)
    {
        auto GetPhysicalDeviceRating = [](VkPhysicalDevice _PhysicalDevice) -> u64
        {
            u64 Rating{ 0 };

            VkPhysicalDeviceProperties Properties;
            vkGetPhysicalDeviceProperties(_PhysicalDevice, &Properties);

            if (Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                Rating += 16'384;
            }
            else if (Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            {
                Rating += 8'192;
            }

            Rating += Properties.limits.maxImageDimension2D;

            return Rating;
        };

        RankedPhysicalDevices.insert(std::make_pair(GetPhysicalDeviceRating(PhysicalDevice), PhysicalDevice));
        continue;
    }

    LOG_VERBOSE(LogVulkan, "Available physical devices ranked by suitability:")
    for (auto const& [Rating, PhysicalDevice] : RankedPhysicalDevices)
    {
        VkPhysicalDeviceProperties Properties;
        vkGetPhysicalDeviceProperties(PhysicalDevice, &Properties);
        LOG_VERBOSE(LogVulkan, "    [{}] rated [{}]: {} (API v{}.{}.{}), Driver v{}.{}.{}",
            reinterpret_cast<void const*>(PhysicalDevice),
            Rating,
            Properties.deviceName,
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

    LOG_VERBOSE(LogVulkan, "Loading vulkan queue families.")
    for (auto const& PhysicalDevice: RankedPhysicalDevices | std::views::values)
    {
        check( this->VkMyPhysicalDevice == nullptr )
        check( this->VkMyGraphicsQueueFamilyIndex.has_value() == false )
        check( this->VkMyPresentQueueFamilyIndex.has_value() == false )

        u32 QueueFamilies{ 0 };
        vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilies, nullptr);
        if (QueueFamilies < 1)
        {
            LOG_VERBOSE(LogVulkan, "Physical device [{}] has no queue families. Skipping.", reinterpret_cast<void const*>(PhysicalDevice))
            continue;
        }

        std::vector<VkQueueFamilyProperties> QueueFamilyProperties(QueueFamilies);
        vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilies, QueueFamilyProperties.data());
        for (u32 Idx{ 0 }; Idx < QueueFamilies; ++Idx)
        {
            if (this->VkMyGraphicsQueueFamilyIndex.has_value() == false && QueueFamilyProperties[Idx].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                this->VkMyGraphicsQueueFamilyIndex = Idx;
            }

            if (this->VkMyPresentQueueFamilyIndex.has_value() == false)
            {
                VkBool32 bCanPresent{ VK_FALSE };
                vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, Idx, this->VkMySurface, &bCanPresent);
                if (bCanPresent == VK_TRUE)
                {
                   this->VkMyPresentQueueFamilyIndex = Idx;
                }
            }

            if (this->VkMyGraphicsQueueFamilyIndex.has_value() && this->VkMyPresentQueueFamilyIndex.has_value())
            {
                break;
            }

            continue;
        }

        if (this->VkMyGraphicsQueueFamilyIndex.has_value() == false)
        {
            LOG_VERBOSE(LogVulkan, "Physical device [{}] has no suitable graphics queue family. Skipping.", reinterpret_cast<void const*>(PhysicalDevice))
            this->VkMyPresentQueueFamilyIndex.reset();
            continue;
        }

        if (this->VkMyPresentQueueFamilyIndex.has_value() == false)
        {
            LOG_VERBOSE(LogVulkan, "Physical device [{}] has no suitable graphics queue family. Skipping.", reinterpret_cast<void const*>(PhysicalDevice))
            this->VkMyGraphicsQueueFamilyIndex.reset();
            continue;
        }

        this->VkMyPhysicalDevice = PhysicalDevice;
        break;
    }
    if (this->VkMyPhysicalDevice == nullptr)
    {
        panicMsgf( "Failed to find a suitable physical device." )
    }
    if (this->VkMyGraphicsQueueFamilyIndex.value())
    {
        panicMsgf( "Failed to find a suitable graphics queue family." )
    }
    LOG_VERBOSE(LogVulkan, "Selected physical device [{}] with graphics queue family index [{}] and present queue family index [{}].",
        reinterpret_cast<void const*>(this->VkMyPhysicalDevice),
        this->VkMyGraphicsQueueFamilyIndex.value(),
        this->VkMyPresentQueueFamilyIndex.value()
        )

    LOG_VERBOSE(LogVulkan, "Checking required device extensions.")
    u32 AvailableDeviceExtensionCount{ 0 };
    if (vkEnumerateDeviceExtensionProperties(this->VkMyPhysicalDevice, nullptr, &AvailableDeviceExtensionCount, nullptr) != VK_SUCCESS)
    {
        panic( "Failed to enumerate device extensions." )
    }
    TArray<VkExtensionProperties> AvailableDeviceExtensions(AvailableDeviceExtensionCount);
    if (vkEnumerateDeviceExtensionProperties(this->VkMyPhysicalDevice, nullptr, &AvailableDeviceExtensionCount, AvailableDeviceExtensions.data()) != VK_SUCCESS)
    {
        panic( "Failed to enumerate device extensions." )
    }
    LOG_VERBOSE(LogVulkan, "Device Extension Count [{}]. Available extensions:", AvailableDeviceExtensionCount)
    for (VkExtensionProperties const& Extension : AvailableDeviceExtensions)
    {
        LOG_VERBOSE(LogVulkan, "    {} spec[{}]", Extension.extensionName, Extension.specVersion)
    }
    TArray<char const*> RequiredDeviceExtensions{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
    algo::for_each(RequiredDeviceExtensions, [&AvailableDeviceExtensions](auto const* RequiredDeviceExtension)
    {
        if (algo::contains(AvailableDeviceExtensions, LString{RequiredDeviceExtension}, &VkExtensionProperties::extensionName) == false)
        {
            panicMsgf("Required Vulkan device extension [{}] is not available.", RequiredDeviceExtension)
        }

        return;
    });

    LOG_VERBOSE(LogVulkan, "Loading vulkan logical device.")
    TSet<u32> UniqueQueueFamilyIndices{
        this->VkMyGraphicsQueueFamilyIndex.value(),
        this->VkMyPresentQueueFamilyIndex.value()
        };
    TArray<VkDeviceQueueCreateInfo> QueueCreateInfos; QueueCreateInfos.reserve(UniqueQueueFamilyIndices.size());
    f32 QueuePriority{ 1.0f };
    for (u32 QueueFamilyIndex : UniqueQueueFamilyIndices)
    {
        VkDeviceQueueCreateInfo QueueCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = QueueFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = &QueuePriority
            };
        QueueCreateInfos.emplace_back(std::move(QueueCreateInfo));
        continue;
    }

    VkPhysicalDeviceFeatures DeviceFeatures{};
    VkDeviceCreateInfo CreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<u32>(QueueCreateInfos.size()),
        .pQueueCreateInfos = QueueCreateInfos.data(),
// #if !IN_SHIPPING
//         .enabledLayerCount = static_cast<u32>(RequiredInstanceLayers_c_str.size()),
//         .ppEnabledLayerNames = reinterpret_cast<char const* const*>(RequiredInstanceLayers_c_str.data()),
// #else /* !IN_SHIPPING */
//         .enabledLayerCount = 0,
//         .ppEnabledLayerNames = nullptr,
// #endif /* IN_SHIPPING */
        .enabledExtensionCount = static_cast<u32>(RequiredDeviceExtensions.size()),
        .ppEnabledExtensionNames = RequiredDeviceExtensions.data(),
        };
    check( CreateInfo.enabledLayerCount == 0 && CreateInfo.ppEnabledLayerNames == nullptr && "Deprecated")

    if (vkCreateDevice(this->VkMyPhysicalDevice, &CreateInfo, nullptr, &this->VkMyDevice) != VK_SUCCESS)
    {
        panic( "Failed to create Vulkan logical device." )
    }
    volkLoadDevice(this->VkMyDevice);

    vkGetDeviceQueue(this->VkMyDevice, this->VkMyGraphicsQueueFamilyIndex.value(), 0, &this->VkMyGraphicsQueue);
    vkGetDeviceQueue(this->VkMyDevice, this->VkMyPresentQueueFamilyIndex.value(), 0, &this->VkMyPresentQueue);

    LOG_VERBOSE(LogVulkan, "Creating VMA allocator.")
    VmaVulkanFunctions VmaVulkanFunc{
        .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr = vkGetDeviceProcAddr
        };

    VmaAllocatorCreateInfo VmaAllocatorCreateInfo{
        .physicalDevice = this->VkMyPhysicalDevice,
        .device = this->VkMyDevice,
        .pVulkanFunctions = &VmaVulkanFunc,
        .instance = this->VkMyInstance
        };

    if (vmaCreateAllocator(&VmaAllocatorCreateInfo, &this->VmaMyAllocator) != VK_SUCCESS)
    {
        panic( "Failed to create VMA allocator." )
    }

    return;
}

void Jafg::LSurfaceGlfw3::OnClear()
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    // glfwMakeContextCurrent(this->Handle);

    Super::OnClear();

    return;
}

void Jafg::LSurfaceGlfw3::OnUpdate()
{
    STAT_CYCLE_FUNCTION()

    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    // glfwMakeContextCurrent(this->Handle);

    Super::OnUpdate();

    {
        STAT_QUICK_CYCLE_START("SwapBuffers")
        // glfwSwapBuffers(this->Handle);
    }

    return;
}

void Jafg::LSurfaceGlfw3::TearDown()
{
    Super::TearDown();

    if (this->Cursor)
    {
        glfwDestroyCursor(this->Cursor);
        this->Cursor = nullptr;
    }

    if (this->Handle)
    {
        LOG_INFO(LogSurface, "Destroying glfw window.")
        glfwDestroyWindow(this->Handle);
        this->Handle = nullptr;
    }

    if (IsEngineExitRequested() && GEngine->GetLocalEgo().GetFrontend().GetSurfaceCount() == 0)
    {
        LOG_INFO(LogSurface, "Terminating glfw.")
        glfwTerminate();
    }

    return;
}

void Jafg::LSurfaceGlfw3::BeginNewFrame()
{
    Super::BeginNewFrame();

    if (this->bPendingResize)
    {
        this->PendingTimeForResizeApply -= Application::GetDeltaTimeAsFloat();

        if (this->PendingTimeForResizeApply <= 0.0f)
        {
            LOG_VERBOSE(LogSurface,
                "Applying pending resize to surface [{}x{}].",
                this->PendingWidth,
                this->PendingHeight
                )

            this->FramebufferSizeCallbackImpl(this->PendingWidth, this->PendingHeight);

            this->bPendingResize = false;
            this->PendingTimeForResizeApply = 0.0f;
            this->PendingWidth = 0;
            this->PendingHeight = 0;
        }
    }

    return;
}

void Jafg::LSurfaceGlfw3::PollInputs()
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    // glfwMakeContextCurrent(this->Handle);

    LKey KeyCursor = EKeys::A;
    while (KeyCursor <= EKeys::LastKey)
    {
        const i32 TranslatedKey = Glfw3::TranslateKeyToGlfw(KeyCursor);
        if (TranslatedKey == INDEX_NONE)
        {
            ++KeyCursor;
            continue;
        }

        if (false)//glfwGetKey(this->Handle, TranslatedKey) == GLFW_PRESS)
        {
            this->AddKeyDown(KeyCursor);

#if PLATFORM_LINUX
            if (this->IsPlatformSupportsRepeatedKey() == false && this->IsNewKeyDown(KeyCursor))
            {
                Application::LHrcTimePoint Now { Application::GetHighestNow() };
                this->SetLastPressTimePoint(Now);
                this->SetCurrentRepeatedKeyInQuestion(KeyCursor);
                this->Glfw3LastNewKey = TranslatedKey;
                // this->EmulateContentForBufferedInputGlfw3(TranslatedKey);
            }
#endif /* PLATFORM_LINUX */
        }

        ++KeyCursor;

        continue;
    }

    // if (glfwGetMouseButton(this->Handle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    // {
    //     this->AddKeyDown(EKeys::LeftMouseButton);
    // }
    // if (glfwGetMouseButton(this->Handle, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    // {
    //     this->AddKeyDown(EKeys::RightMouseButton);
    // }
    // if (glfwGetMouseButton(this->Handle, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    // {
    //     this->AddKeyDown(EKeys::MiddleMouseButton);
    // }

    return;
}

void Jafg::LSurfaceGlfw3::PollEvents()
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    // glfwMakeContextCurrent(this->Handle);

    if (glfwWindowShouldClose(this->Handle))
    {
        GEngine->RequestEngineExit("Window closed by user.");
    }

    glfwPollEvents();

    return;
}

void Jafg::LSurfaceGlfw3::SetInputMode(const EInputMode::Type InMode, const bool bInShowCursor)
{
    Super::SetInputMode(InMode, bInShowCursor);
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )

    if (this->bShowCursor)
    {
        this->bFirstMouseCallback = true;
    }

    // glfwMakeContextCurrent(this->Handle);
    // glfwSetInputMode(this->Handle, GLFW_CURSOR, this->bShowCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

    return;
}

void Jafg::LSurfaceGlfw3::SetMouseCursor(const EMouseCursor::Type InCursor)
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    // glfwMakeContextCurrent(this->Handle);

    if (this->Cursor)
    {
        //glfwDestroyCursor(this->Cursor);
        this->Cursor = nullptr;
    }

    if (InCursor == EMouseCursor::Default)
    {
        check( this->Cursor == nullptr )
    }
    else if (InCursor == EMouseCursor::Arrow)
    {
        // this->Cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    }
    else if (InCursor == EMouseCursor::Beam)
    {
        // this->Cursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    }
    else if (InCursor == EMouseCursor::Crosshair)
    {
        // this->Cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    }
    else if (InCursor == EMouseCursor::Hand)
    {
        // this->Cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    }
    else if (InCursor == EMouseCursor::ResizeNS)
    {
        // this->Cursor = glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);
    }
    else if (InCursor == EMouseCursor::ResizeEW)
    {
        // this->Cursor = glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);
    }
    else if (InCursor == EMouseCursor::ResizeNESW)
    {
        // this->Cursor = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
    }
    else if (InCursor == EMouseCursor::ResizeNWSE)
    {
        // this->Cursor = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    }
    else if (InCursor == EMouseCursor::ResizeOmni)
    {
        // this->Cursor = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
    }
    else if (InCursor == EMouseCursor::NotAllowed)
    {
        // this->Cursor = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
    }
    else
    {
        LOG_WARNING(LogSystem, "Unknown cursor type {}[{}].", static_cast<i32>(InCursor), LexToString(InCursor))
    }

    if (InCursor != EMouseCursor::None)
    {
        // glfwSetCursor(this->Handle, this->Cursor);
    }

    return;
}

LIntVector2 Jafg::LSurfaceGlfw3::GetDimensions() const
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    // glfwMakeContextCurrent(this->Handle);

    /*
     * Do we want to cache this value?
     * How long does it take to get the window size?
     */
    i32 Width, Height;
    glfwGetWindowSize(this->Handle, &Width, &Height);
    return LIntVector2{Width, Height};
}

bool Jafg::LSurfaceGlfw3::CanVSync() const
{
    return true;
}

void Jafg::LSurfaceGlfw3::SetVSync(const bool bEnabled)
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )

    if (this->bVSync == bEnabled)
    {
        return;
    }

    this->bVSync = bEnabled;

    // glfwMakeContextCurrent(this->Handle);
    // glfwSwapInterval(this->bVSync ? 1 : 0);

    return;
}

void Jafg::LSurfaceGlfw3::FramebufferSizeCallback(const i32 Width, const i32 Height)
{
    this->PendingWidth = Width;
    this->PendingHeight = Height;
    this->bPendingResize = true;

    /*
     * One-point-five-second delay when making changes to the viewport. We might want to change this later depending
     * on some platform parameters.
     */
    // this->PendingTimeForResizeApply = 1.5f;
    this->PendingTimeForResizeApply = 0.2f;

    return;
}

void Jafg::LSurfaceGlfw3::FramebufferSizeCallbackImpl(const i32 Width, const i32 Height)
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    // glfwMakeContextCurrent(this->Handle);

    // glViewport(0, 0, Width, Height);
    this->GetViewport().ChangeDimensions({Width, Height});

    return;
}

void Jafg::LSurfaceGlfw3::MouseCallback(const double XPos, const double YPos)
{
    this->MouseLocation = LVector2(static_cast<float>(XPos), static_cast<float>(YPos));

    if (this->IsShowMouseCursor())
    {
        return;
    }

    if (algo::contains(this->GetCurrentlyPressedKeys(), EKeys::MouseX, &LRawInput::Key))
    {
        return;
    }
    if (algo::contains(this->GetCurrentlyPressedKeys(), EKeys::MouseY, &LRawInput::Key))
    {
        return;
    }

    if (this->bFirstMouseCallback)
    {
        this->LastMouseX = XPos;
        this->LastMouseY = YPos;
        this->bFirstMouseCallback = false;
    }

    const double XOffset = static_cast<double>(XPos) - this->LastMouseX;
    const double YOffset = this->LastMouseY - static_cast<double>(YPos);
    this->LastMouseX = XPos;
    this->LastMouseY = YPos;

    this->AddKeyDown(EKeys::MouseX, static_cast<float>(YOffset));
    this->AddKeyDown(EKeys::MouseY, static_cast<float>(XOffset));

    return;
}

void Jafg::LSurfaceGlfw3::ScrollCallback(const double XOffset, const double YOffset)
{
    if (algo::contains(this->GetCurrentlyPressedKeys(), EKeys::MouseWheelAxis, &LRawInput::Key))
    {
        return;
    }

    if (YOffset > 0.0f)
    {
        this->AddKeyDown(EKeys::MouseWheelUp, static_cast<float>(YOffset));
    }
    else if (YOffset < 0.0f)
    {
        this->AddKeyDown(EKeys::MouseWheelDown, static_cast<float>(YOffset));
    }

    this->AddKeyDown(EKeys::MouseWheelAxis, static_cast<float>(YOffset));

    return;
}

void Jafg::LSurfaceGlfw3::MouseEnterCallback(const i32 Entered)
{
    if (Entered == GLFW_TRUE)
    {
        this->bMouseLocationIsMeaningful = true;
    }
    else
    {
        this->bMouseLocationIsMeaningful = false;
    }

    return;
}

void Jafg::LSurfaceGlfw3::CharCallback(const u32 Codepoint)
{
    std::u32string Char;
    Char.push_back(Codepoint);

#if LAL_WITH_MSVC
    #pragma warning( push )
    #pragma warning(disable: 4996)
#endif /* LAL_WITH_MSVC */
#if LAL_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif /* LAL_WITH_CLANG */
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::string utf8String = converter.to_bytes(Char);
#if LAL_WITH_MSVC
    #pragma warning( pop )
#endif /* LAL_WITH_MSVC */
#if LAL_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* LAL_WITH_CLANG */

    this->AddBufferedPlatformInput(utf8String.c_str());

    return;
}

void Jafg::LSurfaceGlfw3::KeyCallback(const i32 Key, const i32 Scancode, const i32 Action, const i32 Mods)
{
    if (Action == GLFW_REPEAT)
    {
        const LKey JafgKey { Glfw3::TranslateKeyFromGlfw(Key) };

        if (JafgKey != EKeys::Unresolved)
        {
            LRawInput* RealKey = algo::find_pointer(this->GetCurrentlyPressedKeys(), JafgKey, &LRawInput::Key);
            if (RealKey == nullptr)
            {
                this->AddKeyDown(JafgKey);
                RealKey = algo::find_pointer(this->GetCurrentlyPressedKeys(), JafgKey, &LRawInput::Key);
            }

            check( RealKey )
            RealKey->bRepeated = true;
        }
    }

    return;
}


#if PLATFORM_LINUX
void Jafg::LSurfaceGlfw3::EmulateRepeatedContentForBufferedInput()
{
    this->EmulateContentForBufferedInputGlfw3(this->Glfw3LastNewKey);
    return;
}

void Jafg::LSurfaceGlfw3::EmulateContentForBufferedInput(const LKey InKey)
{
    this->EmulateContentForBufferedInputGlfw3(Glfw3::TranslateKeyToGlfw(InKey));
    return;
}

void Jafg::LSurfaceGlfw3::EmulateContentForBufferedInputGlfw3(const i32 InKey)
{
    if (InKey == GLFW_DONT_CARE)
    {
        return;
    }

    // Somehow native access wayland? Glfw3 does not have an Api for this.

    return;
}
#endif /* PLATFORM_LINUX */

#endif /* JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */
