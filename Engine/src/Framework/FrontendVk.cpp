// Copyright mzoesch. All rights reserved.

/* Included for vkGetInstanceProcAddr and vkGetDeviceProcAddr */
#include <vulkan/vulkan.h>

#define VMA_IMPLEMENTATION
#include "Framework/FrontendVk.h"

#include "Framework/MeshSubsystem.h"
#include "Framework/TextureSubsystem.h"
#include "Stats/Stats.h"
#include "Engine/Engine.h"
#include "User/UserPreferences.h"
#include "Engine/WorldData.h"
#include "Rhi/VisualInstance.h"
#include "Rhi/Bindless.h"
#include "Runtime/Parameter.h"

#include <GLFW/glfw3.h>
#if JAFG_PLATFORM_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif /* JAFG_PLATFORM_WINDOWS */
#if JAFG_PLATFORM_LINUX
    #define GLFW_EXPOSE_NATIVE_X11
    #define GLFW_EXPOSE_NATIVE_WAYLAND
#endif /* JAFG_PLATFORM_LINUX */
// #define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#if JAFG_PLATFORM_LINUX
    // Xlib??? What why.
    #ifdef Bool
        #undef Bool
    #endif /* Bool */
    #ifdef Status
        #undef Status
    #endif /* Status */
    #ifdef True
        #undef True
    #endif /* True */
    #ifdef False
        #undef False
    #endif /* False */
#endif /* JAFG_PLATFORM_LINUX */

#include <nfd.h>
#include <nfd.hpp>
#include <nfd_glfw3.h>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace
{

constexpr i32 GlfwContextVersionMajor{ 3 };
constexpr i32 GlfwContextVersionMinor{ 3 };
constexpr u32 Vk_ApiVersion{ vk::ApiVersion14 };

} /* ~Namespace <Anonymous> */

namespace
{

Jafg::LProgramParameter Glfw_PlatformHint({
    .Identifier = "Jafg.PlatformHint",
    .Description = "Hints the platform to use. Valid are: [auto, wayland, x11, cocoa, win32].",
    .Flags = Jafg::EProgramParameterBits::Value,
    });

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

void GlfwMonitorCallback(GLFWmonitor* monitor, int event)
{
    if (event == GLFW_CONNECTED)
    {
        LOG_VERBOSE(LogSurface, "[{}]: Connected.", glfwGetMonitorName(monitor))
    }
    else if (event == GLFW_DISCONNECTED)
    {
        LOG_VERBOSE(LogSurface, "[{}]: Disconnected.", glfwGetMonitorName(monitor))
    }
    else
    {
        LOG_FATAL(LogSurface, "[{}]: Unknown event {}.", glfwGetMonitorName(monitor), event)
    }

    Jafg::Detail::GMutableEngine->GetLocalEgo().GetFrontend()._RefreshUsablePhysicalViewports();

    return;
}

struct LNamedPhysicalKeyToPhysicalKeyResult final
{
    i32 Scancodable{ INDEX_NONE };
    i32 Logical{ INDEX_NONE };
};

LNamedPhysicalKeyToPhysicalKeyResult NamedPhysicalKeyToPhysicalKey(Jafg::ELogicalKey Key) noexcept
{
    switch (Key)
    {
    case Jafg::ELogicalKey::Unresolved:       return {};
    case Jafg::ELogicalKey::LeftShift:        return {.Scancodable = GLFW_KEY_LEFT_SHIFT };
    case Jafg::ELogicalKey::RightShift:       return {.Scancodable = GLFW_KEY_RIGHT_SHIFT };
    case Jafg::ELogicalKey::LeftControl:      return {.Scancodable = GLFW_KEY_LEFT_CONTROL };
    case Jafg::ELogicalKey::RightControl:     return {.Scancodable = GLFW_KEY_RIGHT_CONTROL };
    case Jafg::ELogicalKey::LeftAlt:          return {.Scancodable = GLFW_KEY_LEFT_ALT };
    case Jafg::ELogicalKey::RightAlt:         return {.Scancodable = GLFW_KEY_RIGHT_ALT };
    case Jafg::ELogicalKey::LeftCommand:      return {.Scancodable = GLFW_KEY_LEFT_SUPER };
    case Jafg::ELogicalKey::RightCommand:     return {.Scancodable = GLFW_KEY_RIGHT_SUPER };
    case Jafg::ELogicalKey::PrintScreen:      return {.Scancodable = GLFW_KEY_PRINT_SCREEN };
    case Jafg::ELogicalKey::ScrollLock:       return {.Scancodable = GLFW_KEY_SCROLL_LOCK };
    case Jafg::ELogicalKey::Pause:            return {.Scancodable = GLFW_KEY_PAUSE };
    case Jafg::ELogicalKey::Insert:           return {.Scancodable = GLFW_KEY_INSERT };
    case Jafg::ELogicalKey::Home:             return {.Scancodable = GLFW_KEY_HOME };
    case Jafg::ELogicalKey::PageUp:           return {.Scancodable = GLFW_KEY_PAGE_UP };
    case Jafg::ELogicalKey::Delete:           return {.Scancodable = GLFW_KEY_DELETE };
    case Jafg::ELogicalKey::End:              return {.Scancodable = GLFW_KEY_END };
    case Jafg::ELogicalKey::PageDown:         return {.Scancodable = GLFW_KEY_PAGE_DOWN };
    case Jafg::ELogicalKey::Escape:           return {.Scancodable = GLFW_KEY_ESCAPE };
    case Jafg::ELogicalKey::Tilde:            return {.Scancodable = GLFW_KEY_GRAVE_ACCENT };
    case Jafg::ELogicalKey::Hyphen:           return {.Scancodable = GLFW_KEY_MINUS };
    case Jafg::ELogicalKey::Equals:           return {.Scancodable = GLFW_KEY_EQUAL };
    case Jafg::ELogicalKey::BackSpace:        return {.Scancodable = GLFW_KEY_BACKSPACE };
    case Jafg::ELogicalKey::Tab:              return {.Scancodable = GLFW_KEY_TAB };
    case Jafg::ELogicalKey::LeftBracket:      return {.Scancodable = GLFW_KEY_LEFT_BRACKET };
    case Jafg::ELogicalKey::RightBracket:     return {.Scancodable = GLFW_KEY_RIGHT_BRACKET };
    case Jafg::ELogicalKey::Enter:            return {.Scancodable = GLFW_KEY_ENTER };
    case Jafg::ELogicalKey::CapsLock:         return {.Scancodable = GLFW_KEY_CAPS_LOCK };
    case Jafg::ELogicalKey::Semicolon:        return {.Scancodable = GLFW_KEY_SEMICOLON };
    case Jafg::ELogicalKey::Apostrophe:       return {.Scancodable = GLFW_KEY_APOSTROPHE };
    case Jafg::ELogicalKey::Hashtag:          return {.Scancodable = GLFW_KEY_WORLD_1 };
    case Jafg::ELogicalKey::Backslash:        return {.Scancodable = GLFW_KEY_BACKSLASH };
    case Jafg::ELogicalKey::Comma:            return {.Scancodable = GLFW_KEY_COMMA };
    case Jafg::ELogicalKey::Period:           return {.Scancodable = GLFW_KEY_PERIOD };
    case Jafg::ELogicalKey::Slash:            return {.Scancodable = GLFW_KEY_SLASH };
    case Jafg::ELogicalKey::Space:            return {.Scancodable = GLFW_KEY_SPACE };
    case Jafg::ELogicalKey::Menu:             return {.Scancodable = GLFW_KEY_MENU };
    case Jafg::ELogicalKey::Left:             return {.Scancodable = GLFW_KEY_LEFT };
    case Jafg::ELogicalKey::Up:               return {.Scancodable = GLFW_KEY_UP };
    case Jafg::ELogicalKey::Right:            return {.Scancodable = GLFW_KEY_RIGHT };
    case Jafg::ELogicalKey::Down:             return {.Scancodable = GLFW_KEY_DOWN };
    case Jafg::ELogicalKey::A:                return {.Scancodable = GLFW_KEY_A };
    case Jafg::ELogicalKey::B:                return {.Scancodable = GLFW_KEY_B };
    case Jafg::ELogicalKey::C:                return {.Scancodable = GLFW_KEY_C };
    case Jafg::ELogicalKey::D:                return {.Scancodable = GLFW_KEY_D };
    case Jafg::ELogicalKey::E:                return {.Scancodable = GLFW_KEY_E };
    case Jafg::ELogicalKey::F:                return {.Scancodable = GLFW_KEY_F };
    case Jafg::ELogicalKey::G:                return {.Scancodable = GLFW_KEY_G };
    case Jafg::ELogicalKey::H:                return {.Scancodable = GLFW_KEY_H };
    case Jafg::ELogicalKey::I:                return {.Scancodable = GLFW_KEY_I };
    case Jafg::ELogicalKey::J:                return {.Scancodable = GLFW_KEY_J };
    case Jafg::ELogicalKey::K:                return {.Scancodable = GLFW_KEY_K };
    case Jafg::ELogicalKey::L:                return {.Scancodable = GLFW_KEY_L };
    case Jafg::ELogicalKey::M:                return {.Scancodable = GLFW_KEY_M };
    case Jafg::ELogicalKey::N:                return {.Scancodable = GLFW_KEY_N };
    case Jafg::ELogicalKey::O:                return {.Scancodable = GLFW_KEY_O };
    case Jafg::ELogicalKey::P:                return {.Scancodable = GLFW_KEY_P };
    case Jafg::ELogicalKey::Q:                return {.Scancodable = GLFW_KEY_Q };
    case Jafg::ELogicalKey::R:                return {.Scancodable = GLFW_KEY_R };
    case Jafg::ELogicalKey::S:                return {.Scancodable = GLFW_KEY_S };
    case Jafg::ELogicalKey::T:                return {.Scancodable = GLFW_KEY_T };
    case Jafg::ELogicalKey::U:                return {.Scancodable = GLFW_KEY_U };
    case Jafg::ELogicalKey::V:                return {.Scancodable = GLFW_KEY_V };
    case Jafg::ELogicalKey::W:                return {.Scancodable = GLFW_KEY_W };
    case Jafg::ELogicalKey::X:                return {.Scancodable = GLFW_KEY_X };
    case Jafg::ELogicalKey::Y:                return {.Scancodable = GLFW_KEY_Y };
    case Jafg::ELogicalKey::Z:                return {.Scancodable = GLFW_KEY_Z };
    case Jafg::ELogicalKey::Zero:             return {.Scancodable = GLFW_KEY_0 };
    case Jafg::ELogicalKey::One:              return {.Scancodable = GLFW_KEY_1 };
    case Jafg::ELogicalKey::Two:              return {.Scancodable = GLFW_KEY_2 };
    case Jafg::ELogicalKey::Three:            return {.Scancodable = GLFW_KEY_3 };
    case Jafg::ELogicalKey::Four:             return {.Scancodable = GLFW_KEY_4 };
    case Jafg::ELogicalKey::Five:             return {.Scancodable = GLFW_KEY_5 };
    case Jafg::ELogicalKey::Six:              return {.Scancodable = GLFW_KEY_6 };
    case Jafg::ELogicalKey::Seven:            return {.Scancodable = GLFW_KEY_7 };
    case Jafg::ELogicalKey::Eight:            return {.Scancodable = GLFW_KEY_8 };
    case Jafg::ELogicalKey::Nine:             return {.Scancodable = GLFW_KEY_9 };
    case Jafg::ELogicalKey::F1:               return {.Scancodable = GLFW_KEY_F1 };
    case Jafg::ELogicalKey::F2:               return {.Scancodable = GLFW_KEY_F2 };
    case Jafg::ELogicalKey::F3:               return {.Scancodable = GLFW_KEY_F3 };
    case Jafg::ELogicalKey::F4:               return {.Scancodable = GLFW_KEY_F4 };
    case Jafg::ELogicalKey::F5:               return {.Scancodable = GLFW_KEY_F5 };
    case Jafg::ELogicalKey::F6:               return {.Scancodable = GLFW_KEY_F6 };
    case Jafg::ELogicalKey::F7:               return {.Scancodable = GLFW_KEY_F7 };
    case Jafg::ELogicalKey::F8:               return {.Scancodable = GLFW_KEY_F8 };
    case Jafg::ELogicalKey::F9:               return {.Scancodable = GLFW_KEY_F9 };
    case Jafg::ELogicalKey::F10:              return {.Scancodable = GLFW_KEY_F10 };
    case Jafg::ELogicalKey::F11:              return {.Scancodable = GLFW_KEY_F11 };
    case Jafg::ELogicalKey::F12:              return {.Scancodable = GLFW_KEY_F12 };
    case Jafg::ELogicalKey::F13:              return {.Scancodable = GLFW_KEY_F13 };
    case Jafg::ELogicalKey::F14:              return {.Scancodable = GLFW_KEY_F14 };
    case Jafg::ELogicalKey::F15:              return {.Scancodable = GLFW_KEY_F15 };
    case Jafg::ELogicalKey::F16:              return {.Scancodable = GLFW_KEY_F16 };
    case Jafg::ELogicalKey::F17:              return {.Scancodable = GLFW_KEY_F17 };
    case Jafg::ELogicalKey::F18:              return {.Scancodable = GLFW_KEY_F18 };
    case Jafg::ELogicalKey::F19:              return {.Scancodable = GLFW_KEY_F19 };
    case Jafg::ELogicalKey::F20:              return {.Scancodable = GLFW_KEY_F20 };
    case Jafg::ELogicalKey::F21:              return {.Scancodable = GLFW_KEY_F21 };
    case Jafg::ELogicalKey::F22:              return {.Scancodable = GLFW_KEY_F22 };
    case Jafg::ELogicalKey::F23:              return {.Scancodable = GLFW_KEY_F23 };
    case Jafg::ELogicalKey::F24:              return {.Scancodable = GLFW_KEY_F24 };
    case Jafg::ELogicalKey::F25:              return {.Scancodable = GLFW_KEY_F25 };
    case Jafg::ELogicalKey::NumPadZero:       return {.Scancodable = GLFW_KEY_KP_0 };
    case Jafg::ELogicalKey::NumPadOne:        return {.Scancodable = GLFW_KEY_KP_1 };
    case Jafg::ELogicalKey::NumPadTwo:        return {.Scancodable = GLFW_KEY_KP_2 };
    case Jafg::ELogicalKey::NumPadThree:      return {.Scancodable = GLFW_KEY_KP_3 };
    case Jafg::ELogicalKey::NumPadFour:       return {.Scancodable = GLFW_KEY_KP_4 };
    case Jafg::ELogicalKey::NumPadFive:       return {.Scancodable = GLFW_KEY_KP_5 };
    case Jafg::ELogicalKey::NumPadSix:        return {.Scancodable = GLFW_KEY_KP_6 };
    case Jafg::ELogicalKey::NumPadSeven:      return {.Scancodable = GLFW_KEY_KP_7 };
    case Jafg::ELogicalKey::NumPadEight:      return {.Scancodable = GLFW_KEY_KP_8 };
    case Jafg::ELogicalKey::NumPadNine:       return {.Scancodable = GLFW_KEY_KP_9 };
    case Jafg::ELogicalKey::NumPadLock:       return {.Scancodable = GLFW_KEY_NUM_LOCK };
    case Jafg::ELogicalKey::NumPadDivide:     return {.Scancodable = GLFW_KEY_KP_DIVIDE };
    case Jafg::ELogicalKey::NumPadMultiply:   return {.Scancodable = GLFW_KEY_KP_MULTIPLY };
    case Jafg::ELogicalKey::NumPadSubtract:   return {.Scancodable = GLFW_KEY_KP_SUBTRACT };
    case Jafg::ELogicalKey::NumPadAdd:        return {.Scancodable = GLFW_KEY_KP_ADD };
    case Jafg::ELogicalKey::NumPadEnter:      return {.Scancodable = GLFW_KEY_KP_ENTER };
    case Jafg::ELogicalKey::NumPadDecimal:    return {.Scancodable = GLFW_KEY_KP_DECIMAL };
    default:
    {
        check(Key >= Jafg::ELogicalKey::FirstLogicalKey)
        check(Key <= Jafg::ELogicalKey::LastKey)
        return {
            .Logical = static_cast<std::underlying_type_t<Jafg::ELogicalKey>>(Key)
            };
    }
    }
}

} /* ~Namespace <Anonymous> */

#if !JAFG_IN_SHIPPING
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
#endif /* !JAFG_IN_SHIPPING */

void Jafg::Detail::FreeDeviceAllocation(vk::Buffer Handle, rhi::device_allocation Allocation) noexcept
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

void Jafg::Detail::FreeDeviceAllocation(vk::Image Handle, rhi::device_allocation Allocation) noexcept
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

    if (auto* Arg{App::GetCommandLineArgument(::Glfw_PlatformHint)})
    {
        auto Value{Arg->GetValue()};
        if (Value == "auto")
        {
        }
        else if (Value == "wayland")
        {
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
        }
        else if (Value == "x11")
        {
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
        }
        else if (Value == "cocoa")
        {
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_COCOA);
        }
        else if (Value == "win32")
        {
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WIN32);
        }
        else
        {
            LOG_FATAL(LogSurface
                , "Invalid value [{}] for command line argument [{}]. Valid values are: [auto, wayland, x11, cocoa, win32]."
                , Value, ::Glfw_PlatformHint.Identifier
                )
        }
    }

    // TODO: This does not work for wayland - but with x11.
    //       So we want that?
    // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    //# TODO: Move tis somewhere else? This has nothing to do with VK.
    if (!glfwInit())
    {
        panic("Failed to initialize glfw.")
    }

    glfwSetMonitorCallback(::GlfwMonitorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ::GlfwContextVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ::GlfwContextVersionMinor);

    glfwSetErrorCallback(::GlfwErrorCallback);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    check(this->Vk_Framework == rhi::framework::Identity)
    {
        auto Platform{glfwGetPlatform()};
#if JAFG_PLATFORM_WINDOWS
        if (Platform == GLFW_PLATFORM_WIN32)
        {
            this->Vk_Framework = rhi::framework::Win32;
        }
#endif /* JAFG_PLATFORM_WINDOWS */
        // if (Platform == GLFW_PLATFORM_COCOA)
        // {
        //     this->Vk_Framework = rhi::framework::Cocoa;
        // }
#if JAFG_PLATFORM_LINUX
        if (Platform == GLFW_PLATFORM_WAYLAND)
        {
            this->Vk_Framework = rhi::framework::Wayland;
        }
        else if (Platform == GLFW_PLATFORM_X11)
        {
            this->Vk_Framework = rhi::framework::x11;
        }
#endif /* JAFG_PLATFORM_LINUX */
        else
        {
            LOG_FATAL(LogSurface, "Unknown platform [{}] detected from glfw3.", Platform)
        }
    }
#if JAFG_PLATFORM_LINUX
    //
    // When using x11 we have a lot of power. But if this is just xWayland, we do not.
    // Therefore, we have to distinguish between the two.
    //
    if (this->Vk_Framework == rhi::framework::x11)
    {
        if (std::getenv("WAYLAND_DISPLAY") != nullptr || (std::getenv("XDG_SESSION_TYPE")
            && std::strcmp(std::getenv("XDG_SESSION_TYPE"), "wayland") == 0))
        {
            this->Vk_Framework = rhi::framework::xWayland;
        }
    }
#endif /* JAFG_PLATFORM_LINUX */
    LOG_VERBOSE(LogSurface, "Using framework [{}].", rhi::to_string(this->Vk_Framework))

    this->_RefreshUsablePhysicalViewports();

    LOG_VERBOSE(LogVulkan, "Initializing Vulkan.")

    LOG_VERBOSE(LogVulkan, "Setting up Vulkan dynamic dispatch loader.")
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    check(glfwVulkanSupported())

    if (NFD::Init() != NFD_OKAY)
    {
        if (auto* Error{NFD::GetError()})
        {
            LOG_FATAL(LogFrontend, "Failed to initialize native file dialog extended: {}", Error)
        }
        LOG_FATAL(LogFrontend, "Failed to initialize native file dialog extended.")
    }

    this->Vk_FetchAndCheckInstanceExtensions();
    this->Vk_FetchAndCheckInstanceLayers();
    this->Vk_CreateInstance();
#if !JAFG_IN_SHIPPING
    this->Vk_SetupDebugUtilsMessenger();
#endif /* !JAFG_IN_SHIPPING */

    LOG_VERBOSE(LogVulkan, "Initializing Vulkan dispatch loader with instance.")
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*this->Vk_Instance);

    this->Vk_PickPhysicalDevice();
    this->Vk_SetMaxMsaaSamples();

    /*
     * In order to create the best and optimal preferences for Vulkan, we first create a new surface to query
     * data from it. This will allow for some great optimizations...
     */
    auto QuerySurface{std::make_unique<LSurface>(LSurfaceCreateInfo{
#if !JAFG_IN_SHIPPING
        /* For development purposes, we want a smaller window as it does not cover so much space. */
        .DesiredDimensionsPx = { 855, 475 },
#endif /* !JAFG_IN_SHIPPING */
        .HumanReadableName = "Jafg - @mzoesch",
        })};

    this->Vk_CreateLogicalDevice(*QuerySurface);

    LOG_VERBOSE(LogVulkan, "Initializing Vulkan dispatch loader with device.")
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*this->Vk_Device);

    this->Vk_CreateVma();

    LOG_VERBOSE(LogVulkan, "Creating transient command pool.")
    this->Vk_TransientCommandPool = rhi::vk_build(this->Vk_Device, vk::CommandPoolCreateInfo{
        .flags = vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = this->Vk_GraphicsQueueFamilyIndex, // TODO: Queue that supports VK_QUEUE_TRANSFER_BIT.
        });

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

        this->Vk_DescriptorPool = rhi::vk_build(this->Vk_Device, vk::DescriptorPoolCreateInfo{
            .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, // TODO: Flags??
            .maxSets = 1024, /* Completely arbitrary limit. */
            .poolSizeCount = static_cast<uint32_t>(Sizes.size()),
            .pPoolSizes = Sizes.data(),
            });
    }

    if (this->Vk_DescriptorSetLayouts.contains("WorldData"))
    {
        LOG_WARNING(LogRhi, "Descriptor set layout for WorldData already exists, skipping creation.")
    }
    else
    {
        this->Vk_DescriptorSetLayouts.emplace("WorldData", rhi::vk_build(this->Vk_Device, vk::DescriptorSetLayoutCreateInfo{
            .bindingCount = static_cast<u32>(UBO::WorldData::Bindings().size()),
            .pBindings = UBO::WorldData::Bindings().data(),
            }));
    }
    if (this->Vk_DescriptorSetLayouts.contains("Jafg.VisualShared"))
    {
        LOG_WARNING(LogRhi, "Descriptor set layout for Jafg.VisualShared already exists, skipping creation.")
    }
    else
    {
        this->Vk_DescriptorSetLayouts.emplace("Jafg.VisualShared", rhi::vk_build(this->Vk_Device, vk::DescriptorSetLayoutCreateInfo{
            .bindingCount = static_cast<u32>(UBO::VisualShared::Bindings().size()),
            .pBindings = UBO::VisualShared::Bindings().data(),
            }));
    }
    if (this->Vk_DescriptorSetLayouts.contains("Jafg.BindlessTextures"))
    {
        LOG_WARNING(LogRhi, "Descriptor set layout for Jafg.BindlessTextures already exists, skipping creation.")
    }
    else
    {
        auto Bindings{UBO::Bindless::GetBindings(this->Vk_BindlessTextureCapacity)};
        this->Vk_DescriptorSetLayouts.emplace("Jafg.BindlessTextures", rhi::vk_build(this->Vk_Device, vk::DescriptorSetLayoutCreateInfo{
            .pNext = &UBO::Bindless::FlagsInfo(),
            .flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool,
            .bindingCount = static_cast<u32>(Bindings.size()),
            .pBindings = Bindings.data(),
            }));
    }

    {
        std::array Sizes{
            vk::DescriptorPoolSize{
                .type = vk::DescriptorType::eSampledImage,
                .descriptorCount = this->Vk_BindlessTextureCapacity,
                },
            vk::DescriptorPoolSize{
                .type = vk::DescriptorType::eSampler,
                .descriptorCount = UBO::Bindless::SamplerCount,
                },
            };
        this->Vk_BindlessTextureArrayDescriptorPool = rhi::vk_build(this->Vk_Device, vk::DescriptorPoolCreateInfo{
            .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind,
            .maxSets = 1,
            .poolSizeCount = static_cast<uint32_t>(Sizes.size()),
            .pPoolSizes = Sizes.data(),
            });

        vk::DescriptorSetVariableDescriptorCountAllocateInfo CountInfo{
            .descriptorSetCount = 1,
            .pDescriptorCounts = &this->Vk_BindlessTextureCapacity,
            };

        std::vector<vk::raii::DescriptorSet> Sets{rhi::vk_allocate(this->Vk_Device, vk::DescriptorSetAllocateInfo{
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

    this->GetMutableEngine().GetSubsystemChecked<JMeshSubsystem>()->PurgeUnused();

    algo::swap_default(&this->Vk_ImmutableBuffers);

    LOG_VERBOSE(LogVulkan, "Destroying transient command pool.")
    this->Vk_DescriptorPool.reset();

    LOG_VERBOSE(LogFrontend, "Terminating native file dialog extended.")
    NFD::Quit();

    LOG_VERBOSE(LogVulkan, "Destroying VMA.")
    vmaDestroyAllocator(this->Vk_VmaAllocator);

    LOG_VERBOSE(LogSurface, "Terminating glfw.")
    glfwTerminate();

    return;
}

std::optional<Jafg::LPhysicalKey> Jafg::LFrontendVk::GetPhysicalKey(ELogicalKey LogicalKey) const noexcept
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

std::optional<LString> Jafg::LFrontendVk::Glfw_GetPhysicalKeyLocalizedRepr(LPhysicalKey Key, i32 Hint) const noexcept
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

void Jafg::LFrontendVk::_RefreshUsablePhysicalViewports()
{
    LOG_VERBOSE(LogSurface, "Refreshing usable physical viewports.")

    this->UsablePhysicalViewports.clear();

    i32 MonitorCount{};
    auto Monitors{glfwGetMonitors(&MonitorCount)};
    if (MonitorCount < 1)
    {
        LOG_FATAL(LogSurface, "No suitable physical monitors detected.")
    }
    auto* PrimaryMonitor{glfwGetPrimaryMonitor()};
    if (!PrimaryMonitor)
    {
        LOG_VERBOSE(LogSurface, "No primary monitor detected, picking first available monitor as primary.")
        /* Just pick the first one. */
        PrimaryMonitor = Monitors[0];
        check(PrimaryMonitor)
    }
    LOG_VERBOSE(LogSurface, "Found [{}] physical monitors connected:", MonitorCount)
    for (auto MonitorIndex{0uz}; MonitorIndex < static_cast<std::size_t>(MonitorCount); ++MonitorIndex)
    {
        GLFWmonitor* Monitor{Monitors[MonitorIndex]};
        check(Monitor)

        LPhysicalViewport Pv{};
        Pv.Handle = Monitor;
        glfwGetMonitorPhysicalSize(Monitor, &Pv.SizeMm.x, &Pv.SizeMm.y);
        glfwGetMonitorContentScale(Monitor, &Pv.ContentScale.x, &Pv.ContentScale.y);
        glfwGetMonitorWorkarea(Monitor,
            &Pv.WorkareaOffsetPx.x, &Pv.WorkareaOffsetPx.y,
            &Pv.WorkareaPx.x, &Pv.WorkareaPx.y
            );
        Pv.Index = MonitorIndex;
        Pv.Name = glfwGetMonitorName(Monitor);
        /* Not supported on all target platforms, therefore, never use it to stay consistent. */
        // if (Monitor == PrimaryMonitor)
        // {
        //     Pv.bPrimary = true;
        // }
        // else
        // {
        //     check( Pv.bPrimary == false )
        // }

        int Count;
        auto* Modes{glfwGetVideoModes(Monitor, &Count)};
        check(Modes)
        for (auto ModeIndex{0uz}; ModeIndex < static_cast<std::size_t>(Count); ++ModeIndex)
        {
            auto& Mode{Modes[ModeIndex]};
            Pv.VideoModes.push_back({
                .Bits = {Mode.redBits, Mode.greenBits, Mode.blueBits},
                .ResolutionPx = {Mode.width, Mode.height},
                .RefreshRateHz = Mode.refreshRate,
                });
        }

        GLFWvidmode const* VidMode{glfwGetVideoMode(Monitor)};
        check(VidMode)
        Pv.CurrentVideoMode = LPhysicalViewport::VideoMode{
            .Bits = {VidMode->redBits, VidMode->greenBits, VidMode->blueBits},
            .ResolutionPx = {VidMode->width, VidMode->height},
            .RefreshRateHz = VidMode->refreshRate,
            };

        LOG_VERBOSE(LogSurface, "    Physical Monitor [{}-{}]: {}x{}px, {}x{}mm vid[{}]",
            Pv.Index, Pv.Name,
            Pv.WorkareaPx.x, Pv.WorkareaPx.y,
            Pv.SizeMm.x, Pv.SizeMm.y,
            Pv.CurrentVideoMode.ToHumanReadableString()
            )
        for (auto const& Mode : Pv.VideoModes)
        {
            LOG_VERBOSE(LogSurface, "        - {}", Mode.ToHumanReadableString())
        }
        this->UsablePhysicalViewports.emplace_back(std::move(Pv));
    }
    for (auto MonitorIndex{0uz}; MonitorIndex < static_cast<std::size_t>(MonitorCount); ++MonitorIndex)
    {
        GLFWmonitor* Monitor{Monitors[MonitorIndex]};
        check(Monitor)
        if (Monitor == PrimaryMonitor)
        {
            auto& Pv{this->UsablePhysicalViewports[MonitorIndex]};
            LOG_VERBOSE(LogSurface, "Physical monitor [{}-{}] is the primary monitor.", Pv.Index, Pv.Name)
            break;
        }
        continue;
    }

    return;
}

std::optional<rhi::present_mode> Jafg::LFrontendVk::Vk_GetFirstSurfacePresentMode() const noexcept
{
    if (this->GetSurfaces().empty())
    {
        return std::nullopt;
    }
    return rhi::vk_from_khr_present_mode(this->GetSurfaces()[0]->Vk_GetPresentMode());
}

void Jafg::LFrontendVk::Vk_AddTextureToGlobalBindlessArray(LTexture2* Texture)
{
    check(Texture)
    check(Texture->IsOnDevice())
    check(!Texture->IsBindless())

    Texture->_SetBindlessIndex(this->_VK_AddTransientImageToGlobalBindlessArray(Texture->GetImageView()).value_or(INDEX_NONE));
    if (!Texture->IsBindless())
    {
        LOG_FATAL(LogVulkan, "[{}]: Failed to make texture bindless. Out of binding points.", Texture->GetPath())
    }
    LOG_VERBOSE(LogVulkan, "[{}]: Binding resource to global bindless texture array slot [{}].", Texture->GetPath(), Texture->GetBindlessIndex())

    return;
}

std::optional<std::size_t> Jafg::LFrontendVk::_VK_AddTransientImageToGlobalBindlessArray(vk::ImageView const& ImageView)
{
    check(!!ImageView)

    u64 Idx{this->Vk_FreeBindlessTextures.Allocate()};
    if (Idx == std::numeric_limits<u64>::max())
    {
        return {};
    }
    check(Idx < this->Vk_BindlessTextureCapacity)

    vk::DescriptorImageInfo ImageInfo{
        .imageView = ImageView,
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        };
    std::array Writes{vk::WriteDescriptorSet{
        .dstSet = *this->Vk_BindlessTextureArrayDescriptorSet,
        .dstBinding = UBO::Bindless::ArrayBinding,
        .dstArrayElement = static_cast<u32>(Idx),
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eSampledImage,
        .pImageInfo = &ImageInfo,
        }};
    this->Vk_Device.updateDescriptorSets(Writes, {});

    return Idx;
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

    vk::raii::CommandBuffer CommandBuffer{ std::move(rhi::vk_allocate(this->Vk_Device, AllocateInfo).front()) };

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
    auto N{static_cast<std::size_t>(rhi::vk_bytes_per_pixel(Info.Info.format) * Info.Info.extent.width * Info.Info.extent.height)};
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

#if JAFG_WITH_EDITOR
void Jafg::LFrontendVk::Vk_EditorWaitIdle()
{
    // TODO: Some mutex checks for rendering??
    STAT_CYCLE_FUNCTION()
    this->Vk_Device.waitIdle();
    return;
}
#endif /* JAFG_WITH_EDITOR */

void Jafg::LFrontendVk::Vk_FetchAndCheckInstanceExtensions()
{
    LOG_VERBOSE(LogVulkan, "Refetching available instance extensions.")

    auto Result{this->Vk_Context.enumerateInstanceExtensionProperties()};
    check(Result.has_value())
    this->Vk_AvailableInstanceExtensions = std::move(*Result);
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

    auto Result{this->Vk_Context.enumerateInstanceLayerProperties()};
    check(Result.has_value())
    this->Vk_AvailableInstanceLayers = std::move(*Result);
    LOG_VERBOSE(LogVulkan, "Available Vulkan instance layers:")
    for (auto const& Layer : this->Vk_AvailableInstanceLayers)
    {
        LOG_VERBOSE(LogVulkan, "    {} spec[{}]", LStringView{Layer.layerName}, Layer.specVersion)
    }

#if !JAFG_IN_SHIPPING
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
#endif /* !JAFG_IN_SHIPPING */

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

    auto Result{vk::enumerateInstanceVersion()};
    check(Result.has_value())
    if (auto SupportedVersion{*Result}; SupportedVersion < ::Vk_ApiVersion)
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
        .applicationVersion = VK_MAKE_VERSION(DETAIL_ENGINE_VERSION_MAJOR, DETAIL_ENGINE_VERSION_MINOR, DETAIL_ENGINE_VERSION_PATCH),
        .pEngineName = "Jafg Engine",
        .engineVersion = VK_MAKE_VERSION(DETAIL_ENGINE_VERSION_MAJOR, DETAIL_ENGINE_VERSION_MINOR, DETAIL_ENGINE_VERSION_PATCH),
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

    auto InstanceResult{this->Vk_Context.createInstance({
        .pApplicationInfo = &ApplicationInfo,
        .enabledLayerCount = static_cast<u32>(RequiredInstanceLayers_c_str.size()),
        .ppEnabledLayerNames = RequiredInstanceLayers_c_str.data(),
        .enabledExtensionCount = static_cast<u32>(RequiredInstanceExtensions_c_str.size()),
        .ppEnabledExtensionNames = RequiredInstanceExtensions_c_str.data(),
        })};
    check(InstanceResult.has_value())
    this->Vk_Instance = std::move(*InstanceResult);

    return;
}

#if !JAFG_IN_SHIPPING
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

    auto Result{this->Vk_Instance.createDebugUtilsMessengerEXT(vk::DebugUtilsMessengerCreateInfoEXT{
        .messageSeverity = SeverityFlags,
        .messageType = TypeFlags,
        .pfnUserCallback = &::Hermes,
        .pUserData = this
        })};
    check(Result.has_value())
    this->Vk_DebugUtilsMessenger = std::move(*Result);

    return;
}
#endif /* !JAFG_IN_SHIPPING */

void Jafg::LFrontendVk::Vk_PickPhysicalDevice()
{
    LOG_VERBOSE(LogVulkan, "Picking Vulkan physical device.")

    this->Vk_PhysicalDevice = nullptr;
    algo::orphan(&this->Vk_AvailablePhysicalDevices);

    TArray<vk::raii::PhysicalDevice> AvailablePhysicalDevices;
    auto Result{this->Vk_Instance.enumeratePhysicalDevices()};
    check(Result.has_value())
    AvailablePhysicalDevices = std::move(*Result);

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

    if (const auto& Prefs{GetSingleton<JUserPreferences>()}; !Prefs.PreferredPhysicalDevice->empty())
    {
        for (auto const& [Rating, PhysicalDevice] : this->Vk_AvailablePhysicalDevices)
        {
            if (auto Properties{ PhysicalDevice.getProperties() }; Prefs.PreferredPhysicalDevice == Properties.deviceName)
            {
                if (Rating == 0)
                {
                    LOG_WARNING(LogVulkan, "Preferred physical device [{}] found but is no longer suitable. Falling back to best rated device.",
                        Prefs.PreferredPhysicalDevice
                        )
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

        LOG_WARNING(LogVulkan, "Preferred physical device [{}] not found among available devices. Falling back to best rated device.",
            Prefs.PreferredPhysicalDevice
            )
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
    this->Vk_MsaaSampleLimits = {
          PhysicalDeviceProperties.limits.framebufferColorSampleCounts
        & PhysicalDeviceProperties.limits.framebufferDepthSampleCounts
        };

    this->Vk_MaxMsaaSampleCount = rhi::vk_get_max_msaa_sample(this->Vk_MsaaSampleLimits);

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
    auto PresentQueueFamilySupport{this->Vk_PhysicalDevice.getSurfaceSupportKHR(GraphicsQueueFamilyIndex, *QuerySurface.Vk_GetSurface())};
    check(PresentQueueFamilySupport.has_value())
    u32 PresentQueueFamilyIndex{*PresentQueueFamilySupport
        ? GraphicsQueueFamilyIndex
        : static_cast<u32>(QueueFamilyProperties.size())};

    if (PresentQueueFamilyIndex == QueueFamilyProperties.size())
    {
        /* Now try really hard to find a combined queue. */
        for (auto Idx{0uz}; Idx < QueueFamilyProperties.size(); ++Idx)
        {
            if ((QueueFamilyProperties[Idx].queueFlags & vk::QueueFlagBits::eGraphics))
            {
                auto Result{this->Vk_PhysicalDevice.getSurfaceSupportKHR(static_cast<u32>(Idx), *QuerySurface.Vk_GetSurface())};
                check(Result.has_value())
                if (*Result)
                {
                    GraphicsQueueFamilyIndex = static_cast<u32>(Idx);
                    PresentQueueFamilyIndex  = GraphicsQueueFamilyIndex;
                    break;
                }
            }
            continue;
        }

        /* Yikes, ig we now have to tile this. */
        if (PresentQueueFamilyIndex == QueueFamilyProperties.size())
        {
            for (auto Idx{0uz}; Idx < QueueFamilyProperties.size(); ++Idx)
            {
                auto Result{this->Vk_PhysicalDevice.getSurfaceSupportKHR(static_cast<u32>(Idx), *QuerySurface.Vk_GetSurface())};
                check(Result.has_value())
                if (*Result)
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

    auto DeviceResult{this->Vk_PhysicalDevice.createDevice(DeviceCreateInfo)};
    check(DeviceResult.has_value())
    this->Vk_Device = std::move(*DeviceResult);
    this->Vk_GraphicsQueue = this->Vk_Device.getQueue(GraphicsQueueFamilyIndex, 0);
    this->Vk_PresentQueue = this->Vk_Device.getQueue(PresentQueueFamilyIndex, 0);

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

    std::array<vk::DescriptorImageInfo, UBO::Bindless::SamplerCount> DescriptorImageInfos;

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
    this->Vk_DefaultSamplers[UBO::Bindless::LinearRepeatSamplerIdx] = rhi::vk_build(this->Vk_Device, CreateInfo);
    DescriptorImageInfos[UBO::Bindless::LinearRepeatSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::LinearRepeatSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eMirroredRepeat;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eMirroredRepeat;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eMirroredRepeat;
    this->Vk_DefaultSamplers[UBO::Bindless::LinearMirroredRepeatSamplerIdx] = rhi::vk_build(this->Vk_Device, CreateInfo);
    DescriptorImageInfos[UBO::Bindless::LinearMirroredRepeatSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::LinearMirroredRepeatSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    this->Vk_DefaultSamplers[UBO::Bindless::LinearClampToEdgeSamplerIdx] = rhi::vk_build(this->Vk_Device, CreateInfo);
    DescriptorImageInfos[UBO::Bindless::LinearClampToEdgeSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::LinearClampToEdgeSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eClampToBorder;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eClampToBorder;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eClampToBorder;
    this->Vk_DefaultSamplers[UBO::Bindless::LinearClampToBorderSamplerIdx] = rhi::vk_build(this->Vk_Device, CreateInfo);
    DescriptorImageInfos[UBO::Bindless::LinearClampToBorderSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::LinearClampToBorderSamplerIdx],
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
    this->Vk_DefaultSamplers[UBO::Bindless::NearestRepeatSamplerIdx] = rhi::vk_build(this->Vk_Device, CreateInfo);
    DescriptorImageInfos[UBO::Bindless::NearestRepeatSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::NearestRepeatSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eMirroredRepeat;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eMirroredRepeat;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eMirroredRepeat;
    this->Vk_DefaultSamplers[UBO::Bindless::NearestMirroredRepeatSamplerIdx] = rhi::vk_build(this->Vk_Device, CreateInfo);
    DescriptorImageInfos[UBO::Bindless::NearestMirroredRepeatSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::NearestMirroredRepeatSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    this->Vk_DefaultSamplers[UBO::Bindless::NearestClampToEdgeSamplerIdx] = rhi::vk_build(this->Vk_Device, CreateInfo);
    DescriptorImageInfos[UBO::Bindless::NearestClampToEdgeSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::NearestClampToEdgeSamplerIdx],
        };

    CreateInfo.addressModeU = vk::SamplerAddressMode::eClampToBorder;
    CreateInfo.addressModeV = vk::SamplerAddressMode::eClampToBorder;
    CreateInfo.addressModeW = vk::SamplerAddressMode::eClampToBorder;
    this->Vk_DefaultSamplers[UBO::Bindless::NearestClampToBorderSamplerIdx] = rhi::vk_build(this->Vk_Device, CreateInfo);
    DescriptorImageInfos[UBO::Bindless::NearestClampToBorderSamplerIdx] = vk::DescriptorImageInfo{
        .sampler = this->Vk_DefaultSamplers[UBO::Bindless::NearestClampToBorderSamplerIdx],
        };

    // CreateInfo.addressModeU = vk::SamplerAddressMode::eMirrorClampToEdge;
    // CreateInfo.addressModeV = vk::SamplerAddressMode::eMirrorClampToEdge;
    // CreateInfo.addressModeW = vk::SamplerAddressMode::eMirrorClampToEdge;
    // this->Vk_DefaultSamplers[UBO::BindlessTextureArray::NearestMirrorClampToEdgeSamplerIdx] = rhi::vk_build(this->Vk_Device, CreateInfo);
    // DescriptorImageInfos[UBO::BindlessTextureArray::NearestMirrorClampToEdgeSamplerIdx] = vk::DescriptorImageInfo{
    //     .sampler = this->Vk_DefaultSamplers[UBO::BindlessTextureArray::NearestMirrorClampToEdgeSamplerIdx],
    //     };

    std::array Writes{
        vk::WriteDescriptorSet{
            .dstSet = this->Vk_BindlessTextureArrayDescriptorSet,
            .dstBinding = UBO::Bindless::SamplerBinding,
            .dstArrayElement = 0,
            .descriptorCount = static_cast<u32>(DescriptorImageInfos.size()),
            .descriptorType = vk::DescriptorType::eSampler,
            .pImageInfo = DescriptorImageInfos.data(),
            },
        };
    this->Vk_Device.updateDescriptorSets(Writes, {});

    return;
}

std::optional<vk::Format> Jafg::LFrontendVk::Vk_FindSupportedFormat(TArray<vk::Format> const& Candidates, vk::ImageTiling Tiling, vk::FormatFeatureFlags Features) const
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
        u64 Rating{};

        bool bSupportsGeometryShaders{};
        bool bSupportsVulkan14{};
        bool bSupportsGraphicsQueue{};
        // bool bSupportsPresentQueue{}; // TODO: How can we check this? Or is this obsolete?
        bool bSupportsRequiredExtensions{};
        bool bSupportsRequiredFeatures{};

        auto Properties = PhysicalDevice.getProperties();
        auto Features = PhysicalDevice.template getFeatures2<
              vk::PhysicalDeviceFeatures2
            , vk::PhysicalDeviceVulkan11Features
            , vk::PhysicalDeviceVulkan13Features
            , vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
            >();
        auto Families = PhysicalDevice.getQueueFamilyProperties();
        auto Result{PhysicalDevice.enumerateDeviceExtensionProperties()};
        check(Result.has_value())
        std::vector<vk::ExtensionProperties>& Extensions{*Result};

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

        bSupportsRequiredExtensions = algo::all_of(this->Vk_RequiredDeviceExtensions, [&Extensions](char const* RequiredPhysicalDeviceExtensions)
        {
            return algo::any_of(Extensions, [RequiredPhysicalDeviceExtensions](auto AvailablePhysicalDeviceExtension)
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
