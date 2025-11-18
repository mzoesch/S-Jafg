// Copyright mzoesch. All rights reserved.

#if JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER

#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "Engine/CoreGlobals.h"
#include "Engine/Engine.h"
#include "Async/TaskUtility.h"

#include "Rhi/RhiVendorInclude.h"
#include <GLFW/glfw3.h>

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
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
constexpr i32 VkMyMaxFramesInFlight{ 2 };

} /* ~Namespace <Anonymous> */

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static vk::VertexInputBindingDescription getBindingDescription()
    {
        return {
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = vk::VertexInputRate::eVertex
            };
    }

    static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions() {
        return {
            vk::VertexInputAttributeDescription{
                .location = 0, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(Vertex, pos)
                },
            vk::VertexInputAttributeDescription{
                .location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, color)
                },
            vk::VertexInputAttributeDescription{
                .location = 2, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(Vertex, texCoord)
                }
        };
    }
};
static_assert(std::is_standard_layout_v<Vertex>);

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};
static_assert(std::is_standard_layout_v<UniformBufferObject>);

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
    };

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
        // LOG_TRACE(LogSurface, "Width: {}, Height: {}", Width, Height)
    }

    static void WindowFocusCallback(GLFWwindow* Window, const i32 Focused)
    {
        LOG_WARNING(LogSurface, "Focused: {}", Focused)
    }

    static void CharCallback(::GLFWwindow* Window, const u32 Codepoint)
    {
        LOG_WARNING(LogSurface, "Codepoint: {}", Codepoint)
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->CharCallback(Codepoint);
    }

    static void KeyCallback(::GLFWwindow* Window, const i32 Key, const i32 Scancode, const i32 Action, const i32 Mods)
    {
        LOG_WARNING(LogSurface, "Key: {}, Scancode: {}, Action: {}, Mods: {}", Key, Scancode, Action, Mods)
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->KeyCallback(Key, Scancode, Action, Mods);
    }

    static void CursorPosCallback(::GLFWwindow* Window, const f64 XPos, const f64 YPos)
    {
        // LOG_TRACE(LogSurface, "XPos: {}, YPos: {}", XPos, YPos)
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->MouseCallback(XPos, YPos);
    }

    static void MouseButtonCallback(::GLFWwindow* Window, const i32 Button, const i32 Action, const i32 Mods)
    {
        LOG_WARNING(LogSurface, "Button: {}, Action: {}, Mods: {}", Button, Action, Mods)
    }

    static void CursorEnterCallback(::GLFWwindow* Window, const i32 Entered)
    {
        LOG_WARNING(LogSurface, "Entered: {}", Entered)
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->MouseEnterCallback(Entered);
    }

    static void FramebufferSizeCallback(::GLFWwindow* Window, const i32 Width, const i32 Height)
    {
        // LOG_TRACE(LogSurface, "Width: {}, Height: {}", Width, Height)
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->FramebufferSizeCallback(Width, Height);
    }

    static void ScrollCallback(::GLFWwindow* Window, const f64 XOffset, const f64 YOffset)
    {
        LOG_WARNING(LogSurface, "XOffset: {}, YOffset: {}", XOffset, YOffset)
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->ScrollCallback(XOffset, YOffset);
    }
};

} /* ~Namespace Jafg::Private */

Jafg::LSurfaceGlfw3::LSurfaceGlfw3() : Super{}
{
    STAT_CYCLE_FUNCTION()

    check( Tasks::IsOnMasterThread() )

    LOG_VERBOSE(LogSurface, "Creating Glfw3 window surface.")

    // TODO Do we need this still??
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
    LOG_VERBOSE(LogSurface, "Glfw3 window created. Dimensions: [{}x{}], DPI: [{}]", WindowDimensions.X, WindowDimensions.Y, PlatformDpi)

    this->GetViewport().SetBackgroundColor(Lal::LLinearColor::Black);

    VkSurfaceKHR CSurface;
    if (glfwCreateWindowSurface(*this->GetFrontend().GetVkInstance(), this->Handle, nullptr, &CSurface) != VK_SUCCESS)
    {
        panic( "Failed to create Vulkan window surface." )
    }
    check( CSurface )
    this->VkMySurface = vk::raii::SurfaceKHR{ this->GetFrontend().GetVkInstance(), CSurface };

    return;
}

Jafg::LSurfaceGlfw3::~LSurfaceGlfw3()
{
    {
        STAT_QUICK_CYCLE_START("VkDeviceWaitIdle")
        this->GetFrontend().GetVkDevice().waitIdle();
    }

    this->VertexBuffer.Free();

    if (this->Cursor)
    {
        LOG_VERBOSE(LogSurface, "Destroying glfw cursor.")
        glfwDestroyCursor(this->Cursor);
        this->Cursor = nullptr;
    }

    if (this->Handle)
    {
        LOG_VERBOSE(LogSurface, "Destroying glfw window.")
        glfwDestroyWindow(this->Handle);
        this->Handle = nullptr;
    }

    return;
}

void Jafg::LSurfaceGlfw3::LateSetupVk()
{
    this->VkCreateSwapchain();
    this->VkCreateImageViews();
    this->VkCreateDescriptorSetLayout();
    this->VkCreateGraphicsPipeline();
    this->VkCreateCommandPool();
    this->VkCreateTextureImage();
    this->VkCreateTextureImageView();
    this->VkCreateTextureSampler();
    this->VkCreateVertexBuffer();
    this->VkCreateIndexBuffer();
    this->VkCreateUniformBuffers();
    this->VkCreateDescriptorPool();
    this->VkCreateDescriptorSets();
    this->VkCreateCommandBuffers();
    this->VkCreateSynchObjects();

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

static_assert(UINT64_MAX == std::numeric_limits<u64>::max());

void Jafg::LSurfaceGlfw3::OnUpdate()
{
    STAT_CYCLE_FUNCTION()

    check( this->Handle )
    check( Tasks::IsOnRendererThread() )

    check( this->VkCommandBuffers.size() == ::VkMyMaxFramesInFlight )
    check( this->VkPresentSemaphores.size() == this->VkSwapchainImages.size() )
    check( this->VkRenderSemaphores.size() == this->VkSwapchainImages.size() )
    check( this->VkFlightFences.size() == ::VkMyMaxFramesInFlight )

    auto& Frontend{ this->GetFrontend() };

    while (vk::Result::eTimeout == Frontend.GetVkDevice().waitForFences(*this->VkFlightFences[this->VkFlightSyncFrameIndex], vk::True, UINT64_MAX))
        ;

    auto [Result, ImageIndex] = this->VkMySwapchain.acquireNextImage(
        std::numeric_limits<u64>::max(), this->VkPresentSemaphores[this->VkSemaphoreSyncIndex], nullptr
        );

    if (Result == vk::Result::eErrorOutOfDateKHR)
    {
        LOG_VERBOSE(LogSurface, "Swapchain is out of date. Recreating swapchain.")
        this->VkRecreateSwapchain();
        return;
    }

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

            LOG_VERBOSE(LogSurface, "Recreating swapchain due to suboptimal state and pending resize.")
            this->VkRecreateSwapchain();
            check( this->bPendingResize == false )
            return;
        }
    }
    else if (Result == vk::Result::eSuboptimalKHR)
    {
        LOG_VERBOSE(LogSurface, "Swapchain is suboptimal. Recreating swapchain.")
        this->VkRecreateSwapchain();
        return;
    }

    if (Result != vk::Result::eSuccess &&
        Result != vk::Result::eSuboptimalKHR /* Not an error. Just ignore. */
        )
    {
        panicMsgf( "Failed to acquire swapchain image. vk::Result: [{}].", static_cast<u32>(Result) )
    }

    this->VkUpdateUniformBuffers(this->VkFlightSyncFrameIndex);

    Frontend.GetVkDevice().resetFences(*this->VkFlightFences[this->VkFlightSyncFrameIndex]);

    this->VkCommandBuffers[this->VkFlightSyncFrameIndex].reset();
    this->RecordCommandBuffer(ImageIndex);

    vk::PipelineStageFlags DstStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput );
    const vk::SubmitInfo SubmitInfo{
        .waitSemaphoreCount = 1, .pWaitSemaphores = &*this->VkPresentSemaphores[this->VkSemaphoreSyncIndex],
        .pWaitDstStageMask = &DstStageMask,
        .commandBufferCount = 1, .pCommandBuffers = &*this->VkCommandBuffers[this->VkFlightSyncFrameIndex],
        .signalSemaphoreCount = 1, .pSignalSemaphores = &*this->VkRenderSemaphores[ImageIndex]
        };
    Frontend.GetVkGraphicsQueue().submit(SubmitInfo, *this->VkFlightFences[this->VkFlightSyncFrameIndex]);

    const vk::PresentInfoKHR PresentInfo{
        .waitSemaphoreCount = 1, .pWaitSemaphores = &*this->VkRenderSemaphores[ImageIndex],
        .swapchainCount = 1, .pSwapchains = &*this->VkMySwapchain,
        .pImageIndices = &ImageIndex
        };
    Result = Frontend.GetVkGraphicsQueue().presentKHR(PresentInfo);
    switch (Result)
    {
    case vk::Result::eSuccess:
    {
        break;
    }
    case vk::Result::eSuboptimalKHR:
    {
        LOG_WARNING(LogVulkan, "The swapchain is no longer optimal for the surface. Consider recreating the swapchain.")
        break;
    }
    default:
    {
        LOG_WARNING(LogVulkan, "vk::Queue::presentKHR returned unexpected vk::Result [{}].", static_cast<u32>(Result))
        break;
    }
    }

    Super::OnUpdate();

    this->VkSemaphoreSyncIndex = (this->VkSemaphoreSyncIndex + 1) % this->VkPresentSemaphores.size();
    this->VkFlightSyncFrameIndex = (this->VkFlightSyncFrameIndex + 1) % ::VkMyMaxFramesInFlight;

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
    else
    {
        glfwPollEvents();
    }

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

void Jafg::LSurfaceGlfw3::VkCreateSwapchain()
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan swapchain for Glfw3 surface.")

    auto& Frontend = this->GetFrontend();

    auto SurfaceCapabilities = Frontend.GetVkPhysicalDevice().getSurfaceCapabilitiesKHR(this->VkMySurface);
    auto AvailableFormats = Frontend.GetVkPhysicalDevice().getSurfaceFormatsKHR(this->VkMySurface);
    auto AvailablePresentModes = Frontend.GetVkPhysicalDevice().getSurfacePresentModesKHR(this->VkMySurface);

    this->VkMySwapchainSurfaceFormat = this->ChooseVkSwapSurfaceFormatKHR(AvailableFormats);
    this->VkMySwapchainPresentMode = this->ChooseVkSwapPresentModeKHR(Frontend.GetVkPhysicalDevice().getSurfacePresentModesKHR(this->VkMySurface));
    this->VkMySwapchainExtent = this->ChooseVkSwapExtent(SurfaceCapabilities);

    LOG_VERBOSE(LogVulkan, "Preferred surface format: Format [{}], Color Space [{}]",
        vk::to_string(this->VkMySwapchainSurfaceFormat.format),
        vk::to_string(this->VkMySwapchainSurfaceFormat.colorSpace)
        )
    LOG_VERBOSE(LogVulkan, "Preferred present mode: [{}]", vk::to_string(this->VkMySwapchainPresentMode))
    LOG_VERBOSE(LogVulkan, "Swapchain extent: [{}x{}]",
        this->VkMySwapchainExtent.width,
        this->VkMySwapchainExtent.height
        )

    u32 MinImageCount = Maths::Max(3u, SurfaceCapabilities.minImageCount); /* Default to triple buffering. */
    MinImageCount = (SurfaceCapabilities.maxImageCount > 0 && MinImageCount > SurfaceCapabilities.maxImageCount)
        ? SurfaceCapabilities.maxImageCount
        : MinImageCount;

    vk::SwapchainCreateInfoKHR SwapChainCreateInfo{
        .flags = vk::SwapchainCreateFlagsKHR{},
        .surface = this->VkMySurface,
        .minImageCount = MinImageCount,
        .imageFormat = this->VkMySwapchainSurfaceFormat.format,
        .imageColorSpace = this->VkMySwapchainSurfaceFormat.colorSpace,
        .imageExtent =  this->VkMySwapchainExtent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = SurfaceCapabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = this->VkMySwapchainPresentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
        };

    u32 QueueFamilyIndices[] = {Frontend.GetVkGraphicsQueueFamilyIndex(), Frontend.GetVkPresentQueueFamilyIndex()};
    if (Frontend.GetVkGraphicsQueue() != Frontend.GetVkPresentQueue())
    {
        LOG_VERBOSE(LogVulkan, "Using concurrent image sharing mode for swapchain as graphics and present queues differ.")
        SwapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent; /* TODO: Not optimal performance wise. But we can fix this later. */
        SwapChainCreateInfo.queueFamilyIndexCount = 2;
        SwapChainCreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
    }
    else
    {
        LOG_VERBOSE(LogVulkan, "Using exclusive image sharing mode for swapchain as graphics and present queues are the same.")
        SwapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        SwapChainCreateInfo.queueFamilyIndexCount = 0; /* Optional */
        SwapChainCreateInfo.pQueueFamilyIndices = nullptr; /* Optional */
    }

    this->VkMySwapchain = vk::raii::SwapchainKHR{ Frontend.GetVkDevice(), SwapChainCreateInfo };
    this->VkSwapchainImages = this->VkMySwapchain.getImages();

    this->GetViewport().ChangeDimensions(LIntVector2{
        static_cast<i32>(this->VkMySwapchainExtent.width),
        static_cast<i32>(this->VkMySwapchainExtent.height)
        });


    this->bPendingResize = false;
    this->PendingTimeForResizeApply = 0.0f;
    this->PendingWidth = 0;
    this->PendingHeight = 0;

    return;
}

vk::SurfaceFormatKHR Jafg::LSurfaceGlfw3::ChooseVkSwapSurfaceFormatKHR(std::vector<vk::SurfaceFormatKHR> const& AvailableFormats) const
{
    for (auto const& AvailableFormat : AvailableFormats)
    {
        if (AvailableFormat.format == vk::Format::eB8G8R8A8Srgb && AvailableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return AvailableFormat;
        }

        continue;
    }

    LOG_WARNING(LogVulkan, "Preferred swap surface format not found. Using first available format.")
    return AvailableFormats[0];
}

vk::PresentModeKHR Jafg::LSurfaceGlfw3::ChooseVkSwapPresentModeKHR(std::vector<vk::PresentModeKHR> const& AvailablePresentModes) const
{
    // VK_PRESENT_MODE_IMMEDIATE_KHR: Images submitted by your application are transferred to the
    //                                screen right away, which may result in tearing.
    // VK_PRESENT_MODE_FIFO_KHR: The swap chain is a queue where the display takes an image from the front of the
    //                           queue when the display is refreshed, and the program inserts rendered images at
    //                           the back of the queue. If the queue is full, then the program has to wait. This is
    //                           most similar to vertical sync as found in modern games. The moment that the display
    //                           is refreshed is known as "vertical blank".
    // VK_PRESENT_MODE_FIFO_RELAXED_KHR: This mode only differs from the previous one if the application is late and
    //                                   the queue was empty at the last vertical blank. Instead of waiting for the
    //                                   next vertical blank, the image is transferred right away when it finally
    //                                   arrives. This may result in visible tearing.
    // VK_PRESENT_MODE_MAILBOX_KHR: This is another variation of the second mode. Instead of blocking the
    //                              application when the queue is full, the images that are already queued are
    //                              simply replaced with the newer ones. This mode can be used to render frames as
    //                              fast as possible while still avoiding tearing, resulting in fewer latency issues
    //                              than standard vertical sync. This is commonly known as "triple buffering,"
    //                              although the existence of three buffers alone does not necessarily mean that
    //                              the framerate is unlocked

    for (const auto& AvailablePresentMode : AvailablePresentModes)
    {
        if (AvailablePresentMode == vk::PresentModeKHR::eMailbox)
        {
            return AvailablePresentMode;
        }

        continue;
    }

    LOG_WARNING(LogVulkan, "Preferred swap present mode not found. Using FIFO present mode.")
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D Jafg::LSurfaceGlfw3::ChooseVkSwapExtent(vk::SurfaceCapabilitiesKHR const& Capabilities) const
{
    if (Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return Capabilities.currentExtent;
    }

    i32 Width;
    i32 Height;
    glfwGetFramebufferSize(this->Handle, &Width, &Height);

    return
    {
        Maths::Clamp<u32>(Width, Capabilities.minImageExtent.width, Capabilities.maxImageExtent.width),
        Maths::Clamp<u32>(Height, Capabilities.minImageExtent.height, Capabilities.maxImageExtent.height)
    };
}

void Jafg::LSurfaceGlfw3::VkCreateImageViews()
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan image views for swapchain images.")

    auto& Frontend = this->GetFrontend();

    this->VkSwapchainImageViews.clear();

    for (const auto& SwapchainImage : this->VkSwapchainImages)
    {
        this->VkSwapchainImageViews.emplace_back(Frontend.CreateImageView(SwapchainImage, this->VkMySwapchainSurfaceFormat.format));
        continue;
    }

    LOG_VERBOSE(LogVulkan, "Created {} Vulkan image views for swapchain images.", this->VkSwapchainImageViews.size())

    return;
}

void Jafg::LSurfaceGlfw3::VkCreateDescriptorSetLayout()
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan descriptor set layout for surface.")

    std::array Bindings = {
          vk::DescriptorSetLayoutBinding{ 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr }
        , vk::DescriptorSetLayoutBinding{ 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr }
        };

    vk::DescriptorSetLayoutCreateInfo layoutInfo{.bindingCount = Bindings.size(), .pBindings = Bindings.data()};
    VkMyDescriptorSetLayout = vk::raii::DescriptorSetLayout(this->GetFrontend().GetVkDevice(), layoutInfo);

    return;
}

void Jafg::LSurfaceGlfw3::VkCreateGraphicsPipeline()
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan graphics pipeline for surface.")

    vk::raii::ShaderModule ShaderModule = this->CreateShaderModule(Finder::ReadFileAsBinary("Content/Shaders/Spir-V/Test.spv"));
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = ShaderModule,
        .pName = "vertMain"
        };
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = ShaderModule,
        .pName = "fragMain"
        };
    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    auto BindingDescription = Vertex::getBindingDescription();
    auto AttributeDescriptions = Vertex::getAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo   vertexInputInfo {
        .vertexBindingDescriptionCount = 1, .pVertexBindingDescriptions = &BindingDescription,
        .vertexAttributeDescriptionCount = AttributeDescriptions.size(), .pVertexAttributeDescriptions = AttributeDescriptions.data()
        };
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{.topology = vk::PrimitiveTopology::eTriangleList};
    vk::PipelineViewportStateCreateInfo      viewportState{.viewportCount = 1, .scissorCount = 1};

    vk::PipelineRasterizationStateCreateInfo rasterizer{.depthClampEnable = vk::False, .rasterizerDiscardEnable = vk::False,
        .polygonMode = vk::PolygonMode::eFill, .cullMode = vk::CullModeFlagBits::eBack, .frontFace = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable = vk::False, .depthBiasSlopeFactor = 1.0f, .lineWidth = 1.0f};

    vk::PipelineMultisampleStateCreateInfo multisampling{.rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False};

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{.blendEnable    = vk::False,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};
    vk::PipelineColorBlendStateCreateInfo colorBlending{.logicOpEnable = vk::False, .logicOp = vk::LogicOp::eCopy, .attachmentCount = 1, .pAttachments = &colorBlendAttachment};

    TArray<vk::DynamicState> dynamicStates{ vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    vk::PipelineDynamicStateCreateInfo dynamicState{ .dynamicStateCount = static_cast<u32>(dynamicStates.size()), .pDynamicStates = dynamicStates.data() };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{
        .setLayoutCount = 1,
        .pSetLayouts = &*this->VkMyDescriptorSetLayout,
        .pushConstantRangeCount = 0
        };

    this->VkMyPipelineLayout = vk::raii::PipelineLayout(this->GetFrontend().GetVkDevice(), pipelineLayoutInfo);

    // vk::SubpassDependency Dependency{
    //     .srcSubpass =  VK_SUBPASS_EXTERNAL,
    //     .dstSubpass = 0,
    //     .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
    //     .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
    //     .srcAccessMask = {},
    //     .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
    //     };

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain = {
        {.stageCount          = 2,
         .pStages             = shaderStages,
         .pVertexInputState   = &vertexInputInfo,
         .pInputAssemblyState = &inputAssembly,
         .pViewportState      = &viewportState,
         .pRasterizationState = &rasterizer,
         .pMultisampleState   = &multisampling,
         .pColorBlendState    = &colorBlending,
         .pDynamicState       = &dynamicState,
         .layout              = this->VkMyPipelineLayout,
         .renderPass          = nullptr},
        {.colorAttachmentCount = 1, .pColorAttachmentFormats = &this->VkMySwapchainSurfaceFormat.format}};

    this->VkMyPipeline = vk::raii::Pipeline{
        this->GetFrontend().GetVkDevice(),
        nullptr,
        pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()
        };

    LOG_VERBOSE(LogVulkan, "Created Vulkan graphics pipeline for surface.")

    return;
}

vk::raii::ShaderModule Jafg::LSurfaceGlfw3::CreateShaderModule(TArray<u8> const& Code) const
{
    vk::ShaderModuleCreateInfo createInfo{
        .codeSize = Code.size() * sizeof(char),
        .pCode = reinterpret_cast<u32 const*>(Code.data())
        };

    return vk::raii::ShaderModule{ this->GetFrontend().GetVkDevice(), createInfo };
}

void Jafg::LSurfaceGlfw3::VkCreateCommandPool()
{
    LOG_VERBOSE(LogVulkan, "Creating vk command pool for surface.")

    vk::CommandPoolCreateInfo PoolInfo{
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = this->GetFrontend().GetVkGraphicsQueueFamilyIndex()
        };

    this->VkMyCommandPool = vk::raii::CommandPool{ this->GetFrontend().GetVkDevice(), PoolInfo };

    return;
}

void Jafg::LSurfaceGlfw3::VkCreateCommandBuffers()
{
    LOG_VERBOSE(LogVulkan, "Creating [{}] vulkan command buffers for surface.", ::VkMyMaxFramesInFlight)

    this->VkCommandBuffers.clear();

    vk::CommandBufferAllocateInfo Info{
        .commandPool = this->VkMyCommandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = ::VkMyMaxFramesInFlight
        };

    this->VkCommandBuffers = vk::raii::CommandBuffers(this->GetFrontend().GetVkDevice(), Info);
    check( this->VkCommandBuffers.size() == ::VkMyMaxFramesInFlight )

    return;
}

void Jafg::LSurfaceGlfw3::VkCreateSynchObjects()
{
    LOG_VERBOSE(LogVulkan, "Creating [{}+2*{}] vulkan synchronization objects for surface.", ::VkMyMaxFramesInFlight, this->VkSwapchainImages.size())

    this->VkPresentSemaphores.clear();
    this->VkRenderSemaphores.clear();
    this->VkFlightFences.clear();

    auto& Device = this->GetFrontend().GetVkDevice();

    for (LSize Idx{ 0 }; Idx < this->VkSwapchainImages.size(); ++Idx)
    {
        this->VkPresentSemaphores.emplace_back(Device, vk::SemaphoreCreateInfo{});
        this->VkRenderSemaphores.emplace_back(Device, vk::SemaphoreCreateInfo{});
        continue;
    }

    for (LSize Idx{ 0 }; Idx < ::VkMyMaxFramesInFlight; ++Idx)
    {
        this->VkFlightFences.emplace_back(Device, vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled });
        continue;
    }

    return;
}

void Jafg::LSurfaceGlfw3::VkCleanSwapchain()
{
    LOG_TRACE(LogVulkan, "Cleaning up Vulkan swapchain for surface.")

    this->VkSwapchainImageViews.clear();
    this->VkMySwapchain = nullptr;

    return;
}

void Jafg::LSurfaceGlfw3::WaitForSemaphore(vk::raii::Semaphore const& Semaphore)
{
    vk::SemaphoreWaitInfo WaitInfo{
        .semaphoreCount = 1,
        .pSemaphores = &*Semaphore,
        .pValues = nullptr
        };

    if (this->GetFrontend().GetVkDevice().waitSemaphores(WaitInfo, std::numeric_limits<u64>::max()) != vk::Result::eSuccess)
    {
        panic("Failed to wait for semaphore.")
    }

    return;
}

void Jafg::LSurfaceGlfw3::WaitForSemaphores(TArray<vk::raii::Semaphore> const& Semaphores)
{
    for (const auto& Semaphore : Semaphores)
    {
        this->WaitForSemaphore(Semaphore);
    }

    return;
}

void Jafg::LSurfaceGlfw3::VkRecreateSwapchain()
{
    STAT_CYCLE_FUNCTION()

    i32 Width{ 0 };
    i32 Height{ 0 };
    glfwGetFramebufferSize(this->Handle, &Width, &Height);
    if (Width == 0 || Height == 0)
    {
        LOG_VERBOSE(LogSurface, "Waiting for non-zero dimensions to recreate vulkan swapchain for surface.")
        STAT_QUICK_CYCLE_START("GlfwWaitEventsForNonZeroFramebufferSize")
        while (Width == 0 || Height == 0)
        {
            glfwGetFramebufferSize(this->Handle, &Width, &Height);
            glfwWaitEvents();
        }
    }

    {
        STAT_QUICK_CYCLE_START("VkDevice.waitIdle")
        this->GetFrontend().GetVkDevice().waitIdle();

        check( this->VkFlightFences.size() == ::VkMyMaxFramesInFlight )
        check( this->VkPresentSemaphores.size() == this->VkSwapchainImages.size() )
        check( this->VkRenderSemaphores.size() == this->VkSwapchainImages.size() )

        // this->WaitForSemaphores(this->VkPresentSemaphores);
        // this->WaitForSemaphores(this->VkRenderSemaphores);
    }

    this->VkCleanSwapchain();
    this->VkCreateSwapchain();
    this->VkCreateImageViews();
    this->VkCreateSynchObjects();

    return;
}

void Jafg::LSurfaceGlfw3::VkCreateBuffer(
      vk::DeviceSize Size
    , vk::BufferUsageFlags Usage
    , vk::MemoryPropertyFlags Properties
    , vk::raii::Buffer& Buffer
    , vk::raii::DeviceMemory& BufferMemory
    )
{
    auto& Frontend = this->GetFrontend();

    vk::BufferCreateInfo bufferInfo{ .size = Size, .usage = Usage, .sharingMode = vk::SharingMode::eExclusive };
    Buffer = vk::raii::Buffer(Frontend.GetVkDevice(), bufferInfo);
    vk::MemoryRequirements memRequirements = Buffer.getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo{ .allocationSize = memRequirements.size, .memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, Properties) };
    BufferMemory = vk::raii::DeviceMemory(Frontend.GetVkDevice(), allocInfo);
    Buffer.bindMemory(*BufferMemory, 0);

    return;
}

void Jafg::LSurfaceGlfw3::RecordCommandBuffer(u32 ImageIndex)
{
    auto& TargetBuffer = this->VkCommandBuffers[this->VkFlightSyncFrameIndex];

    TargetBuffer.begin({});

    TransitionImageLayout(
        ImageIndex,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},                                                         // srcAccessMask (no need to wait for previous operations)
        vk::AccessFlagBits2::eColorAttachmentWrite,                 // dstAccessMask
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,         // srcStage
        vk::PipelineStageFlagBits2::eColorAttachmentOutput          // dstStage
        );

    vk::ClearValue clearColor = vk::ClearValue( vk::ClearColorValue( std::array<f32,4>{ 0.0f, 0.0f, 0.0f, 1.0f } ) );
    vk::RenderingAttachmentInfo attachmentInfo = {
        .imageView = this->VkSwapchainImageViews[ImageIndex],
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = clearColor
    };
    vk::RenderingInfo renderingInfo = {
        .renderArea = { .offset = { 0, 0 }, .extent = this->VkMySwapchainExtent },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentInfo
        };

    TargetBuffer.beginRendering(renderingInfo);

    TargetBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *this->VkMyPipeline);

    TargetBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<f32>(this->VkMySwapchainExtent.width), static_cast<f32>(this->VkMySwapchainExtent.height), 0.0f, 1.0f));
    TargetBuffer.setScissor( 0, vk::Rect2D( vk::Offset2D( 0, 0 ), this->VkMySwapchainExtent ) );

    TargetBuffer.bindVertexBuffers(0, this->VertexBuffer.Buffer, {0});
    TargetBuffer.bindIndexBuffer(this->IndexBuffer.Buffer, 0, vk::IndexTypeValue<decltype(::indices)::value_type>::value);
    TargetBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, this->VkMyPipelineLayout, 0 , *this->VkDescriptorSets[this->VkFlightSyncFrameIndex], nullptr);
    TargetBuffer.drawIndexed(::indices.size(), 1, 0, 0, 0);

    TargetBuffer.endRendering();

    TransitionImageLayout(
        ImageIndex,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite,                 // srcAccessMask
        {},                                                         // dstAccessMask
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,         // srcStage
        vk::PipelineStageFlagBits2::eBottomOfPipe                   // dstStage
        );

    TargetBuffer.end();

    return;
}

void Jafg::LSurfaceGlfw3::TransitionImageLayout(u32 ImageIndex, vk::ImageLayout OldLayout, vk::ImageLayout NewLayout,
                                                vk::AccessFlags2 SrcAccessMask, vk::AccessFlags2 DstAccessMask, vk::PipelineStageFlags2 SrcStage,
                                                vk::PipelineStageFlags2 DstStage)
{
    // LOG_TRACE(LogVulkan, "Transitioning image layout for swapchain image [{}] from [{}] to [{}].",
    //     ImageIndex,
    //     vk::to_string(OldLayout),
    //     vk::to_string(NewLayout)
    //     )

    vk::ImageMemoryBarrier2 Barrier = {
        .srcStageMask = SrcStage,
        .srcAccessMask = SrcAccessMask,
        .dstStageMask = DstStage,
        .dstAccessMask = DstAccessMask,
        .oldLayout = OldLayout,
        .newLayout = NewLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = this->VkSwapchainImages[ImageIndex],
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
            }
        };
    vk::DependencyInfo DependencyInfo = {
        .dependencyFlags = {},
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &Barrier
        };

    this->VkCommandBuffers[this->VkFlightSyncFrameIndex].pipelineBarrier2(DependencyInfo);

    return;
}

u32 Jafg::LSurfaceGlfw3::FindMemoryType(u32 TypeFilter, vk::MemoryPropertyFlags Properties) const
{
    vk::PhysicalDeviceMemoryProperties MemProperties = this->GetFrontend().GetVkPhysicalDevice().getMemoryProperties();
    for (u32 Idx{ 0 }; Idx < MemProperties.memoryTypeCount; ++Idx)
    {
        if ((TypeFilter & (1 << Idx)) && (MemProperties.memoryTypes[Idx].propertyFlags & Properties) == Properties)
        {
            return Idx;
        }
    }

    panic( "Failed to find suitable memory type." )
}

void Jafg::LSurfaceGlfw3::VkCreateTextureImage()
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan texture image for surface.")

    int texWidth, texHeight, _;
    stbi_uc* pixels = stbi_load("Content/Textures/statue-1275469.jpg", &texWidth, &texHeight, &_, STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        char const* error = stbi_failure_reason();
        panicMsgf("Failed to load texture image [Content/Textures/statue-1275469.jpg]. Reason: {}", error)
    }

    this->TextureImage = this->GetFrontend().VkStage2dImage(texWidth, texHeight, 4, pixels, this->VkMyCommandPool);
    stbi_image_free(pixels);

    return;
}

void Jafg::LSurfaceGlfw3::VkCreateTextureImageView()
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan texture image view for surface.")

    this->TextureImageView = this->GetFrontend().CreateImageView(this->TextureImage.Image, vk::Format::eR8G8B8A8Srgb);

    return;
}

void Jafg::LSurfaceGlfw3::VkCreateTextureSampler()
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan texture sampler for surface.")

    auto& Frontend = this->GetFrontend();

    vk::PhysicalDeviceProperties Properties = Frontend.GetVkPhysicalDevice().getProperties();
    vk::SamplerCreateInfo SamplerInfo{.magFilter = vk::Filter::eLinear, .minFilter = vk::Filter::eLinear,  .mipmapMode = vk::SamplerMipmapMode::eLinear,
        .addressModeU = vk::SamplerAddressMode::eRepeat, .addressModeV = vk::SamplerAddressMode::eRepeat, .addressModeW = vk::SamplerAddressMode::eRepeat,
        .mipLodBias = 0.0f,
        .anisotropyEnable = vk::True, .maxAnisotropy = Properties.limits.maxSamplerAnisotropy,
        .compareEnable = vk::False, .compareOp = vk::CompareOp::eAlways,
        .minLod = 0.0f, .maxLod = 0.0f,
        .borderColor = vk::BorderColor::eIntOpaqueBlack,
        };

    this->TextureSampler = vk::raii::Sampler(Frontend.GetVkDevice(), SamplerInfo);

    return;
}

void Jafg::LSurfaceGlfw3::VkCreateVertexBuffer()
{
    LOG_VERBOSE(LogTemporal, "Creating Vulkan vertex buffer for surface.")

    this->VertexBuffer = this->GetFrontend().VkStageVertexBuffer(
        vk::BufferCopy{ 0, 0, sizeof(vertices[0]) * vertices.size()},
        ::vertices.data(),
        this->VkMyCommandPool
        );

    // auto& Frontend = this->GetFrontend();
    // auto  Vma = Frontend.GetVma();
    //
    //
    // vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    //
    // // --- Staging buffer (CPU visible) ---
    // VkBuffer StagingBuffer;
    // VmaAllocation StagingAllocation;
    // VmaAllocationCreateInfo StagingAllocationCreateInfo{
    //     .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
    //     .usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
    //     };
    // vk::BufferCreateInfo StagingBufferCreateInfo{
    //     .size = bufferSize,
    //     .usage = vk::BufferUsageFlagBits::eTransferSrc,
    //     .sharingMode = vk::SharingMode::eExclusive
    //     };
    //
    // VmaAllocationInfo stagingAllocResult{};
    // if (auto Res = vmaCreateBuffer(
    //     Vma,
    //     &*StagingBufferCreateInfo,
    //     &StagingAllocationCreateInfo,
    //     &StagingBuffer,
    //     &StagingAllocation,
    //     &stagingAllocResult
    //     ); Res != VK_SUCCESS)
    // {
    //     panic("Failed to create vertex staging buffer.")
    // }
    // check( stagingAllocResult.pMappedData )
    // std::memcpy(stagingAllocResult.pMappedData, vertices.data(), static_cast<size_t>(bufferSize));
    //
    // // --- Device local buffer (GPU only) ---
    // VkBuffer vertexBuffer;
    // VmaAllocation vertexAllocation;
    // VmaAllocationCreateInfo vertexAllocInfo = {};
    // vertexAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    // vertexAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    //
    // VkBufferCreateInfo vertexBufferInfo = {};
    // vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    // vertexBufferInfo.size = bufferSize;
    // vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    //
    // vmaCreateBuffer(Vma, &vertexBufferInfo, &vertexAllocInfo, &vertexBuffer, &vertexAllocation, nullptr);
    //
    // // --- Copy from staging to device local ---
    // Frontend.VkCopyBuffer(*this->VkMyCommandPool, StagingBuffer, vertexBuffer, vk::BufferCopy{ 0, 0, bufferSize });
    //
    // // --- Cleanup staging buffer ---
    // vmaDestroyBuffer(Vma, StagingBuffer, StagingAllocation);
    //
    // // Store VMA handles for later destruction
    // this->VkMyVertexBuffer = vertexBuffer;
    // this->VmaMyVertexBufferAllocation = vertexAllocation;

    return;
}

void Jafg::LSurfaceGlfw3::VkCreateIndexBuffer()
{
    LOG_VERBOSE(LogTemporal, "Creating Vulkan index buffer for surface.")

    this->IndexBuffer = this->GetFrontend().VkStageIndexBuffer(
        vk::BufferCopy{ 0, 0, sizeof(indices[0]) * indices.size()},
        indices.data(),
        this->VkMyCommandPool
        );

    return;
}

void Jafg::LSurfaceGlfw3::VkCreateUniformBuffers()
{
    LOG_VERBOSE(LogTemporal, "Creating Vulkan uniform buffers for surface.")

    this->UniformBuffers.clear();

    auto& Frontend = this->GetFrontend();

    for (LSize Idx{ 0 }; Idx < ::VkMyMaxFramesInFlight; ++Idx)
    {
        this->UniformBuffers.emplace_back(Frontend.VkCreateMappedBuffer(vk::BufferCreateInfo{
                .size = sizeof(UniformBufferObject),
                .usage = vk::BufferUsageFlagBits::eUniformBuffer,
                }
            , vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
            ));
    }

    return;
}

void Jafg::LSurfaceGlfw3::VkCreateDescriptorPool()
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan descriptor pool for surface.")

    std::array PoolSize = {
        vk::DescriptorPoolSize( vk::DescriptorType::eUniformBuffer, ::VkMyMaxFramesInFlight),
        vk::DescriptorPoolSize( vk::DescriptorType::eCombinedImageSampler, ::VkMyMaxFramesInFlight)
        };

    vk::DescriptorPoolCreateInfo poolInfo{
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = ::VkMyMaxFramesInFlight,
        .poolSizeCount = static_cast<u32>(PoolSize.size()),
        .pPoolSizes = PoolSize.data()
        };

    this->VkMyDescriptorPool = vk::raii::DescriptorPool(this->GetFrontend().GetVkDevice(), poolInfo);

    return;
}

void Jafg::LSurfaceGlfw3::VkCreateDescriptorSets()
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan descriptor sets for surface.")

    auto& Frontend = this->GetFrontend();

    std::vector<vk::DescriptorSetLayout> layouts(VkMyMaxFramesInFlight, *VkMyDescriptorSetLayout);
    vk::DescriptorSetAllocateInfo        allocInfo{.descriptorPool = this->VkMyDescriptorPool,
        .descriptorSetCount = static_cast<uint32_t>(layouts.size()), .pSetLayouts = layouts.data()};

    this->VkDescriptorSets = Frontend.GetVkDevice().allocateDescriptorSets(allocInfo);

    for (size_t i = 0; i < VkMyMaxFramesInFlight; i++)
    {
        vk::DescriptorBufferInfo bufferInfo{.buffer = UniformBuffers[i].Buffer, .offset = 0, .range = sizeof(UniformBufferObject)};
        vk::DescriptorImageInfo imageInfo{ .sampler = this->TextureSampler, .imageView = this->TextureImageView, .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal};

        std::array DescriptorWrites = {
            vk::WriteDescriptorSet{ .dstSet = VkDescriptorSets[i], .dstBinding = 0, .dstArrayElement = 0, .descriptorCount = 1,
                .descriptorType = vk::DescriptorType::eUniformBuffer, .pBufferInfo = &bufferInfo },
            vk::WriteDescriptorSet{ .dstSet = VkDescriptorSets[i], .dstBinding = 1, .dstArrayElement = 0, .descriptorCount = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler, .pImageInfo = &imageInfo }
            };

        Frontend.GetVkDevice().updateDescriptorSets(DescriptorWrites, {});
    }

    return;
}

void Jafg::LSurfaceGlfw3::VkUpdateUniformBuffers(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto  currentTime = std::chrono::high_resolution_clock::now();
    float time        = std::chrono::duration<float>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view  = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj  = glm::perspective(glm::radians(45.0f), static_cast<float>(this->VkMySwapchainExtent.width) / static_cast<float>(this->VkMySwapchainExtent.height), 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    std::memcpy(this->UniformBuffers[currentImage].Data, &ubo, sizeof(ubo));

    return;
}

#endif /* JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */
