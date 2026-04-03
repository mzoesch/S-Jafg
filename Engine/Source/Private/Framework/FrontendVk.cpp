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

#include "Framework/MeshSubsystem.h"
#include "Framework/TextureSubsystem.h"
#include "Platform/PlatformMisc.h"
#include "Stats/Stats.h"
#include "Engine/Engine.h"
#include "User/UserPreferences.h"
#include "Engine/WorldData.h"
#include "Rhi/VisualInstance.h"
#include "Rhi/BindlessTextureArray.h"

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
    if (Error == GLFW_PLATFORM_ERROR)
    {
        if (Description == LStringView{"Wayland: Failed to encode keysym as UTF-8"}) /* Not an error... */
        {
            return;
        }
    }

    LOG_FATAL(LogPlatform, "GLFW Error (code {}): {}", Error, Description)
}

struct LNamedPhysicalKeyToPhysicalKeyResult final
{
    i32 Scancodable{ INDEX_NONE };
    i32 Logical{ INDEX_NONE };
};

LNamedPhysicalKeyToPhysicalKeyResult NamedPhysicalKeyToPhysicalKey(Jafg::ENamedPhysicalKey Key) noexcept
{
    switch (Key)
    {
    case Jafg::ENamedPhysicalKey::Unresolved:       return {};
    case Jafg::ENamedPhysicalKey::LeftShift:        return {.Scancodable = GLFW_KEY_LEFT_SHIFT };
    case Jafg::ENamedPhysicalKey::RightShift:       return {.Scancodable = GLFW_KEY_RIGHT_SHIFT };
    case Jafg::ENamedPhysicalKey::LeftControl:      return {.Scancodable = GLFW_KEY_LEFT_CONTROL };
    case Jafg::ENamedPhysicalKey::RightControl:     return {.Scancodable = GLFW_KEY_RIGHT_CONTROL };
    case Jafg::ENamedPhysicalKey::LeftAlt:          return {.Scancodable = GLFW_KEY_LEFT_ALT };
    case Jafg::ENamedPhysicalKey::RightAlt:         return {.Scancodable = GLFW_KEY_RIGHT_ALT };
    case Jafg::ENamedPhysicalKey::LeftCommand:      return {.Scancodable = GLFW_KEY_LEFT_SUPER };
    case Jafg::ENamedPhysicalKey::RightCommand:     return {.Scancodable = GLFW_KEY_RIGHT_SUPER };
    case Jafg::ENamedPhysicalKey::PrintScreen:      return {.Scancodable = GLFW_KEY_PRINT_SCREEN };
    case Jafg::ENamedPhysicalKey::ScrollLock:       return {.Scancodable = GLFW_KEY_SCROLL_LOCK };
    case Jafg::ENamedPhysicalKey::Pause:            return {.Scancodable = GLFW_KEY_PAUSE };
    case Jafg::ENamedPhysicalKey::Insert:           return {.Scancodable = GLFW_KEY_INSERT };
    case Jafg::ENamedPhysicalKey::Home:             return {.Scancodable = GLFW_KEY_HOME };
    case Jafg::ENamedPhysicalKey::PageUp:           return {.Scancodable = GLFW_KEY_PAGE_UP };
    case Jafg::ENamedPhysicalKey::Delete:           return {.Scancodable = GLFW_KEY_DELETE };
    case Jafg::ENamedPhysicalKey::End:              return {.Scancodable = GLFW_KEY_END };
    case Jafg::ENamedPhysicalKey::PageDown:         return {.Scancodable = GLFW_KEY_PAGE_DOWN };
    case Jafg::ENamedPhysicalKey::Escape:           return {.Scancodable = GLFW_KEY_ESCAPE };
    case Jafg::ENamedPhysicalKey::Tilde:            return {.Scancodable = GLFW_KEY_GRAVE_ACCENT };
    case Jafg::ENamedPhysicalKey::Hyphen:           return {.Scancodable = GLFW_KEY_MINUS };
    case Jafg::ENamedPhysicalKey::Equals:           return {.Scancodable = GLFW_KEY_EQUAL };
    case Jafg::ENamedPhysicalKey::BackSpace:        return {.Scancodable = GLFW_KEY_BACKSPACE };
    case Jafg::ENamedPhysicalKey::Tab:              return {.Scancodable = GLFW_KEY_TAB };
    case Jafg::ENamedPhysicalKey::LeftBracket:      return {.Scancodable = GLFW_KEY_LEFT_BRACKET };
    case Jafg::ENamedPhysicalKey::RightBracket:     return {.Scancodable = GLFW_KEY_RIGHT_BRACKET };
    case Jafg::ENamedPhysicalKey::Enter:            return {.Scancodable = GLFW_KEY_ENTER };
    case Jafg::ENamedPhysicalKey::CapsLock:         return {.Scancodable = GLFW_KEY_CAPS_LOCK };
    case Jafg::ENamedPhysicalKey::Semicolon:        return {.Scancodable = GLFW_KEY_SEMICOLON };
    case Jafg::ENamedPhysicalKey::Apostrophe:       return {.Scancodable = GLFW_KEY_APOSTROPHE };
    case Jafg::ENamedPhysicalKey::Hashtag:          return {.Scancodable = GLFW_KEY_WORLD_1 };
    case Jafg::ENamedPhysicalKey::Backslash:        return {.Scancodable = GLFW_KEY_BACKSLASH };
    case Jafg::ENamedPhysicalKey::Comma:            return {.Scancodable = GLFW_KEY_COMMA };
    case Jafg::ENamedPhysicalKey::Period:           return {.Scancodable = GLFW_KEY_PERIOD };
    case Jafg::ENamedPhysicalKey::Slash:            return {.Scancodable = GLFW_KEY_SLASH };
    case Jafg::ENamedPhysicalKey::Space:            return {.Scancodable = GLFW_KEY_SPACE };
    case Jafg::ENamedPhysicalKey::Menu:             return {.Scancodable = GLFW_KEY_MENU };
    case Jafg::ENamedPhysicalKey::Left:             return {.Scancodable = GLFW_KEY_LEFT };
    case Jafg::ENamedPhysicalKey::Up:               return {.Scancodable = GLFW_KEY_UP };
    case Jafg::ENamedPhysicalKey::Right:            return {.Scancodable = GLFW_KEY_RIGHT };
    case Jafg::ENamedPhysicalKey::Down:             return {.Scancodable = GLFW_KEY_DOWN };
    case Jafg::ENamedPhysicalKey::A:                return {.Scancodable = GLFW_KEY_A };
    case Jafg::ENamedPhysicalKey::B:                return {.Scancodable = GLFW_KEY_B };
    case Jafg::ENamedPhysicalKey::C:                return {.Scancodable = GLFW_KEY_C };
    case Jafg::ENamedPhysicalKey::D:                return {.Scancodable = GLFW_KEY_D };
    case Jafg::ENamedPhysicalKey::E:                return {.Scancodable = GLFW_KEY_E };
    case Jafg::ENamedPhysicalKey::F:                return {.Scancodable = GLFW_KEY_F };
    case Jafg::ENamedPhysicalKey::G:                return {.Scancodable = GLFW_KEY_G };
    case Jafg::ENamedPhysicalKey::H:                return {.Scancodable = GLFW_KEY_H };
    case Jafg::ENamedPhysicalKey::I:                return {.Scancodable = GLFW_KEY_I };
    case Jafg::ENamedPhysicalKey::J:                return {.Scancodable = GLFW_KEY_J };
    case Jafg::ENamedPhysicalKey::K:                return {.Scancodable = GLFW_KEY_K };
    case Jafg::ENamedPhysicalKey::L:                return {.Scancodable = GLFW_KEY_L };
    case Jafg::ENamedPhysicalKey::M:                return {.Scancodable = GLFW_KEY_M };
    case Jafg::ENamedPhysicalKey::N:                return {.Scancodable = GLFW_KEY_N };
    case Jafg::ENamedPhysicalKey::O:                return {.Scancodable = GLFW_KEY_O };
    case Jafg::ENamedPhysicalKey::P:                return {.Scancodable = GLFW_KEY_P };
    case Jafg::ENamedPhysicalKey::Q:                return {.Scancodable = GLFW_KEY_Q };
    case Jafg::ENamedPhysicalKey::R:                return {.Scancodable = GLFW_KEY_R };
    case Jafg::ENamedPhysicalKey::S:                return {.Scancodable = GLFW_KEY_S };
    case Jafg::ENamedPhysicalKey::T:                return {.Scancodable = GLFW_KEY_T };
    case Jafg::ENamedPhysicalKey::U:                return {.Scancodable = GLFW_KEY_U };
    case Jafg::ENamedPhysicalKey::V:                return {.Scancodable = GLFW_KEY_V };
    case Jafg::ENamedPhysicalKey::W:                return {.Scancodable = GLFW_KEY_W };
    case Jafg::ENamedPhysicalKey::X:                return {.Scancodable = GLFW_KEY_X };
    case Jafg::ENamedPhysicalKey::Y:                return {.Scancodable = GLFW_KEY_Y };
    case Jafg::ENamedPhysicalKey::Z:                return {.Scancodable = GLFW_KEY_Z };
    case Jafg::ENamedPhysicalKey::Zero:             return {.Scancodable = GLFW_KEY_0 };
    case Jafg::ENamedPhysicalKey::One:              return {.Scancodable = GLFW_KEY_1 };
    case Jafg::ENamedPhysicalKey::Two:              return {.Scancodable = GLFW_KEY_2 };
    case Jafg::ENamedPhysicalKey::Three:            return {.Scancodable = GLFW_KEY_3 };
    case Jafg::ENamedPhysicalKey::Four:             return {.Scancodable = GLFW_KEY_4 };
    case Jafg::ENamedPhysicalKey::Five:             return {.Scancodable = GLFW_KEY_5 };
    case Jafg::ENamedPhysicalKey::Six:              return {.Scancodable = GLFW_KEY_6 };
    case Jafg::ENamedPhysicalKey::Seven:            return {.Scancodable = GLFW_KEY_7 };
    case Jafg::ENamedPhysicalKey::Eight:            return {.Scancodable = GLFW_KEY_8 };
    case Jafg::ENamedPhysicalKey::Nine:             return {.Scancodable = GLFW_KEY_9 };
    case Jafg::ENamedPhysicalKey::F1:               return {.Scancodable = GLFW_KEY_F1 };
    case Jafg::ENamedPhysicalKey::F2:               return {.Scancodable = GLFW_KEY_F2 };
    case Jafg::ENamedPhysicalKey::F3:               return {.Scancodable = GLFW_KEY_F3 };
    case Jafg::ENamedPhysicalKey::F4:               return {.Scancodable = GLFW_KEY_F4 };
    case Jafg::ENamedPhysicalKey::F5:               return {.Scancodable = GLFW_KEY_F5 };
    case Jafg::ENamedPhysicalKey::F6:               return {.Scancodable = GLFW_KEY_F6 };
    case Jafg::ENamedPhysicalKey::F7:               return {.Scancodable = GLFW_KEY_F7 };
    case Jafg::ENamedPhysicalKey::F8:               return {.Scancodable = GLFW_KEY_F8 };
    case Jafg::ENamedPhysicalKey::F9:               return {.Scancodable = GLFW_KEY_F9 };
    case Jafg::ENamedPhysicalKey::F10:              return {.Scancodable = GLFW_KEY_F10 };
    case Jafg::ENamedPhysicalKey::F11:              return {.Scancodable = GLFW_KEY_F11 };
    case Jafg::ENamedPhysicalKey::F12:              return {.Scancodable = GLFW_KEY_F12 };
    case Jafg::ENamedPhysicalKey::F13:              return {.Scancodable = GLFW_KEY_F13 };
    case Jafg::ENamedPhysicalKey::F14:              return {.Scancodable = GLFW_KEY_F14 };
    case Jafg::ENamedPhysicalKey::F15:              return {.Scancodable = GLFW_KEY_F15 };
    case Jafg::ENamedPhysicalKey::F16:              return {.Scancodable = GLFW_KEY_F16 };
    case Jafg::ENamedPhysicalKey::F17:              return {.Scancodable = GLFW_KEY_F17 };
    case Jafg::ENamedPhysicalKey::F18:              return {.Scancodable = GLFW_KEY_F18 };
    case Jafg::ENamedPhysicalKey::F19:              return {.Scancodable = GLFW_KEY_F19 };
    case Jafg::ENamedPhysicalKey::F20:              return {.Scancodable = GLFW_KEY_F20 };
    case Jafg::ENamedPhysicalKey::F21:              return {.Scancodable = GLFW_KEY_F21 };
    case Jafg::ENamedPhysicalKey::F22:              return {.Scancodable = GLFW_KEY_F22 };
    case Jafg::ENamedPhysicalKey::F23:              return {.Scancodable = GLFW_KEY_F23 };
    case Jafg::ENamedPhysicalKey::F24:              return {.Scancodable = GLFW_KEY_F24 };
    case Jafg::ENamedPhysicalKey::F25:              return {.Scancodable = GLFW_KEY_F25 };
    case Jafg::ENamedPhysicalKey::NumPadZero:       return {.Scancodable = GLFW_KEY_KP_0 };
    case Jafg::ENamedPhysicalKey::NumPadOne:        return {.Scancodable = GLFW_KEY_KP_1 };
    case Jafg::ENamedPhysicalKey::NumPadTwo:        return {.Scancodable = GLFW_KEY_KP_2 };
    case Jafg::ENamedPhysicalKey::NumPadThree:      return {.Scancodable = GLFW_KEY_KP_3 };
    case Jafg::ENamedPhysicalKey::NumPadFour:       return {.Scancodable = GLFW_KEY_KP_4 };
    case Jafg::ENamedPhysicalKey::NumPadFive:       return {.Scancodable = GLFW_KEY_KP_5 };
    case Jafg::ENamedPhysicalKey::NumPadSix:        return {.Scancodable = GLFW_KEY_KP_6 };
    case Jafg::ENamedPhysicalKey::NumPadSeven:      return {.Scancodable = GLFW_KEY_KP_7 };
    case Jafg::ENamedPhysicalKey::NumPadEight:      return {.Scancodable = GLFW_KEY_KP_8 };
    case Jafg::ENamedPhysicalKey::NumPadNine:       return {.Scancodable = GLFW_KEY_KP_9 };
    case Jafg::ENamedPhysicalKey::NumPadLock:       return {.Scancodable = GLFW_KEY_NUM_LOCK };
    case Jafg::ENamedPhysicalKey::NumPadDivide:     return {.Scancodable = GLFW_KEY_KP_DIVIDE };
    case Jafg::ENamedPhysicalKey::NumPadMultiply:   return {.Scancodable = GLFW_KEY_KP_MULTIPLY };
    case Jafg::ENamedPhysicalKey::NumPadSubtract:   return {.Scancodable = GLFW_KEY_KP_SUBTRACT };
    case Jafg::ENamedPhysicalKey::NumPadAdd:        return {.Scancodable = GLFW_KEY_KP_ADD };
    case Jafg::ENamedPhysicalKey::NumPadEnter:      return {.Scancodable = GLFW_KEY_KP_ENTER };
    case Jafg::ENamedPhysicalKey::NumPadDecimal:    return {.Scancodable = GLFW_KEY_KP_DECIMAL };
    default:
    {
        check(Key >= Jafg::ENamedPhysicalKey::FirstLogicalKey)
        check(Key >= Jafg::ENamedPhysicalKey::LastKey)
        return {
            .Logical = static_cast<std::underlying_type_t<Jafg::ENamedPhysicalKey>>(Key)
            };
    }
    }
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

    LOG_VERBOSE(LogSurface, "Initializing glfw.")

    check(Tasks::IsOnMasterThread())

    // TODO: Do we want to use this sometimes/always? Or make a user flag for this??
    // glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    // TODO: This does not work for wayland - but with x11.
    //       So we want that?
    // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    //# TODO: Move tis somewhere else? This has nothing to do with VK.
    if (!glfwInit())
    {
        panic("Failed to initialize glfw.")
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ::GlfwContextVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ::GlfwContextVersionMinor);

    glfwSetErrorCallback(::GlfwErrorCallback);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    const i32 Platform{glfwGetPlatform()};
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

    i32 MonitorCount{};
    auto Monitors{glfwGetMonitors(&MonitorCount)};
    if (MonitorCount < 1)
    {
        panic( "No suitable physical monitors detected." )
    }
    auto* PrimaryMonitor{glfwGetPrimaryMonitor()};
    if (PrimaryMonitor == nullptr)
    {
        LOG_VERBOSE(LogSurface, "No primary monitor detected, picking first available monitor as primary.")
        /* Just pick the first one. */
        PrimaryMonitor = Monitors[0];
        check(PrimaryMonitor)
    }
    LOG_VERBOSE(LogSurface, "Found [{}] physical monitors connected.", MonitorCount)
    for (auto MonitorIndex{0uz}; MonitorIndex < static_cast<LSize>(MonitorCount); ++MonitorIndex)
    {
        GLFWmonitor* Monitor{Monitors[MonitorIndex]};
        check(Monitor)

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

    {
        std::array Sizes{
            vk::DescriptorPoolSize{
                .type = vk::DescriptorType::eSampler,
                .descriptorCount = 2048, /* Completely arbitrary limit. */
                },
            vk::DescriptorPoolSize{
                .type = vk::DescriptorType::eSampledImage,
                .descriptorCount = 2048, /* Completely arbitrary limit. */
                },
            vk::DescriptorPoolSize{
                .type = vk::DescriptorType::eUniformBuffer,
                .descriptorCount = 2048, /* Completely arbitrary limit. */
                },
            vk::DescriptorPoolSize{
                .type = vk::DescriptorType::eStorageBuffer,
                .descriptorCount = 2048, /* Completely arbitrary limit. */
                },
            };
        this->Vk_DescriptorPool = vk::raii::DescriptorPool{this->Vk_Device, vk::DescriptorPoolCreateInfo{
                .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, // TODO: Flags??
                .maxSets = 1024, /* Completely arbitrary limit. */
                .poolSizeCount = static_cast<uint32_t>(Sizes.size()),
                .pPoolSizes = Sizes.data(),
            }};
    }

    if (this->Vk_DescriptorSetLayouts.contains("WorldData"))
    {
        LOG_WARNING(LogRhi, "Descriptor set layout for WorldData already exists, skipping creation.")
    }
    else
    {
        this->Vk_DescriptorSetLayouts.emplace("WorldData", vk::raii::DescriptorSetLayout{
            this->Vk_Device,
            vk::DescriptorSetLayoutCreateInfo{
                .bindingCount = static_cast<u32>(UBO::WorldData::Bindings().size()),
                .pBindings = UBO::WorldData::Bindings().data(),
                },
            });
    }
    if (this->Vk_DescriptorSetLayouts.contains("Jafg.VisualShared"))
    {
        LOG_WARNING(LogRhi, "Descriptor set layout for Jafg.VisualShared already exists, skipping creation.")
    }
    else
    {
        this->Vk_DescriptorSetLayouts.emplace("Jafg.VisualShared", vk::raii::DescriptorSetLayout{
            this->Vk_Device,
            vk::DescriptorSetLayoutCreateInfo{
                .bindingCount = static_cast<u32>(UBO::VisualShared::Bindings().size()),
                .pBindings = UBO::VisualShared::Bindings().data(),
                },
            });
    }
    if (this->Vk_DescriptorSetLayouts.contains("Jafg.BindlessTextures"))
    {
        LOG_WARNING(LogRhi, "Descriptor set layout for Jafg.BindlessTextures already exists, skipping creation.")
    }
    else
    {
        auto Bindings{UBO::BindlessTextureArray::GetBindings(this->Vk_BindlessTextureCapacity)};
        this->Vk_DescriptorSetLayouts.emplace("Jafg.BindlessTextures", vk::raii::DescriptorSetLayout{
            this->Vk_Device,
            vk::DescriptorSetLayoutCreateInfo{
                .pNext = &UBO::BindlessTextureArray::FlagsInfo(),
                .flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool,
                .bindingCount = static_cast<u32>(Bindings.size()),
                .pBindings = Bindings.data(),
                },
            });
    }

    {
        std::array Sizes{
            vk::DescriptorPoolSize{
                .type = vk::DescriptorType::eSampledImage,
                .descriptorCount = this->Vk_BindlessTextureCapacity,
                },
            vk::DescriptorPoolSize{
                .type = vk::DescriptorType::eSampler,
                .descriptorCount = UBO::BindlessTextureArray::SamplerCount,
                },
            };
        this->Vk_BindlessTextureArrayDescriptorPool = vk::raii::DescriptorPool{this->Vk_Device, vk::DescriptorPoolCreateInfo{
            .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind,
            .maxSets = 1,
            .poolSizeCount = static_cast<uint32_t>(Sizes.size()),
            .pPoolSizes = Sizes.data(),
            }};

        vk::DescriptorSetVariableDescriptorCountAllocateInfo CountInfo{
            .descriptorSetCount = 1,
            .pDescriptorCounts = &this->Vk_BindlessTextureCapacity,
            };
        auto Sets{this->Vk_Device.allocateDescriptorSets({
            .pNext = &CountInfo,
            .descriptorPool = this->Vk_BindlessTextureArrayDescriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &*this->Vk_GetMutableDescriptorSetLayouts().at("Jafg.BindlessTextures"),
            })};
        check(Sets.size() == 1)
        this->Vk_BindlessTextureArrayDescriptorSet = std::move(Sets[0]);
    }

    this->Vk_UpdateSamplers();

    this->AddSurface(std::move(QuerySurface), ENewSurfaceBehavior::FocusIfNonePresent);
    this->GetSurfaces().back()->LateSetupVk();

    check(this->Vk_FreeBindlessTextures.GetWords().empty())
    this->Vk_FreeBindlessTextures = LDynamicBitset{this->Vk_BindlessTextureCapacity};
    check(this->Vk_FreeBindlessTextures.GetBitCount() == this->Vk_BindlessTextureCapacity)

    LFrontendBase::Initialize(Outer);

    return;
}

void Jafg::LFrontendVk::TearDown()
{
    LFrontendBase::TearDown();

    GetMutableSingleton<JMeshSubsystem>().PurgeUnused();

    algo::swap_default(&this->Vk_ImmutableBuffers);

    this->Vk_DescriptorPool.reset();

    LOG_VERBOSE(LogVulkan, "Destroying VMA.")
    vmaDestroyAllocator(this->Vk_VmaAllocator);

    LOG_VERBOSE(LogSurface, "Terminating glfw.")
    glfwTerminate();

    return;
}


TOptional<Jafg::LPhysicalKey> Jafg::LFrontendVk::GetPhysicalKey(ENamedPhysicalKey LogicalKey) const noexcept
{
    auto Tl{::NamedPhysicalKeyToPhysicalKey(LogicalKey)};

    if (Tl.Scancodable != INDEX_NONE)
    {
        auto Scancode{glfwGetKeyScancode(Tl.Scancodable)};
        if (Scancode == INDEX_NONE)
        {
            return {};
        }
        check(Tl.Logical == INDEX_NONE)
        return LPhysicalKey{.Scancode = Scancode};
    }

    if (Tl.Logical != INDEX_NONE)
    {
        check(Tl.Scancodable == INDEX_NONE)
        return LPhysicalKey{.Logical = Tl.Logical};
    }

    return LPhysicalKey{};
}

TOptional<LString> Jafg::LFrontendVk::Glfw_GetPhysicalKeyLocalizedRepr(LPhysicalKey Key, i32 Hint) const noexcept
{
    switch (Hint)
    {
    case GLFW_KEY_ESCAPE: return "ESCAPE";
    case GLFW_KEY_ENTER: return "ENTER";
    case GLFW_KEY_TAB: return "TAB";
    case GLFW_KEY_BACKSPACE: return "BACKSPACE";
    case GLFW_KEY_INSERT: return "INSERT";
    case GLFW_KEY_DELETE: return "DELETE";
    case GLFW_KEY_RIGHT: return "RIGHT";
    case GLFW_KEY_LEFT: return "LEFT";
    case GLFW_KEY_DOWN: return "DOWN";
    case GLFW_KEY_UP: return "UP";
    case GLFW_KEY_PAGE_UP: return "PAGE_UP";
    case GLFW_KEY_PAGE_DOWN: return "PAGE_DOWN";
    case GLFW_KEY_HOME: return "HOME";
    case GLFW_KEY_END: return "END";
    case GLFW_KEY_CAPS_LOCK: return "CAPS_LOCK";
    case GLFW_KEY_SCROLL_LOCK: return "SCROLL_LOCK";
    case GLFW_KEY_NUM_LOCK: return "NUM_LOCK";
    case GLFW_KEY_PRINT_SCREEN: return "PRINT_SCREEN";
    case GLFW_KEY_PAUSE: return "PAUSE";
    case GLFW_KEY_F1: return "F1";
    case GLFW_KEY_F2: return "F2";
    case GLFW_KEY_F3: return "F3";
    case GLFW_KEY_F4: return "F4";
    case GLFW_KEY_F5: return "F5";
    case GLFW_KEY_F6: return "F6";
    case GLFW_KEY_F7: return "F7";
    case GLFW_KEY_F8: return "F8";
    case GLFW_KEY_F9: return "F9";
    case GLFW_KEY_F10: return "F10";
    case GLFW_KEY_F11: return "F11";
    case GLFW_KEY_F12: return "F12";
    case GLFW_KEY_F13: return "F13";
    case GLFW_KEY_F14: return "F14";
    case GLFW_KEY_F15: return "F15";
    case GLFW_KEY_F16: return "F16";
    case GLFW_KEY_F17: return "F17";
    case GLFW_KEY_F18: return "F18";
    case GLFW_KEY_F19: return "F19";
    case GLFW_KEY_F20: return "F20";
    case GLFW_KEY_F21: return "F21";
    case GLFW_KEY_F22: return "F22";
    case GLFW_KEY_F23: return "F23";
    case GLFW_KEY_F24: return "F24";
    case GLFW_KEY_F25: return "F25";
    case GLFW_KEY_KP_0: return "KP_0";
    case GLFW_KEY_KP_1: return "KP_1";
    case GLFW_KEY_KP_2: return "KP_2";
    case GLFW_KEY_KP_3: return "KP_3";
    case GLFW_KEY_KP_4: return "KP_4";
    case GLFW_KEY_KP_5: return "KP_5";
    case GLFW_KEY_KP_6: return "KP_6";
    case GLFW_KEY_KP_7: return "KP_7";
    case GLFW_KEY_KP_8: return "KP_8";
    case GLFW_KEY_KP_9: return "KP_9";
    case GLFW_KEY_KP_DECIMAL: return "KP_DECIMAL";
    case GLFW_KEY_KP_DIVIDE: return "KP_DIVIDE";
    case GLFW_KEY_KP_MULTIPLY: return "KP_MULTIPLY";
    case GLFW_KEY_KP_SUBTRACT: return "KP_SUBTRACT";
    case GLFW_KEY_KP_ADD: return "KP_ADD";
    case GLFW_KEY_KP_ENTER: return "KP_ENTER";
    case GLFW_KEY_KP_EQUAL: return "KP_EQUAL";
    case GLFW_KEY_LEFT_SHIFT: return "LEFT_SHIFT";
    case GLFW_KEY_LEFT_CONTROL: return "LEFT_CONTROL";
    case GLFW_KEY_LEFT_ALT: return "LEFT_ALT";
    case GLFW_KEY_LEFT_SUPER: return "LEFT_SUPER";
    case GLFW_KEY_RIGHT_SHIFT: return "RIGHT_SHIFT";
    case GLFW_KEY_RIGHT_CONTROL: return "RIGHT_CONTROL";
    case GLFW_KEY_RIGHT_ALT: return "RIGHT_ALT";
    case GLFW_KEY_RIGHT_SUPER: return "RIGHT_SUPER";
    case GLFW_KEY_MENU: return "MENU";
    default: break;
    }

    if (char const* Result{glfwGetKeyName(GLFW_KEY_UNKNOWN, Key.Scancode)})
    {
        return Result;
    }

    return {};
}

void Jafg::LFrontendVk::Vk_AddTextureToGlobalBindlessArray(LTexture2* Texture)
{
    check(Texture)
    check(Texture->IsOnDevice())
    check(Texture->IsBindless() == false)

    u64 Idx{this->Vk_FreeBindlessTextures.Allocate()};
    if (Idx == std::numeric_limits<u64>::max())
    {
        LOG_FATAL(LogVulkan, "[{}]: Failed to make texture bindless. Out of binding points.", Texture->GetPath())
    }
    check(Idx < this->Vk_BindlessTextureCapacity)
    LOG_VERBOSE(LogVulkan, "[{}]: Binding resource to global bindless texture array slot [{}].", Texture->GetPath(), Idx)

    vk::DescriptorImageInfo ImageInfo{
        .imageView = Texture->GetImageView(),
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        };
    std::array Writes{vk::WriteDescriptorSet{
        .dstSet = *this->Vk_BindlessTextureArrayDescriptorSet,
        .dstBinding = UBO::BindlessTextureArray::ArrayBinding,
        .dstArrayElement = static_cast<u32>(Idx),
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eSampledImage,
        .pImageInfo = &ImageInfo,
        }};
    this->Vk_Device.updateDescriptorSets(Writes, {});

    check(Idx <= std::numeric_limits<u64>::max())
    Texture->_SetBindlessIndex(static_cast<i64>(Idx));

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

Jafg::LDeviceImage Jafg::LFrontendVk::Vk_CreateImage(vk::ImageCreateInfo const& Info, VmaAllocationCreateInfo const& AllocationCreateInfo) const
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

Jafg::LDeviceImage Jafg::LFrontendVk::Vk_CreateDeviceLocalImage(vk::ImageCreateInfo const& Info) const
{
    VmaAllocationCreateInfo AllocationInfo{
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
        .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        };
    return this->Vk_CreateImage(Info, AllocationInfo);
}

Jafg::LDeviceImage Jafg::LFrontendVk::Vk_StageLinearImage(LStageLinearImageCreateInfo const& Info) const
{
    check(Info.Data)
    auto N{static_cast<size_t>(Vk_GetBytesPerPixel(Info.Info.format) * Info.Info.extent.width * Info.Info.extent.height)};
    auto StagingBuffer{this->Vk_CreateMappedBuffer({.size = N, .usage = vk::BufferUsageFlagBits::eTransferSrc})};
    std::memcpy(StagingBuffer.GetData(), Info.Data, N);

    auto Image{this->Vk_CreateDeviceLocalImage(Info.Info)};

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
        auto CommandBuffer{this->Vk_BeginSingleTimeCommands()};
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
            , vk::Extent2D{Info.Info.extent.width, Info.Info.extent.height}
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

void Jafg::LFrontendVk::Vk_TransitionImageLayout(vk::ImageMemoryBarrier2 const& Barrier) const
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
        jassert(this->Vk_SurfaceFormat.format == Format.format)
        jassert(this->Vk_SurfaceFormat.colorSpace == Format.colorSpace)
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
    if constexpr (IS_COMPILED_LOG(LogVulkan, Trace))
    {
        LOG_TRACE(LogVulkan, "Physical device memory properties:")
        for (auto Idx{0uz}; Idx < this->Vk_PhysicalDeviceMemoryProperties.memoryTypeCount; ++Idx)
        {
            auto const& MemType{ this->Vk_PhysicalDeviceMemoryProperties.memoryTypes[Idx] };
            auto const& MemHeap{ this->Vk_PhysicalDeviceMemoryProperties.memoryHeaps[MemType.heapIndex] };
            LOG_TRACE(LogVulkan, "    Type[{}]: Heap[{}] Size[{}MB] PropertyFlags[{}]",
                Idx,
                MemType.heapIndex,
                MemHeap.size / (1024 * 1024),
                vk::to_string(MemType.propertyFlags)
                )
        }
    }

    vk::PhysicalDeviceLimits Limits{this->Vk_PhysicalDevice.getProperties().limits};
    LOG_TRACE(LogVulkan, "Physical device limits:")
    LOG_TRACE(LogVulkan, "    maxImageDimension1D: {}", Limits.maxImageDimension1D)
    LOG_TRACE(LogVulkan, "    maxImageDimension2D: {}", Limits.maxImageDimension2D)
    LOG_TRACE(LogVulkan, "    maxImageDimension3D: {}", Limits.maxImageDimension3D)
    LOG_TRACE(LogVulkan, "    maxImageDimensionCube: {}", Limits.maxImageDimensionCube)
    LOG_TRACE(LogVulkan, "    maxImageArrayLayers: {}", Limits.maxImageArrayLayers)
    LOG_TRACE(LogVulkan, "    maxTexelBufferElements: {}", Limits.maxTexelBufferElements)
    LOG_TRACE(LogVulkan, "    maxUniformBufferRange: {}", Limits.maxUniformBufferRange)
    LOG_TRACE(LogVulkan, "    maxStorageBufferRange: {}", Limits.maxStorageBufferRange)
    LOG_TRACE(LogVulkan, "    maxPushConstantsSize: {}", Limits.maxPushConstantsSize)
    LOG_TRACE(LogVulkan, "    maxMemoryAllocationCount: {}", Limits.maxMemoryAllocationCount)
    LOG_TRACE(LogVulkan, "    maxSamplerAllocationCount: {}", Limits.maxSamplerAllocationCount)
    LOG_TRACE(LogVulkan, "    bufferImageGranularity: {}", Limits.bufferImageGranularity)
    LOG_TRACE(LogVulkan, "    sparseAddressSpaceSize: {}", Limits.sparseAddressSpaceSize)
    LOG_TRACE(LogVulkan, "    maxBoundDescriptorSets: {}", Limits.maxBoundDescriptorSets)
    LOG_TRACE(LogVulkan, "    maxPerStageDescriptorSamplers: {}", Limits.maxPerStageDescriptorSamplers)
    LOG_TRACE(LogVulkan, "    maxPerStageDescriptorUniformBuffers: {}", Limits.maxPerStageDescriptorUniformBuffers)
    LOG_TRACE(LogVulkan, "    maxPerStageDescriptorStorageBuffers: {}", Limits.maxPerStageDescriptorStorageBuffers)
    LOG_TRACE(LogVulkan, "    maxPerStageDescriptorSampledImages: {}", Limits.maxPerStageDescriptorSampledImages)
    LOG_TRACE(LogVulkan, "    maxPerStageDescriptorStorageImages: {}", Limits.maxPerStageDescriptorStorageImages)
    LOG_TRACE(LogVulkan, "    maxPerStageDescriptorInputAttachments: {}", Limits.maxPerStageDescriptorInputAttachments)
    LOG_TRACE(LogVulkan, "    maxPerStageResources: {}", Limits.maxPerStageResources)
    LOG_TRACE(LogVulkan, "    maxDescriptorSetSamplers: {}", Limits.maxDescriptorSetSamplers)
    LOG_TRACE(LogVulkan, "    maxDescriptorSetUniformBuffers: {}", Limits.maxDescriptorSetUniformBuffers)
    LOG_TRACE(LogVulkan, "    maxDescriptorSetUniformBuffersDynamic: {}", Limits.maxDescriptorSetUniformBuffersDynamic)
    LOG_TRACE(LogVulkan, "    maxDescriptorSetStorageBuffers: {}", Limits.maxDescriptorSetStorageBuffers)
    LOG_TRACE(LogVulkan, "    maxDescriptorSetStorageBuffersDynamic: {}", Limits.maxDescriptorSetStorageBuffersDynamic)
    LOG_TRACE(LogVulkan, "    maxDescriptorSetSampledImages: {}", Limits.maxDescriptorSetSampledImages)
    LOG_TRACE(LogVulkan, "    maxDescriptorSetStorageImages: {}", Limits.maxDescriptorSetStorageImages)
    LOG_TRACE(LogVulkan, "    maxDescriptorSetInputAttachments: {}", Limits.maxDescriptorSetInputAttachments)
    LOG_TRACE(LogVulkan, "    maxVertexInputAttributes: {}", Limits.maxVertexInputAttributes)
    LOG_TRACE(LogVulkan, "    maxVertexInputBindings: {}", Limits.maxVertexInputBindings)
    LOG_TRACE(LogVulkan, "    maxVertexInputAttributeOffset: {}", Limits.maxVertexInputAttributeOffset)
    LOG_TRACE(LogVulkan, "    maxVertexInputBindingStride: {}", Limits.maxVertexInputBindingStride)
    LOG_TRACE(LogVulkan, "    maxVertexOutputComponents: {}", Limits.maxVertexOutputComponents)
    LOG_TRACE(LogVulkan, "    maxTessellationGenerationLevel: {}", Limits.maxTessellationGenerationLevel)
    LOG_TRACE(LogVulkan, "    maxTessellationPatchSize: {}", Limits.maxTessellationPatchSize)
    LOG_TRACE(LogVulkan, "    maxTessellationControlPerVertexInputComponents: {}", Limits.maxTessellationControlPerVertexInputComponents)
    LOG_TRACE(LogVulkan, "    maxTessellationControlPerVertexOutputComponents: {}", Limits.maxTessellationControlPerVertexOutputComponents)
    LOG_TRACE(LogVulkan, "    maxTessellationControlPerPatchOutputComponents: {}", Limits.maxTessellationControlPerPatchOutputComponents)
    LOG_TRACE(LogVulkan, "    maxTessellationControlTotalOutputComponents: {}", Limits.maxTessellationControlTotalOutputComponents)
    LOG_TRACE(LogVulkan, "    maxTessellationEvaluationInputComponents: {}", Limits.maxTessellationEvaluationInputComponents)
    LOG_TRACE(LogVulkan, "    maxTessellationEvaluationOutputComponents: {}", Limits.maxTessellationEvaluationOutputComponents)
    LOG_TRACE(LogVulkan, "    maxGeometryShaderInvocations: {}", Limits.maxGeometryShaderInvocations)
    LOG_TRACE(LogVulkan, "    maxGeometryInputComponents: {}", Limits.maxGeometryInputComponents)
    LOG_TRACE(LogVulkan, "    maxGeometryOutputComponents: {}", Limits.maxGeometryOutputComponents)
    LOG_TRACE(LogVulkan, "    maxGeometryOutputVertices: {}", Limits.maxGeometryOutputVertices)
    LOG_TRACE(LogVulkan, "    maxGeometryTotalOutputComponents: {}", Limits.maxGeometryTotalOutputComponents)
    LOG_TRACE(LogVulkan, "    maxFragmentInputComponents: {}", Limits.maxFragmentInputComponents)
    LOG_TRACE(LogVulkan, "    maxFragmentOutputAttachments: {}", Limits.maxFragmentOutputAttachments)
    LOG_TRACE(LogVulkan, "    maxFragmentDualSrcAttachments: {}", Limits.maxFragmentDualSrcAttachments)
    LOG_TRACE(LogVulkan, "    maxFragmentCombinedOutputResources: {}", Limits.maxFragmentCombinedOutputResources)
    LOG_TRACE(LogVulkan, "    maxComputeSharedMemorySize: {}", Limits.maxComputeSharedMemorySize)
    LOG_TRACE(LogVulkan, "    maxComputeWorkGroupCount: {}", Limits.maxComputeWorkGroupCount)
    LOG_TRACE(LogVulkan, "    maxComputeWorkGroupInvocations: {}", Limits.maxComputeWorkGroupInvocations)
    LOG_TRACE(LogVulkan, "    maxComputeWorkGroupSize: {}", Limits.maxComputeWorkGroupSize)
    LOG_TRACE(LogVulkan, "    subPixelPrecisionBits: {}", Limits.subPixelPrecisionBits)
    LOG_TRACE(LogVulkan, "    subTexelPrecisionBits: {}", Limits.subTexelPrecisionBits)
    LOG_TRACE(LogVulkan, "    mipmapPrecisionBits: {}", Limits.mipmapPrecisionBits)
    LOG_TRACE(LogVulkan, "    maxDrawIndexedIndexValue: {}", Limits.maxDrawIndexedIndexValue)
    LOG_TRACE(LogVulkan, "    maxDrawIndirectCount: {}", Limits.maxDrawIndirectCount)
    LOG_TRACE(LogVulkan, "    maxSamplerLodBias: {}", Limits.maxSamplerLodBias)
    LOG_TRACE(LogVulkan, "    maxSamplerAnisotropy: {}", Limits.maxSamplerAnisotropy)
    LOG_TRACE(LogVulkan, "    maxViewports: {}", Limits.maxViewports)
    LOG_TRACE(LogVulkan, "    maxViewportDimensions: {}", Limits.maxViewportDimensions)
    LOG_TRACE(LogVulkan, "    viewportBoundsRange: {}", Limits.viewportBoundsRange)
    LOG_TRACE(LogVulkan, "    viewportSubPixelBits: {}", Limits.viewportSubPixelBits)
    LOG_TRACE(LogVulkan, "    minMemoryMapAlignment: {}", Limits.minMemoryMapAlignment)
    LOG_TRACE(LogVulkan, "    minTexelBufferOffsetAlignment: {}", Limits.minTexelBufferOffsetAlignment)
    LOG_TRACE(LogVulkan, "    minUniformBufferOffsetAlignment: {}", Limits.minUniformBufferOffsetAlignment)
    LOG_TRACE(LogVulkan, "    minStorageBufferOffsetAlignment: {}", Limits.minStorageBufferOffsetAlignment)
    LOG_TRACE(LogVulkan, "    minTexelOffset: {}", Limits.minTexelOffset)
    LOG_TRACE(LogVulkan, "    maxTexelOffset: {}", Limits.maxTexelOffset)
    LOG_TRACE(LogVulkan, "    minTexelGatherOffset: {}", Limits.minTexelGatherOffset)
    LOG_TRACE(LogVulkan, "    maxTexelGatherOffset: {}", Limits.maxTexelGatherOffset)
    LOG_TRACE(LogVulkan, "    minInterpolationOffset: {}", Limits.minInterpolationOffset)
    LOG_TRACE(LogVulkan, "    maxInterpolationOffset: {}", Limits.maxInterpolationOffset)
    LOG_TRACE(LogVulkan, "    subPixelInterpolationOffsetBits: {}", Limits.subPixelInterpolationOffsetBits)
    LOG_TRACE(LogVulkan, "    maxFramebufferWidth: {}", Limits.maxFramebufferWidth)
    LOG_TRACE(LogVulkan, "    maxFramebufferHeight: {}", Limits.maxFramebufferHeight)
    LOG_TRACE(LogVulkan, "    maxFramebufferLayers: {}", Limits.maxFramebufferLayers)
    LOG_TRACE(LogVulkan, "    framebufferColorSampleCounts: {}", vk::to_string(Limits.framebufferColorSampleCounts))
    LOG_TRACE(LogVulkan, "    framebufferDepthSampleCounts: {}", vk::to_string(Limits.framebufferDepthSampleCounts))
    LOG_TRACE(LogVulkan, "    framebufferStencilSampleCounts: {}", vk::to_string(Limits.framebufferStencilSampleCounts))
    LOG_TRACE(LogVulkan, "    framebufferNoAttachmentsSampleCounts: {}", vk::to_string(Limits.framebufferNoAttachmentsSampleCounts))
    LOG_TRACE(LogVulkan, "    maxColorAttachments: {}", Limits.maxColorAttachments)
    LOG_TRACE(LogVulkan, "    sampledImageColorSampleCounts: {}", vk::to_string(Limits.sampledImageColorSampleCounts))
    LOG_TRACE(LogVulkan, "    sampledImageIntegerSampleCounts: {}", vk::to_string(Limits.sampledImageIntegerSampleCounts))
    LOG_TRACE(LogVulkan, "    sampledImageDepthSampleCounts: {}", vk::to_string(Limits.sampledImageDepthSampleCounts))
    LOG_TRACE(LogVulkan, "    sampledImageStencilSampleCounts: {}", vk::to_string(Limits.sampledImageStencilSampleCounts))
    LOG_TRACE(LogVulkan, "    storageImageSampleCounts: {}", vk::to_string(Limits.storageImageSampleCounts))
    LOG_TRACE(LogVulkan, "    maxSampleMaskWords: {}", Limits.maxSampleMaskWords)
    LOG_TRACE(LogVulkan, "    timestampComputeAndGraphics: {}", Limits.timestampComputeAndGraphics)
    LOG_TRACE(LogVulkan, "    timestampPeriod: {}", Limits.timestampPeriod)
    LOG_TRACE(LogVulkan, "    maxClipDistances: {}", Limits.maxClipDistances)
    LOG_TRACE(LogVulkan, "    maxCullDistances: {}", Limits.maxCullDistances)
    LOG_TRACE(LogVulkan, "    maxCombinedClipAndCullDistances: {}", Limits.maxCombinedClipAndCullDistances)
    LOG_TRACE(LogVulkan, "    discreteQueuePriorities: {}", Limits.discreteQueuePriorities)
    LOG_TRACE(LogVulkan, "    pointSizeRange: {}", Limits.pointSizeRange)
    LOG_TRACE(LogVulkan, "    lineWidthRange: {}", Limits.lineWidthRange)
    LOG_TRACE(LogVulkan, "    pointSizeGranularity: {}", Limits.pointSizeGranularity)
    LOG_TRACE(LogVulkan, "    lineWidthGranularity: {}", Limits.lineWidthGranularity)
    LOG_TRACE(LogVulkan, "    strictLines: {}", Limits.strictLines)
    LOG_TRACE(LogVulkan, "    standardSampleLocations: {}", Limits.standardSampleLocations)
    LOG_TRACE(LogVulkan, "    optimalBufferCopyOffsetAlignment: {}", Limits.optimalBufferCopyOffsetAlignment)
    LOG_TRACE(LogVulkan, "    optimalBufferCopyRowPitchAlignment: {}", Limits.optimalBufferCopyRowPitchAlignment)
    LOG_TRACE(LogVulkan, "    nonCoherentAtomSize: {}", Limits.nonCoherentAtomSize)

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

namespace
{

template<typename TFeature, typename TProj>
void AssertAvailablePhysicalDeviceFeature(TFeature const& AvailableFeature, TFeature const& RequiredFeature, TProj Proj, LStringView FeatureName)
{
    if (std::invoke(Proj, AvailableFeature) != std::invoke(Proj, RequiredFeature))
    {
        LOG_FATAL(LogVulkan, "Required physical device feature [{}] is not available.", FeatureName)
    }
}

} /* +Namespace <Anonymous> */

void Jafg::LFrontendVk::Vk_CreateLogicalDevice(LSurface const& QuerySurface)
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan logical device.")

    auto QueueFamilyProperties{this->Vk_PhysicalDevice.getQueueFamilyProperties()};
    auto GraphicsQueueFamilyProperty{algo::find_if(QueueFamilyProperties, [](auto const& Qfp)
    {
        return (Qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
    })};
    if (GraphicsQueueFamilyProperty == QueueFamilyProperties.end())
    {
        panic("Failed to find a suitable graphics queue family.")
    }
    u32 GraphicsQueueFamilyIndex{static_cast<u32>(algo::distance(QueueFamilyProperties.begin(), GraphicsQueueFamilyProperty))};
    if (JAFG_UNLIKELY(GraphicsQueueFamilyIndex == QueueFamilyProperties.size()))
    {
        panic("Failed to find a suitable graphics queue family.")
    }

    /* We prefer a combined graphics+present queue (because performance), but also separate ones are ok. */
    u32 PresentQueueFamilyIndex{this->Vk_PhysicalDevice.getSurfaceSupportKHR(GraphicsQueueFamilyIndex, *QuerySurface.Vk_GetSurface())
        ? GraphicsQueueFamilyIndex
        : static_cast<u32>(QueueFamilyProperties.size())};

    if (PresentQueueFamilyIndex == QueueFamilyProperties.size())
    {
        /* Now try really hard to find a combined queue. */
        for (auto Idx{0uz}; Idx < QueueFamilyProperties.size(); ++Idx)
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
            for (auto Idx{0uz}; Idx < QueueFamilyProperties.size(); ++Idx)
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
        panic("Failed to find a suitable graphics queue family index.")
    }
    if (PresentQueueFamilyIndex == QueueFamilyProperties.size())
    {
        panic("Failed to find a suitable present queue family index.")
    }

    auto AvailableFeatures{this->Vk_PhysicalDevice.getFeatures2<
      vk::PhysicalDeviceFeatures2
    , vk::PhysicalDeviceVulkan11Features
    , vk::PhysicalDeviceVulkan13Features
    , vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
    , vk::PhysicalDeviceExtendedDynamicState3FeaturesEXT
    , vk::PhysicalDeviceDescriptorIndexingFeatures>()};
    auto& A1{AvailableFeatures.get<vk::PhysicalDeviceFeatures2>()};
    auto& A2{AvailableFeatures.get<vk::PhysicalDeviceVulkan11Features>()};
    auto& A3{AvailableFeatures.get<vk::PhysicalDeviceVulkan13Features>()};
    auto& A4{AvailableFeatures.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()};
    auto& A5{AvailableFeatures.get<vk::PhysicalDeviceExtendedDynamicState3FeaturesEXT>()};
    auto& A6{AvailableFeatures.get<vk::PhysicalDeviceDescriptorIndexingFeatures>()};

    typedef vk::StructureChain<
          vk::PhysicalDeviceFeatures2
        , vk::PhysicalDeviceVulkan11Features
        , vk::PhysicalDeviceVulkan13Features
        , vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
        , vk::PhysicalDeviceExtendedDynamicState3FeaturesEXT
        , vk::PhysicalDeviceDescriptorIndexingFeatures
        > RequiredFeaturesChain;
    RequiredFeaturesChain FeaturesChain{
            /* vk::PhysicalDeviceFeatures2 */ {.features = {
                .fillModeNonSolid = vk::True,
                .samplerAnisotropy = vk::True,
            }},
            /* vk::PhysicalDeviceVulkan11Features */{
                .shaderDrawParameters = vk::True
            },
            /* vk::PhysicalDeviceVulkan13Features */{
                .synchronization2 = vk::True, .dynamicRendering = vk::True
            },
            /* vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT */{
                .extendedDynamicState = vk::True,
            },
            /* vk::PhysicalDeviceExtendedDynamicState3FeaturesEXT */{
                .extendedDynamicState3PolygonMode = vk::True,
            },
            /* vk::PhysicalDeviceDescriptorIndexingFeatures */{
                .shaderSampledImageArrayNonUniformIndexing = vk::True,
                .descriptorBindingSampledImageUpdateAfterBind = vk::True,
                .descriptorBindingPartiallyBound = vk::True,
                .descriptorBindingVariableDescriptorCount = vk::True,
                .runtimeDescriptorArray = vk::True,
            },
        };
    auto& R1{FeaturesChain.get<vk::PhysicalDeviceFeatures2>()};
    auto& R2{FeaturesChain.get<vk::PhysicalDeviceVulkan11Features>()};
    auto& R3{FeaturesChain.get<vk::PhysicalDeviceVulkan13Features>()};
    auto& R4{FeaturesChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()};
    auto& R5{FeaturesChain.get<vk::PhysicalDeviceExtendedDynamicState3FeaturesEXT>()};
    auto& R6{FeaturesChain.get<vk::PhysicalDeviceDescriptorIndexingFeatures>()};

    #ifdef ASSERT_FEATURE
        #error "ASSERT_FEATURE is defined."
    #endif /* ASSERT_FEATURE */
    #define ASSERT_FEATURE(A, R, P) ::AssertAvailablePhysicalDeviceFeature(A, R, P, #P);
    ASSERT_FEATURE(A1.features, R1.features, &vk::PhysicalDeviceFeatures::fillModeNonSolid)
    ASSERT_FEATURE(A1.features, R1.features, &vk::PhysicalDeviceFeatures::samplerAnisotropy)
    ASSERT_FEATURE(A2, R2, &vk::PhysicalDeviceVulkan11Features::shaderDrawParameters)
    ASSERT_FEATURE(A3, R3, &vk::PhysicalDeviceVulkan13Features::synchronization2)
    ASSERT_FEATURE(A3, R3, &vk::PhysicalDeviceVulkan13Features::dynamicRendering)
    ASSERT_FEATURE(A4, R4, &vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT::extendedDynamicState)
    ASSERT_FEATURE(A5, R5, &vk::PhysicalDeviceExtendedDynamicState3FeaturesEXT::extendedDynamicState3PolygonMode)
    ASSERT_FEATURE(A6, R6, &vk::PhysicalDeviceDescriptorIndexingFeatures::shaderSampledImageArrayNonUniformIndexing)
    ASSERT_FEATURE(A6, R6, &vk::PhysicalDeviceDescriptorIndexingFeatures::descriptorBindingSampledImageUpdateAfterBind)
    ASSERT_FEATURE(A6, R6, &vk::PhysicalDeviceDescriptorIndexingFeatures::descriptorBindingPartiallyBound)
    ASSERT_FEATURE(A6, R6, &vk::PhysicalDeviceDescriptorIndexingFeatures::descriptorBindingVariableDescriptorCount)
    ASSERT_FEATURE(A6, R6, &vk::PhysicalDeviceDescriptorIndexingFeatures::runtimeDescriptorArray)
    #undef ASSERT_FEATURE

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

    std::array<vk::DescriptorImageInfo, UBO::BindlessTextureArray::SamplerCount> DescriptorImageInfos;

    vk::SamplerCreateInfo CreateInfo{
        .magFilter = vk::Filter::eLinear, .minFilter = vk::Filter::eLinear,
        .mipmapMode = vk::SamplerMipmapMode::eLinear,
        .addressModeU = vk::SamplerAddressMode::eRepeat, .addressModeV = vk::SamplerAddressMode::eRepeat, .addressModeW = vk::SamplerAddressMode::eRepeat,
        .mipLodBias = 0.0f,
        .anisotropyEnable = vk::True, .maxAnisotropy = this->Vk_PhysicalDevice.getProperties().limits.maxSamplerAnisotropy,
        .compareEnable = vk::False, .compareOp = vk::CompareOp::eAlways,
        .minLod = 0.0f, // Increase for worse texture quality.
        .maxLod = VK_LOD_CLAMP_NONE,
        .borderColor = vk::BorderColor::eFloatOpaqueWhite,
        };
    this->Vk_DefaultSamplers[UBO::BindlessTextureArray::Sampler::LinearRepeatSamplerIdx] = vk::raii::Sampler{this->Vk_Device, CreateInfo};
    DescriptorImageInfos[UBO::BindlessTextureArray::LinearRepeatSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::BindlessTextureArray::LinearRepeatSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eMirroredRepeat;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eMirroredRepeat;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eMirroredRepeat;
    this->Vk_DefaultSamplers[UBO::BindlessTextureArray::LinearMirroredRepeatSamplerIdx] = vk::raii::Sampler{this->Vk_Device, CreateInfo};
    DescriptorImageInfos[UBO::BindlessTextureArray::LinearMirroredRepeatSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::BindlessTextureArray::LinearMirroredRepeatSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    this->Vk_DefaultSamplers[UBO::BindlessTextureArray::LinearClampToEdgeSamplerIdx] = vk::raii::Sampler{this->Vk_Device, CreateInfo};
    DescriptorImageInfos[UBO::BindlessTextureArray::LinearClampToEdgeSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::BindlessTextureArray::LinearClampToEdgeSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eClampToBorder;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eClampToBorder;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eClampToBorder;
    this->Vk_DefaultSamplers[UBO::BindlessTextureArray::LinearClampToBorderSamplerIdx] = vk::raii::Sampler{this->Vk_Device, CreateInfo};
    DescriptorImageInfos[UBO::BindlessTextureArray::LinearClampToBorderSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::BindlessTextureArray::LinearClampToBorderSamplerIdx],
        };

    // CreateInfo.addressModeU = vk::SamplerAddressMode::eMirrorClampToEdge;
    // CreateInfo.addressModeV = vk::SamplerAddressMode::eMirrorClampToEdge;
    // CreateInfo.addressModeW = vk::SamplerAddressMode::eMirrorClampToEdge;
    // this->Vk_DefaultSamplers[UBO::BindlessTextureArray::LinearMirrorClampToEdgeSamplerIdx] = vk::raii::Sampler{this->Vk_Device, CreateInfo};
    // DescriptorImageInfos[UBO::BindlessTextureArray::LinearMirrorClampToEdgeSamplerIdx] = vk::DescriptorImageInfo{
    //     .sampler = this->Vk_DefaultSamplers[UBO::BindlessTextureArray::LinearMirrorClampToEdgeSamplerIdx],
    //     };

    CreateInfo.magFilter = vk::Filter::eNearest;
    CreateInfo.minFilter = vk::Filter::eNearest;
    CreateInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;

    CreateInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
    this->Vk_DefaultSamplers[UBO::BindlessTextureArray::Sampler::NearestRepeatSamplerIdx] = vk::raii::Sampler{this->Vk_Device, CreateInfo};
    DescriptorImageInfos[UBO::BindlessTextureArray::NearestRepeatSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::BindlessTextureArray::NearestRepeatSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eMirroredRepeat;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eMirroredRepeat;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eMirroredRepeat;
    this->Vk_DefaultSamplers[UBO::BindlessTextureArray::NearestMirroredRepeatSamplerIdx] = vk::raii::Sampler{this->Vk_Device, CreateInfo};
    DescriptorImageInfos[UBO::BindlessTextureArray::NearestMirroredRepeatSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::BindlessTextureArray::NearestMirroredRepeatSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    this->Vk_DefaultSamplers[UBO::BindlessTextureArray::NearestClampToEdgeSamplerIdx] = vk::raii::Sampler{this->Vk_Device, CreateInfo};
    DescriptorImageInfos[UBO::BindlessTextureArray::NearestClampToEdgeSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::BindlessTextureArray::NearestClampToEdgeSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eClampToBorder;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eClampToBorder;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eClampToBorder;
    this->Vk_DefaultSamplers[UBO::BindlessTextureArray::NearestClampToBorderSamplerIdx] = vk::raii::Sampler{this->Vk_Device, CreateInfo};
    DescriptorImageInfos[UBO::BindlessTextureArray::NearestClampToBorderSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::BindlessTextureArray::NearestClampToBorderSamplerIdx],
        };

    // CreateInfo.addressModeU = vk::SamplerAddressMode::eMirrorClampToEdge;
    // CreateInfo.addressModeV = vk::SamplerAddressMode::eMirrorClampToEdge;
    // CreateInfo.addressModeW = vk::SamplerAddressMode::eMirrorClampToEdge;
    // this->Vk_DefaultSamplers[UBO::BindlessTextureArray::NearestMirrorClampToEdgeSamplerIdx] = vk::raii::Sampler{this->Vk_Device, CreateInfo};
    // DescriptorImageInfos[UBO::BindlessTextureArray::NearestMirrorClampToEdgeSamplerIdx] = vk::DescriptorImageInfo{
    //     .sampler = this->Vk_DefaultSamplers[UBO::BindlessTextureArray::NearestMirrorClampToEdgeSamplerIdx],
    //     };

    std::array Writes{
        vk::WriteDescriptorSet{
            .dstSet = this->Vk_BindlessTextureArrayDescriptorSet,
            .dstBinding = UBO::BindlessTextureArray::SamplerBinding,
            .dstArrayElement = 0,
            .descriptorCount = static_cast<u32>(DescriptorImageInfos.size()),
            .descriptorType = vk::DescriptorType::eSampler,
            .pImageInfo = DescriptorImageInfos.data(),
            },
        };
    this->Vk_Device.updateDescriptorSets(Writes, {});

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

void Jafg::LFrontendVk::Vk_Generate2DMipMaps(vk::Image Image, vk::Format Format, vk::Extent2D Extent, u32 MipLevels) const
{
    check(MipLevels > 1)

    if (vk::FormatProperties FormatProperties{ this->Vk_PhysicalDevice.getFormatProperties(Format) };
        !(FormatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
    {
        panicMsgf( "The texture image format [{}] does not support linear blitting.", vk::to_string(Format) )
    }

    auto Buffer{this->Vk_BeginSingleTimeCommands()};

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

    for (auto Cursor{1uz}; Cursor < MipLevels; ++Cursor)
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
