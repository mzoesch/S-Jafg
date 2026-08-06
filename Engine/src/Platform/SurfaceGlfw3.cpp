// Copyright mzoesch. All rights reserved.

#if JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER

#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "Engine/Engine.h"
#include "Core/TaskUtility.h"
#include "Nodes/Viewport.h"
#include "Stats/Stats.h"
#include "Framework/SceneComponent.h"
#include "Nodes/UserWidget.h"
#include "Rhi/RendererCore.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/MaterialSubsystem.h"
#include "Framework/ShaderSubsystem.h"
#include "Framework/MeshSubsystem.h"
#include "Framework/FontSubsystem.h"

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

#include "Definitions/PushNoWarnings.h"
    #include <nfd.h>
    #include <nfd.hpp>
    #include <nfd_glfw3.h>
#include "Definitions/PopDiagnostics.h"

static_assert(UINT64_MAX == std::numeric_limits<u64>::max());

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
        // LOG_WARNING(LogSurface, "Focused: {}", Focused)
    }

    static void CharCallback(::GLFWwindow* Window, u32 Codepoint)
    {
        check(static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->_GetNativeHandleDangerous() == Window)
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->CharCallback(Codepoint);
    }

    static void KeyCallback(::GLFWwindow* Window, i32 Key, i32 Scancode, i32 Action, i32 Mods)
    {
        check(static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->_GetNativeHandleDangerous() == Window)
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->KeyCallback(Key, Scancode, Action, Mods);
    }

    static void CursorPosCallback(::GLFWwindow* Window, f64 XPos, f64 YPos)
    {
        check(static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->_GetNativeHandleDangerous() == Window)
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->MouseCallback(XPos, YPos);
    }

    static void MouseButtonCallback(::GLFWwindow* Window, i32 Button, i32 Action, i32 Mods)
    {
        check(static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->_GetNativeHandleDangerous() == Window)
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->MouseButtonCallback(Button, Action, Mods);
    }

    static void CursorEnterCallback(::GLFWwindow* Window, i32 Entered)
    {
        check(static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->_GetNativeHandleDangerous() == Window)
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->MouseEnterCallback(Entered);
    }

    static void FramebufferSizeCallback(::GLFWwindow* Window, i32 Width, i32 Height)
    {
        check(static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->_GetNativeHandleDangerous() == Window)
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->FramebufferSizeCallback(Width, Height);
    }

    static void ScrollCallback(::GLFWwindow* Window, f64 XOffset, f64 YOffset)
    {
        check(static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->_GetNativeHandleDangerous() == Window)
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->ScrollCallback(XOffset, YOffset);
    }
};

} /* ~Namespace Jafg::Private */

namespace
{

LString Glfw3CodePoint2Utf8(auto CodePoint)
{
    LString Result;

    if (CodePoint <= 0x7F)
    {
        Result.push_back(static_cast<char>(CodePoint));
    }
    else if (CodePoint <= 0x7FF)
    {
        Result.push_back(static_cast<char>(0xC0 | (CodePoint >> 6)));
        Result.push_back(static_cast<char>(0x80 | (CodePoint & 0x3F)));
    }
    else if (CodePoint <= 0xFFFF)
    {
        Result.push_back(static_cast<char>(0xE0 | (CodePoint >> 12)));
        Result.push_back(static_cast<char>(0x80 | ((CodePoint >> 6) & 0x3F)));
        Result.push_back(static_cast<char>(0x80 | (CodePoint & 0x3F)));
    }
    else
    {
        Result.push_back(static_cast<char>(0xF0 | (CodePoint >> 18)));
        Result.push_back(static_cast<char>(0x80 | ((CodePoint >> 12) & 0x3F)));
        Result.push_back(static_cast<char>(0x80 | ((CodePoint >> 6) & 0x3F)));
        Result.push_back(static_cast<char>(0x80 | (CodePoint & 0x3F)));
    }

    return Result;
}

} /* ~Namespace <Anonymous> */

Jafg::LSurfaceGlfw3::LSurfaceGlfw3(LSurfaceCreateInfo const& Info) : Super{Info}
{
    STAT_CYCLE_FUNCTION()

    check(Tasks::IsOnMasterThread())
    LOG_VERBOSE(LogSurface, "Creating Glfw3 window surface.")
    check(!(Info.bFullscreen && Info.bBorderless))

    if (this->CanEverResize())
    {
        this->bResizable = Info.bResizable;
        glfwWindowHint(GLFW_RESIZABLE, Info.bResizable ? GLFW_TRUE : GLFW_FALSE);
    }
    else
    {
        check(!this->IsResizable())
       if (Info.bResizable)
        {
            LOG_WARNING(LogSurface, "[{}]: The surface does not support resizing, ignoring request.", this->GetHumanReadableName())
        }
    }

    // glfwWindowHint(GLFW_DECORATED, Info.bBorderless ? GLFW_FALSE : GLFW_TRUE);
    //
    // So this is all wrong. Borderless is the only option on wayland/xwayland but other frameworks might give
    // us the power to become truly fullscreen. We have to detect this, else everything is kinda messy.
    //
    check(Info.bBorderless == false && "Borderless windows are not yet supported.")

    check(this->FallbackExtent)

    //
    // If Info.bFullscreen, the we could here create a fullscreen window. But this is really broken and inconsistent
    // across display frameworks, therefore, we create a normal window and then make it fullscreen later.
    //
    {
        STAT_QUICK_CYCLE_START("Glfw3WindowCreation")
        this->Handle = glfwCreateWindow(
            Info.DesiredDimensionsPx.width, Info.DesiredDimensionsPx.height,
            this->GetHumanReadableName().c_str(),
            nullptr, nullptr
            );
        // glfwGetWindowAttrib
        // glfwGetWindowSize
        // glfwGetFramebufferSize
        // monitor_monitors
    }
    if (this->Handle == nullptr)
    {
        panic("Failed to create glfw window.")
    }

    glfwSetWindowUserPointer(this->Handle, this);

    glfwSetWindowCloseCallback(this->Handle, Private::LGlfw3Bridge::WindowCloseCallback);
    glfwSetWindowSizeCallback(this->Handle, Private::LGlfw3Bridge::WindowSizeCallback);
    glfwSetWindowFocusCallback(this->Handle, Private::LGlfw3Bridge::WindowFocusCallback);
    glfwSetCharCallback(this->Handle, Private::LGlfw3Bridge::CharCallback);
    glfwSetKeyCallback(this->Handle, Private::LGlfw3Bridge::KeyCallback);
    glfwSetCursorPosCallback(this->Handle, Private::LGlfw3Bridge::CursorPosCallback);
    glfwSetMouseButtonCallback(this->Handle, Private::LGlfw3Bridge::MouseButtonCallback);
    glfwSetCursorEnterCallback(this->Handle, Private::LGlfw3Bridge::CursorEnterCallback);
    glfwSetFramebufferSizeCallback(this->Handle, Private::LGlfw3Bridge::FramebufferSizeCallback);
    glfwSetScrollCallback(this->Handle, Private::LGlfw3Bridge::ScrollCallback);

    /* TODO: Do we really want this?? */
    glfwSetInputMode(this->Handle, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(this->Handle, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    check(GEngine)

#if JAFG_PLATFORM_WINDOWS
    const HWND NativeWindowHandle{glfwGetWin32Window(this->Handle)};
    check(NativeWindowHandle)
    const u32 PlatformDpi{::GetDpiForWindow(NativeWindowHandle)};
#else /* JAFG_PLATFORM_WINDOWS */
    const u32 PlatformDpi = 96; // Sketchy
#endif /* !JAFG_PLATFORM_WINDOWS */
    // TODO: Update this when the window is moved to another monitor with different DPI.
    this->GetViewport().SetPlatformDpi(static_cast<f32>(PlatformDpi));
    LOG_VERBOSE(LogSurface, "Glfw3 window created. Dimensions: {}, DPI: [{}]"
        , this->GetSurfaceExtent(), PlatformDpi)

    auto& Instance{this->GetFrontend().Vk_GetInstance()};
    VkSurfaceKHR CSurface;
    if (glfwCreateWindowSurface(*Instance, this->Handle, nullptr, &CSurface) != VK_SUCCESS)
    {
        panic("Failed to create Vulkan window surface.")
    }
    check(CSurface)
    this->Vk_Surface = vk::raii::SurfaceKHR{Instance, CSurface};

    if (Info.bFullscreen)
    {
        Tasks::Make(ENamedThreads::Master, ETaskTime::Late, [this]{ this->SetFullscreen(true); });
    }

    if (this->GetFrontend().GetCollection()->GetClass().HasClass())
    {
        this->LateSetupVk();
    }
}

Jafg::LSurfaceGlfw3::~LSurfaceGlfw3()
{
    this->GetMutableFrontend()._Vk_WaitIdle();

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
}

void Jafg::LSurfaceGlfw3::LateSetupVk()
{
    this->Vk_CreateCommandPool();
    this->Vk_CreateSwapchain();
    this->Vk_CreateCommandBuffers();
    this->Vk_CreateDescriptorPools();

    this->GetViewport().Vk_OnLateInit();
}

void Jafg::LSurfaceGlfw3::PollPlatformEvents()
{
    STAT_CYCLE_FUNCTION()

    check(this->Handle)
    check(Tasks::IsOnMasterThread())

    if (glfwWindowShouldClose(this->Handle))
    {
        if (this->bShutdownEngineOnClose)
        {
            App::RequestEngineExit("Window closed by user.");
        }
        else
        {
            Tasks::Make(ENamedThreads::Master, ETaskTime::Late, [this]
            {
                this->GetMutableFrontend().RemoveSurface(*this);
            });
        }
    }
}

void Jafg::LSurfaceGlfw3::OnRender()
{
    STAT_CYCLE_FUNCTION()

    check(this->Handle)
    check(Tasks::IsOnRendererThread())

    check(this->Vk_LastFrameInFlightIndex < rhi::max_frames_in_flight)

    check(this->Vk_CommandBuffers.size() == this->Vk_GetNumberOfFramesInFlightInternal())

    auto& Frontend{this->GetMutableFrontend()};

    check(this->Vk_GetNumberOfFramesInFlightInternal() == Frontend.Vk_GetNumberOfFramesInFlight())
    this->Vk_CurrentFrameInFlightIndex = static_cast<u32>((this->Vk_LastFrameInFlightIndex + 1) % this->Vk_GetNumberOfFramesInFlightInternal());

    while (vk::Result::eTimeout == Frontend.Vk_GetDevice().waitForFences(*this->Vk_FlightFences[*this->Vk_CurrentFrameInFlightIndex], vk::True, UINT64_MAX))
        ;

    auto [Result, ImageIndex] = this->Vk_VkMySwapchain.acquireNextImage(
        std::numeric_limits<u64>::max(), this->Vk_ImageAvailableSemaphores[*this->Vk_CurrentFrameInFlightIndex], nullptr
        );
    if (Result == vk::Result::eErrorOutOfDateKHR)
    {
        //
        // So, this just sucks when resizing on x11. But said framework is just too old/bad for that and throws
        // (instead of accepting the status quo and take the performance hit of a suboptimal chain). Why.
        // It would be too expansive (and unresponsive) to recreate the swapchain for every poll.
        //
        if (this->bPendingResize && this->GetFrontend().Vk_GetFramework() == rhi::framework::x11)
        {
            this->PendingTimeForResizeApply -= static_cast<f32>(GEngine->DeltaTime);
            if (this->PendingTimeForResizeApply > 0.0f)
            {
                /* Just skip. We do not care. */
                return;
            }
        }

        LOG_VERBOSE(LogSurface, "Swapchain is out of date. Recreating swapchain.")
        this->Vk_CurrentFrameInFlightIndex.reset();
        this->Vk_CreateSwapchain();
        return;
    }

    /* Must check #ImageIndex after eErrorOutOfDateKHR because if eErrorOutOfDateKHR, then #ImageIndex is undefined. */
    check(ImageIndex < this->Vk_SwapchainImageViews.size())

    if (this->bPendingResize)
    {
        this->PendingTimeForResizeApply -= static_cast<f32>(GEngine->DeltaTime);

        if (this->PendingTimeForResizeApply <= 0.0f)
        {
            LOG_VERBOSE(LogSurface,
                "Applying pending resize to surface [{}x{}].",
                this->PendingResizeExtent.x,
                this->PendingResizeExtent.y
                )

            LOG_VERBOSE(LogSurface, "Recreating swapchain due to suboptimal state and pending resize.")
            this->Vk_CurrentFrameInFlightIndex.reset();
            this->Vk_CreateSwapchain();
            check(this->bPendingResize == false)
            for (auto& Widget : this->GetViewport().GetTopLevelWidgets())
            {
                Widget->OnSurfaceResize();
            }
            return;
        }
    }
    else if (Result == vk::Result::eSuboptimalKHR)
    {
        LOG_VERBOSE(LogSurface, "Swapchain is suboptimal. Recreating swapchain.")
        this->Vk_CurrentFrameInFlightIndex.reset();
        this->Vk_CreateSwapchain();
        return;
    }

    if (Result != vk::Result::eSuccess &&
        Result != vk::Result::eSuboptimalKHR /* Not an error. Just ignore. */
        )
    {
        panicMsgf("Failed to acquire swapchain image. vk::Result: [{}].", vk::to_string(Result))
    }

    Frontend.Vk_GetDevice().resetFences(*this->Vk_FlightFences[*this->Vk_CurrentFrameInFlightIndex]);

    auto& CommandBuffer{ this->Vk_CommandBuffers[*this->Vk_CurrentFrameInFlightIndex] };
    CommandBuffer.reset();
    CommandBuffer.begin({});

    this->Vk_DescriptorPools[*this->Vk_CurrentFrameInFlightIndex].reset({});

    LRenderInfo Info{
        .UserPreferences = GetSingleton<JUserPreferences>(),
        .Frontend = Frontend,
        .Surface = *this,
        .TextureSubsystem = *Frontend.GetSubsystemChecked<JTextureSubsystem>(),
        .MaterialSubsystem = *Frontend.GetSubsystemChecked<JMaterialSubsystem>(),
        .ShaderSubsystem = *Frontend.GetSubsystemChecked<JShaderSubsystem>(),
        .MeshSubsystem = *this->GetMutableEngine().GetSubsystemChecked<JMeshSubsystem>(),
        .FontSubsystem = *Frontend.GetSubsystemChecked<JFontSubsystem>(),
        .CommandBuffer = *CommandBuffer,
        .DescriptorPool = *this->Vk_DescriptorPools[*this->Vk_CurrentFrameInFlightIndex],
        .Frame = *this->Vk_CurrentFrameInFlightIndex,
        .Image = ImageIndex,
        .VkViewport = {
            .width = static_cast<f32>(this->SurfaceExtent.width), .height = static_cast<f32>(this->SurfaceExtent.height),
            .minDepth = 0.0f, .maxDepth = 1.0f
            },
        .VkScissor = vk::Rect2D{
            .offset = {0, 0},
            .extent = this->SurfaceExtent,
            },
        };

    Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .srcAccessMask = {},
        .dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = this->Vk_SwapchainImages[Info.Image],
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
            },
        });

    Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .oldLayout = vk::ImageLayout::eUndefined, // this is wrong, more like: ePresentSrcKHR
        .newLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = *this->Vk_ColorImage,
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
            },
        });

    constexpr vk::ClearValue ClearColor(vk::ClearColorValue(std::array<f32,4>{0.0f, 0.0f, 0.0f, 1.0f}));
    vk::RenderingAttachmentInfo ColorAttachmentInfo{
        .imageView = this->Vk_ColorImageView,
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .resolveMode = vk::ResolveModeFlagBits::eAverage,
        .resolveImageView = this->Vk_SwapchainImageViews[Info.Image],
        .resolveImageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = ClearColor
        };

    vk::RenderingInfo RenderingInfo{
        .renderArea = {.offset={ 0, 0 }, .extent=this->SurfaceExtent},
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &ColorAttachmentInfo,
        .pDepthAttachment = nullptr,
        .pStencilAttachment = nullptr,
        };

    this->OnPreRender.Broadcast(Info);

    Info.CommandBuffer.beginRendering(RenderingInfo);
    Info.CommandBuffer.setViewport(0, Info.VkViewport);
    Info.CommandBuffer.setScissor(0, Info.VkScissor);
    this->GetViewport().Draw(Info);
    Info.CommandBuffer.endRendering();

    Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .dstStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe,
        .dstAccessMask = {},
        .oldLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .newLayout = vk::ImageLayout::ePresentSrcKHR,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = this->Vk_SwapchainImages[ImageIndex],
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
            },
        });

    (void)Info.CommandBuffer.end();

    vk::PipelineStageFlags DstStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput );
    Info.Frontend.Vk_GetGraphicsQueue().submit(vk::SubmitInfo{
        .waitSemaphoreCount = 1, .pWaitSemaphores = &*this->Vk_ImageAvailableSemaphores[Info.Frame],
        .pWaitDstStageMask = &DstStageMask,
        .commandBufferCount = 1, .pCommandBuffers = &Info.CommandBuffer,
        .signalSemaphoreCount = 1, .pSignalSemaphores = &*this->Vk_RenderSemaphores[ImageIndex]
        }, *this->Vk_FlightFences[Info.Frame]);

    Result = Info.Frontend.Vk_GetGraphicsQueue().presentKHR({
        .waitSemaphoreCount = 1, .pWaitSemaphores = &*this->Vk_RenderSemaphores[ImageIndex],
        .swapchainCount = 1, .pSwapchains = &*this->Vk_VkMySwapchain,
        .pImageIndices = &ImageIndex
        });

    this->Vk_LastFrameInFlightIndex = Info.Frame;
    this->Vk_CurrentFrameInFlightIndex.reset();

    switch (Result)
    {
    case vk::Result::eSuccess:
    {
        break;
    }
    case vk::Result::eSuboptimalKHR:
    case vk::Result::eErrorOutOfDateKHR:
    {
        LOG_VERBOSE(LogVulkan, "The swapchain is no longer optimal for the surface. Consider recreating the swapchain.")
        break;
    }
    default:
    {
        LOG_WARNING(LogVulkan, "vk::Queue::presentKHR returned unexpected vk::Result [{}].", vk::to_string(Result))
        break;
    }
    }
}

void Jafg::LSurfaceGlfw3::SetInputMode(EInputMode InMode) noexcept
{
    check(this->Handle)
    check(Tasks::IsOnMasterThread())

    if (this->InputMode == InMode)
    {
        return;
    }

    this->InputMode = InMode;

    if (this->IsShowMouseCursor())
    {
        glfwSetInputMode(this->Handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        /*
         * This seems very wrong.
         * Why do we have to actively query the cursor location and can't wait for the next pooling event?? Well,
         * because the event is only fired when the location was changed. This seems like a bug on Wayland maybe...
         * In the meantime the user would have the location of the disabled cursor which is useless and breaks a lot
         * of UI / FP-Rotation logic.
         */
        if (this->MouseLocation)
        {
            LVec2D Query;
            glfwGetCursorPos(this->Handle, &Query.x, &Query.y);
            this->MouseLocation = LVec2F{static_cast<f32>(Query.x), static_cast<f32>(Query.y)};
        }
    }
    else
    {
        glfwSetInputMode(this->Handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void Jafg::LSurfaceGlfw3::_SetMouseCursor(ECursor Cursor)
{
    check(Tasks::IsOnMasterThread())
    check(this->Handle)

    if (LexToString(Cursor) == "Default")
    {
        JAFG_PLATFORM_NO_DISCARD_CTRL_PATH
    }

    LOG_TRACE(LogPlatform, "Setting mouse cursor to [{}].", LexToString(Cursor))

    if (this->Cursor)
    {
        glfwDestroyCursor(this->Cursor);
        this->Cursor = nullptr;
    }

    if (Cursor == ECursor::Default)
    {
        check(this->Cursor == nullptr)
    }
    else if (Cursor == ECursor::Arrow)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    }
    else if (Cursor == ECursor::Beam)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    }
    else if (Cursor == ECursor::Crosshair)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    }
    else if (Cursor == ECursor::Hand)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    }
    else if (Cursor == ECursor::ResizeNS)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);
    }
    else if (Cursor == ECursor::ResizeEW)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);
    }
    else if (Cursor == ECursor::ResizeNESW)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
    }
    else if (Cursor == ECursor::ResizeNWSE)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    }
    else if (Cursor == ECursor::ResizeOmni)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
    }
    else if (Cursor == ECursor::NotAllowed)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
    }
    else
    {
        LOG_WARNING(LogSystem, "Unknown cursor type {}[{}].", static_cast<i32>(Cursor), LexToString(Cursor))
    }

    if (this->Cursor)
    {
        glfwSetCursor(this->Handle, this->Cursor);
    }
}

void Jafg::LSurfaceGlfw3::SetResizable(const bool bResizable)
{
    check(Tasks::IsOnMasterThread())
    check(this->Handle)

    if (this->IsResizable() == bResizable)
    {
        return;
    }

    this->bResizable = bResizable;
    check(this->IsResizable() == bResizable)

    LOG_VERBOSE(LogSurface, "Setting window resizeability to [{}].", this->IsResizable() ? "true" : "false")
    glfwSetWindowAttrib(this->Handle, GLFW_RESIZABLE, this->IsResizable() ? GLFW_TRUE : GLFW_FALSE);
}

void Jafg::LSurfaceGlfw3::SetWindowSize(LVec2u32 Size)
{
    check(Tasks::IsOnMasterThread())
    check(this->Handle)

    LOG_VERBOSE(LogSurface, "Setting window size to [{}x{}].", Size.x, Size.y)
    glfwSetWindowSize(this->Handle, static_cast<i32>(Size.x), static_cast<i32>(Size.y));
}

bool Jafg::LSurfaceGlfw3::CanBorderless() const noexcept
{
    auto& Frontend{this->GetFrontend()};
    return Frontend.Vk_GetFramework() == rhi::framework::x11
        || Frontend.Vk_GetFramework() == rhi::framework::Cocoa
        || Frontend.Vk_GetFramework() == rhi::framework::Win32;
}

bool Jafg::LSurfaceGlfw3::IsBorderless() const noexcept
{
    // TODO: Implement. Currently we only support wayland and xwayland, which do not support this, therefore, this is not implemented.
    std::unreachable();
}

void Jafg::LSurfaceGlfw3::SetBorderless(bool bBorderless)
{
    std::unreachable();
}

bool Jafg::LSurfaceGlfw3::IsFullscreen() const noexcept
{
    check(Tasks::IsOnMasterThread())
    check(this->Handle)

    return glfwGetWindowMonitor(this->Handle) != nullptr;
}

void Jafg::LSurfaceGlfw3::SetFullscreen(bool bFullscreen)
{
    check(Tasks::IsOnMasterThread())
    check(this->Handle)

    if (bFullscreen)
    {
        auto const& Viewport{this->GetPreferredPhysicalViewport()};
        auto const& VideoMode{this->GetPreferredVideoMode(Viewport)};
        this->FallbackExtent = this->SurfaceExtent;
        LOG_VERBOSE(LogSurface, "[{}]: Entering fullscreen mode on viewport [{}-{}] with {}."
            , this->GetHumanReadableName(), Viewport.Index, Viewport.Name, VideoMode.ToHumanReadableString())
        glfwSetWindowMonitor(
            this->Handle,
            static_cast<GLFWmonitor*>(Viewport.Handle),
            0, 0, VideoMode.ResolutionPx.x, VideoMode.ResolutionPx.y,
            VideoMode.RefreshRateHz
            );
    }
    else
    {
        if (!this->FallbackExtent)
        {
            LOG_FATAL(LogSurface, "Fallback extent is not set. Cannot exit fullscreen mode.")
        }
        glfwSetWindowMonitor(
            this->Handle,
            nullptr,
            0, 0, this->FallbackExtent->width, this->FallbackExtent->height,
            GLFW_DONT_CARE
            );
    }
}

void Jafg::LSurfaceGlfw3::Vk_TransitionImageLayout(vk::ImageMemoryBarrier2 const& Barrier)
{
    check(this->Vk_CurrentFrameInFlightIndex.has_value())

    vk::DependencyInfo DependencyInfo{
        .dependencyFlags = {},
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &Barrier
        };

    this->Vk_CommandBuffers[*this->Vk_CurrentFrameInFlightIndex].pipelineBarrier2(DependencyInfo);
}

//# We have to use this instead of a function because of sso.
#ifdef DETAIL_JAFG_CAST_TO_NFDn
    #error "Expected DETAIL_JAFG_CAST_TO_NFDn to be undefined."
#endif /* DETAIL_JAFG_CAST_TO_NFDn */
#define DETAIL_JAFG_CAST_TO_NFDn(JafgFilters) \
    TArray<nfdnfilteritem_t> Filters; JafgFilters.reserve(Filters.size()); \
    TArray<LNativeString> RaiiFilterDummy; RaiiFilterDummy.reserve(JafgFilters.size()*2); \
    for (auto const& Filter: JafgFilters) \
    { \
        auto& DisplayNameRef{RaiiFilterDummy.emplace_back()}; \
        auto& SpecRef{RaiiFilterDummy.emplace_back()}; \
        DisplayNameRef.append(algo::utf8_to_native(LString{Filter.DisplayName})); \
        SpecRef.append(algo::utf8_to_native(LString{Filter.Specs})); \
        Filters.push_back({ \
            .name = DisplayNameRef.c_str(), \
            .spec = SpecRef.c_str(), \
            }); \
        continue; \
    }

std::optional<LPath> Jafg::LSurfaceGlfw3::OpenBlockingDialogForFile(LFileDialogInfo Info, LPath const& Default /* = Finder::GetCwd() */)
{
    LOG_VERBOSE(LogSurface, "Opening directory dialog in [{}].", Default.empty() ? "<auto>" : Default)

    /* Value initialize in case GetNativeWindow fails; Which is okay. */
    nfdwindowhandle_t Parent{};
    check(!Parent.handle)
    (void)NFD_GetNativeWindowFromGLFWWindow(this->Handle, &Parent);

    DETAIL_JAFG_CAST_TO_NFDn(Info.Filters)

    NFD::UniquePathN OutPath;
    if (auto Result{NFD::OpenDialog(OutPath
        , Filters.empty() ? nullptr : Filters.data(), static_cast<nfdfiltersize_t>(Filters.size())
        , Default.c_str()
        , Parent
        )}; Result == NFD_ERROR)
    {
        if (auto* Error{NFD_GetError()}; Error)
        {
            LOG_FATAL(LogSurface, "Error opening directory dialog: {}.", Error)
        }
        LOG_FATAL(LogSurface, "Error opening directory dialog: Unknown error.")
    }
    else if (Result == NFD_CANCEL)
    {
        return {};
    }

    check(OutPath.get())
    return OutPath.get();
}

std::optional<TArray<LPath>> Jafg::LSurfaceGlfw3::OpenBlockingDialogForFiles(LFileDialogInfo Info, LPath const& Default)
{
    LOG_VERBOSE(LogSurface, "Opening directory dialog in [{}].", Default.empty() ? "<auto>" : Default)

    /* Value initialize in case GetNativeWindow fails; Which is okay. */
    nfdwindowhandle_t Parent{};
    check(!Parent.handle)
    (void)NFD_GetNativeWindowFromGLFWWindow(this->Handle, &Parent);

    DETAIL_JAFG_CAST_TO_NFDn(Info.Filters)

    NFD::UniquePathSet OutPaths;
    if (auto Result{NFD::OpenDialogMultiple(OutPaths
        , Filters.empty() ? nullptr : Filters.data(), static_cast<nfdfiltersize_t>(Filters.size())
        , Default.c_str()
        , Parent
        )}; Result == NFD_ERROR)
    {
        if (auto* Error{NFD_GetError()}; Error)
        {
            LOG_FATAL(LogSurface, "Error opening directory dialog: {}.", Error)
        }
        LOG_FATAL(LogSurface, "Error opening directory dialog: Unknown error.")
    }
    else if (Result == NFD_CANCEL)
    {
        return {};
    }

    nfdpathsetsize_t N;
    if (auto Result{NFD::PathSet::Count(OutPaths, N)}; Result != NFD_OKAY)
    {
        if (auto* Error{NFD_GetError()}; Error)
        {
            LOG_FATAL(LogSurface, "Error counting path set: {}.", Error)
        }
        LOG_FATAL(LogSurface, "Error counting path set: Unknown error.")
    }

    TArray<LPath> Result; Result.reserve(N);
    for (auto Idx{0uz}; Idx < N; ++Idx)
    {
        NFD::UniquePathSetPathN Path;
        if (auto Result{NFD::PathSet::GetPath(OutPaths, static_cast<nfdpathsetsize_t>(Idx), Path)}; Result != NFD_OKAY)
        {
            if (auto* Error{NFD_GetError()}; Error)
            {
                LOG_FATAL(LogSurface, "Error getting path from path set: {}.", Error)
            }
            LOG_FATAL(LogSurface, "Error getting path from path set: Unknown error.")
        }
        check(Path.get())
        Result.emplace_back(&*Path);
    }

    return Result;
}

std::optional<LPath> Jafg::LSurfaceGlfw3::OpenBlockingDialogForFileToSave(LSaveFileDialogInfo Info)
{
    LOG_VERBOSE(LogSurface, "Opening save dialog in [{}].", Info.Path/Info.Name)

    /* Value initialize in case GetNativeWindow fails; Which is okay. */
    nfdwindowhandle_t Parent{};
    check(!Parent.handle)
    (void)NFD_GetNativeWindowFromGLFWWindow(this->Handle, &Parent);

    DETAIL_JAFG_CAST_TO_NFDn(Info.Filters)

    NFD::UniquePathN OutPath;
    if (auto Result{NFD::SaveDialog(OutPath
        , Filters.empty() ? nullptr : Filters.data(), static_cast<nfdfiltersize_t>(Filters.size())
        , Info.Path.c_str(), Info.Name.c_str()
        , Parent
        )}; Result == NFD_ERROR)
    {
        if (auto* Error{NFD_GetError()}; Error)
        {
            LOG_FATAL(LogSurface, "Error opening save directory dialog: {}.", Error)
        }
        LOG_FATAL(LogSurface, "Error opening save directory dialog: Unknown error.")
    }
    else if (Result == NFD_CANCEL)
    {
        return {};
    }

    check(OutPath.get())
    return OutPath.get();
}

#undef DETAIL_JAFG_CAST_TO_NFDn

std::optional<LPath> Jafg::LSurfaceGlfw3::OpenBlockingDialogForDirectory(LPath Default)
{
    LOG_VERBOSE(LogSurface, "Opening directory dialog in [{}].", Default.empty() ? "<auto>" : Default)

    /* Value initialize in case GetNativeWindow fails; Which is okay. */
    nfdwindowhandle_t Parent{};
    check(!Parent.handle)
    (void)NFD_GetNativeWindowFromGLFWWindow(this->Handle, &Parent);

    NFD::UniquePathN OutPath;
    if (auto Result{NFD::PickFolder(OutPath
        , Default.c_str()
        , Parent
        )}; Result == NFD_ERROR)
    {
        if (auto* Error{NFD_GetError()}; Error)
        {
            LOG_FATAL(LogSurface, "Error opening directory dialog: {}.", Error)
        }
        LOG_FATAL(LogSurface, "Error opening directory dialog: Unknown error.")
    }
    else if (Result == NFD_CANCEL)
    {
        return {};
    }

    check(OutPath.get())
    return OutPath.get();
}

std::optional<TArray<LPath>> Jafg::LSurfaceGlfw3::OpenBlockingDialogForDirectories(LPath Default)
{
    LOG_VERBOSE(LogSurface, "Opening save directory dialog in [{}].", Default.empty() ? "<auto>" : Default)

    /* Value initialize in case GetNativeWindow fails; Which is okay. */
    nfdwindowhandle_t Parent{};
    check(!Parent.handle)
    (void)NFD_GetNativeWindowFromGLFWWindow(this->Handle, &Parent);

    NFD::UniquePathSet OutPaths;
    if (auto Result{NFD::PickFolderMultiple(OutPaths
        , Default.c_str()
        , Parent
        )}; Result == NFD_ERROR)
    {
        if (auto* Error{NFD_GetError()}; Error)
        {
            LOG_FATAL(LogSurface, "Error opening save directory dialog: {}.", Error)
        }
        LOG_FATAL(LogSurface, "Error opening save directory dialog: Unknown error.")
    }
    else if (Result == NFD_CANCEL)
    {
        return {};
    }

    nfdpathsetsize_t N;
    if (auto Result{NFD::PathSet::Count(OutPaths, N)}; Result != NFD_OKAY)
    {
        if (auto* Error{NFD_GetError()}; Error)
        {
            LOG_FATAL(LogSurface, "Error counting path set: {}.", Error)
        }
        LOG_FATAL(LogSurface, "Error counting path set: Unknown error.")
    }

    TArray<LPath> Result; Result.reserve(N);
    for (auto Idx{0uz}; Idx < N; ++Idx)
    {
        NFD::UniquePathSetPathN Path;
        if (auto Result{NFD::PathSet::GetPath(OutPaths, static_cast<nfdpathsetsize_t>(Idx), Path)}; Result != NFD_OKAY)
        {
            if (auto* Error{NFD_GetError()}; Error)
            {
                LOG_FATAL(LogSurface, "Error getting path from path set: {}.", Error)
            }
            LOG_FATAL(LogSurface, "Error getting path from path set: Unknown error.")
        }
        check(Path.get())
        Result.emplace_back(&*Path);
    }

    return Result;
}

void Jafg::LSurfaceGlfw3::FramebufferSizeCallback(const i32 Width, const i32 Height)
{
    this->bPendingResize = true;
    this->PendingResizeExtent = {static_cast<u32>(Width), static_cast<u32>(Height)};

    /*
     * One-point-five-second delay when making changes to the viewport. We might want to change this later depending
     * on some platform parameters.
     */
    // this->PendingTimeForResizeApply = 1.5f;
    this->PendingTimeForResizeApply = 0.2f;
}

void Jafg::LSurfaceGlfw3::MouseCallback(f64 XPos, f64 YPos)
{
    if (this->bMouseInsideSurface)
    {
        this->MouseLocation = {XPos,YPos};
    }
    else
    {
        //#
        //# So some platforms allow this. But not all. To preserve consistency across all platforms,
        //# We discard this input.
        //#
        this->MouseLocation.reset();
    }
}

void Jafg::LSurfaceGlfw3::ScrollCallback(double XOffset, double YOffset)
{
    if (XOffset < 0.0f)
    {
        auto It{algo::find(this->GetMutableRawInputsDangerous(), LPhysicalKey::FromLogical(ELogicalKey::MouseWheelLeft), &LRawInput::PhysicalKey)};
        if (It != this->GetRawInputs().end())
        {
            /* Mouse wheel inputs get decayed at the beginning of each tick, therefore, this found input is from the same tick, and we can add it. */
            check(It->State == ERawInputStateBits::Press)
            It->Value += static_cast<f32>(XOffset);
        }
        else
        {
            this->UpdateKeyState({
                .PhysicalKey = LPhysicalKey::FromLogical(ELogicalKey::MouseWheelLeft),
                .Value = static_cast<f32>(XOffset),
                .State = ERawInputStateBits::Press,
                });
        }
    }

    if (YOffset > 0.0f)
    {
        auto It{algo::find(this->GetMutableRawInputsDangerous(), LPhysicalKey::FromLogical(ELogicalKey::MouseWheelUp), &LRawInput::PhysicalKey)};
        if (It != this->GetRawInputs().end())
        {
            /* Mouse wheel inputs get decayed at the beginning of each tick, therefore, this found input is from the same tick, and we can add it. */
            check(It->State == ERawInputStateBits::Press)
            It->Value += static_cast<f32>(YOffset);
        }
        else
        {
            this->UpdateKeyState({
                .PhysicalKey = LPhysicalKey::FromLogical(ELogicalKey::MouseWheelUp),
                .Value = static_cast<f32>(YOffset),
                .State = ERawInputStateBits::Press,
                });
        }
    }

    if (XOffset > 0.0f)
    {
        auto It{algo::find(this->GetMutableRawInputsDangerous(), LPhysicalKey::FromLogical(ELogicalKey::MouseWheelRight), &LRawInput::PhysicalKey)};
        if (It != this->GetRawInputs().end())
        {
            /* Mouse wheel inputs get decayed at the beginning of each tick, therefore, this found input is from the same tick, and we can add it. */
            check(It->State == ERawInputStateBits::Press)
            It->Value += static_cast<f32>(XOffset);
        }
        else
        {
            this->UpdateKeyState({
                .PhysicalKey = LPhysicalKey::FromLogical(ELogicalKey::MouseWheelRight),
                .Value = static_cast<f32>(XOffset),
                .State = ERawInputStateBits::Press,
                });
        }
    }

    if (YOffset < 0.0f)
    {
        auto It{algo::find(this->GetMutableRawInputsDangerous(), LPhysicalKey::FromLogical(ELogicalKey::MouseWheelDown), &LRawInput::PhysicalKey)};
        if (It != this->GetRawInputs().end())
        {
            /* Mouse wheel inputs get decayed at the beginning of each tick, therefore, this found input is from the same tick, and we can add it. */
            check(It->State == ERawInputStateBits::Press)
            It->Value += static_cast<f32>(YOffset);
        }
        else
        {
            this->UpdateKeyState({
                .PhysicalKey = LPhysicalKey::FromLogical(ELogicalKey::MouseWheelDown),
                .Value = static_cast<f32>(YOffset),
                .State = ERawInputStateBits::Press,
                });
        }
    }
}

void Jafg::LSurfaceGlfw3::MouseEnterCallback(i32 Entered)
{
    this->MouseLocation.reset();

    if (Entered == GLFW_TRUE)
    {
        this->bMouseInsideSurface = true;
    }
    else
    {
        this->bMouseInsideSurface = false;
    }
}

void Jafg::LSurfaceGlfw3::CharCallback(const u32 Codepoint)
{
    this->AddBufferedPlatformInput(::Glfw3CodePoint2Utf8(Codepoint));
}

void Jafg::LSurfaceGlfw3::KeyCallback(i32 Key, i32 Scancode, i32 Action, i32 Mods)
{
    ERawInputStateFlags State{ ERawInputStateBits::Identity };
    if (Action == GLFW_PRESS) { State = ERawInputStateBits::Press|ERawInputStateBits::Hold; }
    else if (Action == GLFW_REPEAT) { State = ERawInputStateBits::Repeat; }
    else if (Action == GLFW_RELEASE) { State = ERawInputStateBits::Release; }
    else { std::unreachable(); }

    EModFlags ModFlags;
    if (Mods & GLFW_MOD_SHIFT)     { ModFlags |= EModBits::Shift; }
    if (Mods & GLFW_MOD_CONTROL)   { ModFlags |= EModBits::Control; }
    if (Mods & GLFW_MOD_ALT)       { ModFlags |= EModBits::Alt; }
    if (Mods & GLFW_MOD_SUPER)     { ModFlags |= EModBits::Super; }
    if (Mods & GLFW_MOD_CAPS_LOCK) { ModFlags |= EModBits::CapsLock; }
    if (Mods & GLFW_MOD_NUM_LOCK)  { ModFlags |= EModBits::NumLock; }

    this->UpdateKeyState({
        .PhysicalKey = {.Scancode = Scancode,},
        .Mods = ModFlags,
        .State = State,
        });
}

void Jafg::LSurfaceGlfw3::MouseButtonCallback(i32 Button, i32 Action, i32 Mods)
{
    ERawInputStateBits State{ ERawInputStateBits::Identity };
    if (Action == GLFW_PRESS) { State = ERawInputStateBits::Press; }
    else if (Action == GLFW_REPEAT) { State = ERawInputStateBits::Repeat; }
    else if (Action == GLFW_RELEASE) { State = ERawInputStateBits::Release; }
    else { std::unreachable(); }

    EModFlags ModFlags;
    if (Mods & GLFW_MOD_SHIFT)     { ModFlags |= EModBits::Shift; }
    if (Mods & GLFW_MOD_CONTROL)   { ModFlags |= EModBits::Control; }
    if (Mods & GLFW_MOD_ALT)       { ModFlags |= EModBits::Alt; }
    if (Mods & GLFW_MOD_SUPER)     { ModFlags |= EModBits::Super; }
    if (Mods & GLFW_MOD_CAPS_LOCK) { ModFlags |= EModBits::CapsLock; }
    if (Mods & GLFW_MOD_NUM_LOCK)  { ModFlags |= EModBits::NumLock; }

    if (Button == GLFW_MOUSE_BUTTON_LEFT)
    {
        Button = static_cast<i32>(ELogicalKey::LeftMouseButton);
    }
    else if (Button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        Button = static_cast<i32>(ELogicalKey::RightMouseButton);
    }
    else if (Button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        Button = static_cast<i32>(ELogicalKey::MiddleMouseButton);
    }
    else if (Button == GLFW_MOUSE_BUTTON_1)
    {
        Button = static_cast<i32>(ELogicalKey::ThumbMouseButton1);
    }
    else if (Button == GLFW_MOUSE_BUTTON_2)
    {
        Button = static_cast<i32>(ELogicalKey::ThumbMouseButton2);
    }
    else if (Button == GLFW_MOUSE_BUTTON_3)
    {
        Button = static_cast<i32>(ELogicalKey::ThumbMouseButton3);
    }
    else if (Button == GLFW_MOUSE_BUTTON_4)
    {
        Button = static_cast<i32>(ELogicalKey::ThumbMouseButton4);
    }
    else if (Button == GLFW_MOUSE_BUTTON_5)
    {
        Button = static_cast<i32>(ELogicalKey::ThumbMouseButton5);
    }
    else if (Button == GLFW_MOUSE_BUTTON_6)
    {
        Button = static_cast<i32>(ELogicalKey::ThumbMouseButton6);
    }
    else if (Button == GLFW_MOUSE_BUTTON_7)
    {
        Button = static_cast<i32>(ELogicalKey::ThumbMouseButton7);
    }
    else if (Button == GLFW_MOUSE_BUTTON_8)
    {
        Button = static_cast<i32>(ELogicalKey::ThumbMouseButton8);
    }
    else
    {
        LOG_FATAL(LogPlatform, "Unknown code [{}].", Button)
    }

    this->UpdateKeyState({
        .PhysicalKey = {.Logical = Button,},
        .Mods = ModFlags,
        .State = State,
        });
}

Jafg::LPhysicalViewport const& Jafg::LSurfaceGlfw3::GetPreferredPhysicalViewport()
{
    auto& Prefs{GetSingleton<JUserPreferences>()};
    auto& Viewports{this->GetFrontend().GetPhysicalViewports()};

    if (!Prefs.PreferredMonitor.Value.empty())
    {
        auto It{algo::find(Viewports, *Prefs.PreferredMonitor, &LPhysicalViewport::Name)};
        if (It != Viewports.end())
        {
            LOG_VERBOSE(LogSurface, "Using preferred monitor [{}-{}].", It->Index, It->Name)
            return *It;
        }
        LOG_WARNING(LogSurface, "Preferred monitor [{}] not found. Falling back to primary monitor.", *Prefs.PreferredMonitor)
    }

    GLFWmonitor* Monitor{glfwGetPrimaryMonitor()};
    check(Monitor)
    auto It{algo::find(Viewports, Monitor, &LPhysicalViewport::Handle)};
    if (It != Viewports.end())
    {
        LOG_VERBOSE(LogSurface, "Using primary monitor [{}-{}].", It->Index, It->Name)
        return *It;
    }

    LOG_FATAL(LogSurface, "Primary monitor not found in physical viewports.")
}

Jafg::LPhysicalViewport::VideoMode Jafg::LSurfaceGlfw3::GetPreferredVideoMode(LPhysicalViewport const& Viewport)
{
    check(!Viewport.VideoModes.empty())

    auto& Prefs{GetSingleton<JUserPreferences>()};
    TArray<LPhysicalViewport::VideoMode> Candidates{Viewport.VideoModes};

    if (Prefs.PreferredBpp.Value != maths::zero_vector<LVec3i32>)
    {
        TArray<LPhysicalViewport::VideoMode> Previous{Candidates};
        algo::erase_if(&Candidates, [&Prefs](LPhysicalViewport::VideoMode const& VideoMode)
        {
            return VideoMode.Bits != Prefs.PreferredBpp.Value;
        });
        if (Candidates.empty())
        {
            LOG_WARNING(LogSurface, "No video mode found with preferred bpp [{}]. Ignoring preferred bpp."
                , maths::to_string(Prefs.PreferredBpp.Value))
            Candidates = std::move(Previous);
        }
    }

    if (Prefs.PreferredResolutionPx.Value != maths::zero_vector<LVec2i32>)
    {
        TArray<LPhysicalViewport::VideoMode> Previous{Candidates};
        algo::erase_if(&Candidates, [&Prefs](LPhysicalViewport::VideoMode const& VideoMode)
        {
            return VideoMode.ResolutionPx != Prefs.PreferredResolutionPx.Value;
        });
        if (Candidates.empty())
        {
            LOG_WARNING(LogSurface, "No video mode found with preferred resolution [{}]. Ignoring preferred resolution."
                , maths::to_string(Prefs.PreferredResolutionPx.Value))
            Candidates = std::move(Previous);
        }
    }

    if (Prefs.PreferredRefreshRateHz.Value != 0)
    {
        TArray<LPhysicalViewport::VideoMode> Previous{Candidates};
        algo::erase_if(&Candidates, [&Prefs](LPhysicalViewport::VideoMode const& VideoMode)
        {
            return VideoMode.RefreshRateHz != Prefs.PreferredRefreshRateHz.Value;
        });
        if (Candidates.empty())
        {
            LOG_WARNING(LogSurface, "No video mode found with preferred refresh rate [{}]. Ignoring preferred refresh rate."
                , Prefs.PreferredRefreshRateHz.Value)
            Candidates = std::move(Previous);
        }
    }

    if (Candidates.empty())
    {
        LOG_WARNING(LogSurface, "No video modes found with the specified preferences. Falling back to default video mode.")
        return Viewport.VideoModes.front();
    }

    if (Candidates.size() > 1)
    {
        algo::sort(Candidates, [](LPhysicalViewport::VideoMode const& A, LPhysicalViewport::VideoMode const& B)
        {
            if (A.RefreshRateHz != B.RefreshRateHz)
            {
                return A.RefreshRateHz > B.RefreshRateHz;
            }
            if (A.ResolutionPx != B.ResolutionPx)
            {
                return A.ResolutionPx.x * A.ResolutionPx.y > B.ResolutionPx.x * B.ResolutionPx.y;
            }
            return A.Bits.x * A.Bits.y * A.Bits.z > B.Bits.x * B.Bits.y * B.Bits.z;
        });

        LOG_WARNING(LogSurface, "Multiple video modes found with the specified preferences. Comparing with hz > resolution > bpp for selection.")
    }

    return Candidates.front();
}

void Jafg::LSurfaceGlfw3::Vk_CreateCommandPool()
{
    LOG_VERBOSE(LogVulkan, "Creating command pool for surface.")
    this->Vk_CommandPool = rhi::vk_build(this->GetFrontend().Vk_GetDevice(), vk::CommandPoolCreateInfo{
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = this->GetFrontend().Vk_GetGraphicsQueueFamilyIndex()
        });
}

void Jafg::LSurfaceGlfw3::Vk_CreateSwapchain()
{
    STAT_CYCLE_FUNCTION()

    LOG_VERBOSE(LogVulkan, "Creating Vulkan swapchain for Glfw3 surface.")

    auto& Frontend{this->GetMutableFrontend()};
    auto& Prefs{GetSingleton<JUserPreferences>()};

    {
        LVec2i32 _FramebufferSize;
        glfwGetFramebufferSize(this->Handle, &_FramebufferSize.x, &_FramebufferSize.y);
        if (_FramebufferSize.x == 0 || _FramebufferSize.y == 0)
        {
            LOG_VERBOSE(LogSurface, "Waiting for non-zero dimensions to recreate swapchain.")
            STAT_QUICK_CYCLE_START("GlfwWaitEventsForNonZeroFramebufferSize")
            while (_FramebufferSize.x == 0 || _FramebufferSize.y == 0)
            {
                glfwWaitEvents();
                glfwGetFramebufferSize(this->Handle, &_FramebufferSize.x, &_FramebufferSize.y);
                continue;
            }
        }
    }

    Frontend._Vk_WaitIdle();

    this->Vk_SwapchainImageViews.clear();
    this->Vk_SwapchainImages.clear();
    this->Vk_VkMySwapchain = nullptr;
    for (auto& PresentSemaphore : this->Vk_ImageAvailableSemaphores) { PresentSemaphore = nullptr; }
    for (auto& RenderSemaphore : this->Vk_RenderSemaphores) { RenderSemaphore = nullptr; }
    for (auto& FlightFence : this->Vk_FlightFences) { FlightFence = nullptr; }

    auto Result{Frontend.Vk_GetPhysicalDevice().getSurfaceCapabilitiesKHR(this->Vk_Surface)};
    check(Result.has_value())
    this->Vk_SurfaceCapabilities = std::move(*Result);
    LOG_VERBOSE(LogVulkan, "Surface capabilities: "
                           "minImageCount [{}], maxImageCount [{}], currentExtent [{}x{}], "
                           "minImageExtent [{}x{}], maxImageExtent [{}x{}], maxImageArrayLayers [{}], "
                           "supportedTransforms [{}], currentTransform [{}], supportedCompositeAlpha [{}], "
                           "supportedUsageFlags [{}]",
        this->Vk_SurfaceCapabilities.minImageCount,
        this->Vk_SurfaceCapabilities.maxImageCount,
        this->Vk_SurfaceCapabilities.currentExtent.width,
        this->Vk_SurfaceCapabilities.currentExtent.height,
        this->Vk_SurfaceCapabilities.minImageExtent.width,
        this->Vk_SurfaceCapabilities.minImageExtent.height,
        this->Vk_SurfaceCapabilities.maxImageExtent.width,
        this->Vk_SurfaceCapabilities.maxImageExtent.height,
        this->Vk_SurfaceCapabilities.maxImageArrayLayers,
        vk::to_string(this->Vk_SurfaceCapabilities.supportedTransforms),
        vk::to_string(this->Vk_SurfaceCapabilities.currentTransform),
        vk::to_string(this->Vk_SurfaceCapabilities.supportedCompositeAlpha),
        vk::to_string(this->Vk_SurfaceCapabilities.supportedUsageFlags)
        )

    auto Result2{Frontend.Vk_GetPhysicalDevice().getSurfaceFormatsKHR(this->Vk_Surface)};
    check(Result2.has_value())
    this->Vk_AvailableSurfaceFormats = std::move(*Result2);
    LOG_VERBOSE(LogVulkan, "Available surface formats:")
    if constexpr (IS_COMPILED_LOG(LogVulkan, Verbose)) for (auto const& SurfaceFormat : this->Vk_AvailableSurfaceFormats)
    {
        LOG_VERBOSE(LogVulkan, "    Format [{}], Color Space [{}]",
            vk::to_string(SurfaceFormat.format),
            vk::to_string(SurfaceFormat.colorSpace)
            )
    }

    auto Result3{Frontend.Vk_GetPhysicalDevice().getSurfacePresentModesKHR(this->Vk_Surface)};
    this->Vk_AvailablePresentModes = std::move(*Result3);
    LOG_VERBOSE(LogVulkan, "Available present modes:")
    if constexpr (IS_COMPILED_LOG(LogVulkan, Verbose)) for (auto const& PresentMode: this->Vk_AvailablePresentModes)
    {
        LOG_VERBOSE(LogVulkan, "    Present Mode [{}]", vk::to_string(PresentMode))
    }

    if (auto AvailableFormat{this->Vk_GetSwapchainSurfaceFormatKHR(this->Vk_DesiredSurfaceFormat)})
    {
        Frontend.Vk_SetSurfaceFormat(*AvailableFormat);
    }
    else
    {
        /* TODO: Is this even a fatal error?? Should we just use a non-optimal format then?! */
        panicMsgf(
            "Desired surface format [format={}, colorSpace={}] is not available on the current platform.",
            vk::to_string(this->Vk_DesiredSurfaceFormat.format),
            vk::to_string(this->Vk_DesiredSurfaceFormat.colorSpace)
            )
    }

    if (auto AvailablePresentMode{this->Vk_GetSwapchainPresentModeKHR(rhi::vk_to_khr_present_mode(*Prefs.DesiredPresentMode))}; !AvailablePresentMode)
    {
        // We may always use FIFO as the std guarantees its existence.
        // https://docs.vulkan.org/refpages/latest/refpages/source/VkPresentModeKHR.html
        LOG_WARNING(LogVulkan,
            "Desired present mode [{}] is not available on the current platform. Falling back to eFifo.",
            serde::to_string(*Prefs.DesiredPresentMode)
            )
        this->Vk_PresentMode = vk::PresentModeKHR::eFifo;
    }
    else
    {
        this->Vk_PresentMode = AvailablePresentMode.value();
    }

    {
        glfwGetWindowFrameSize(this->Handle
            , &this->WindowFrameSizeTopLeft.x, &this->WindowFrameSizeTopLeft.y
            , &this->WindowFrameSizeBottomRight.x, &this->WindowFrameSizeBottomRight.y);
        glfwGetWindowSize(this->Handle, &this->WindowSize.x, &this->WindowSize.y);
        glfwGetFramebufferSize(this->Handle, &this->FramebufferSize.x, &this->FramebufferSize.y);
        if (this->Vk_SurfaceCapabilities.currentExtent.width == std::numeric_limits<decltype(this->Vk_SurfaceCapabilities.currentExtent.width)>::max())
        {
            this->SurfaceExtent = {
                maths::clamp<decltype(vk::Extent2D::width)>(
                      this->FramebufferSize.x
                    , this->Vk_SurfaceCapabilities.minImageExtent.width, this->Vk_SurfaceCapabilities.maxImageExtent.width),
                maths::clamp<decltype(vk::Extent2D::height)>(
                      this->FramebufferSize.y
                    , this->Vk_SurfaceCapabilities.minImageExtent.height, this->Vk_SurfaceCapabilities.maxImageExtent.height)
                };
        }
        else
        {
            this->SurfaceExtent = rhi::extent2::from_native(this->Vk_SurfaceCapabilities.currentExtent);
            check( this->SurfaceExtent.width >= this->Vk_SurfaceCapabilities.minImageExtent.width
                && this->SurfaceExtent.width <= this->Vk_SurfaceCapabilities.maxImageExtent.width )
            check( this->SurfaceExtent.height >= this->Vk_SurfaceCapabilities.minImageExtent.height
                && this->SurfaceExtent.height <= this->Vk_SurfaceCapabilities.maxImageExtent.height )
        }
    }

    LOG_VERBOSE(LogVulkan, "Using surface format [format={}, colorSpace={}].",
        vk::to_string(Frontend.Vk_GetSurfaceFormat().format), vk::to_string(Frontend.Vk_GetSurfaceFormat().colorSpace)
        )
    LOG_VERBOSE(LogVulkan, "Using present mode [{}].", vk::to_string(this->Vk_PresentMode))
    LOG_VERBOSE(LogVulkan, "Using swapchain extent [{}x{}].", this->SurfaceExtent.width, this->SurfaceExtent.height)
    if (   (static_cast<u32>(this->FramebufferSize.x) != this->SurfaceExtent.width)
        || (static_cast<u32>(this->FramebufferSize.y) != this->SurfaceExtent.height))
    {
        LOG_WARNING(LogVulkan,
            "Framebuffer size [{}x{}] does not match clamped swapchain extent [{}x{}].",
            this->FramebufferSize.x, this->FramebufferSize.y,
            this->SurfaceExtent.width, this->SurfaceExtent.height
            )
    }

    Frontend._Vk_ReportFramesInFlight(maths::clamp<u32>(rhi::max_frames_in_flight, this->Vk_SurfaceCapabilities.minImageCount, this->Vk_SurfaceCapabilities.maxImageCount));
    check( Frontend.Vk_GetNumberOfFramesInFlight() >= this->Vk_SurfaceCapabilities.minImageCount
        && Frontend.Vk_GetNumberOfFramesInFlight() <= this->Vk_SurfaceCapabilities.maxImageCount )

    vk::SwapchainCreateInfoKHR SwapChainCreateInfo{
        .flags = vk::SwapchainCreateFlagsKHR{},
        .surface = this->Vk_Surface,
        .minImageCount = Frontend.Vk_GetNumberOfFramesInFlight(),
        .imageFormat = Frontend.Vk_GetSurfaceFormat().format,
        .imageColorSpace = Frontend.Vk_GetSurfaceFormat().colorSpace,
        .imageExtent = this->SurfaceExtent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0, /* Optional */
        .pQueueFamilyIndices = nullptr, /* Optional */
        .preTransform = this->Vk_SurfaceCapabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = this->Vk_PresentMode,
        .clipped = vk::True,
        .oldSwapchain = VK_NULL_HANDLE
        };

    const u32 QueueFamilyIndices[] {Frontend.Vk_GetGraphicsQueueFamilyIndex(), Frontend.Vk_GetPresentQueueFamilyIndex()};
    if (Frontend.Vk_GetGraphicsQueue() == Frontend.Vk_GetPresentQueue())
    {
        LOG_VERBOSE(LogVulkan, "Using exclusive image sharing mode for swapchain as graphics and present queues are the same.")
    }
    else
    {
        LOG_VERBOSE(LogVulkan, "Using concurrent image sharing mode for swapchain as graphics and present queues differ.")
        SwapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent; /* TODO: Not optimal performance wise. But we can fix this later. */
        SwapChainCreateInfo.queueFamilyIndexCount = 2;
        SwapChainCreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
    }

    auto SwapchainResult{Frontend.Vk_GetDevice().createSwapchainKHR(SwapChainCreateInfo)};
    check(SwapchainResult.has_value())
    this->Vk_VkMySwapchain = std::move(*SwapchainResult);
    {
        auto ImagesResult{this->Vk_VkMySwapchain.getImages()};
        check(ImagesResult.has_value())
        for (auto Image: *ImagesResult)
        {
            this->Vk_SwapchainImages.push_back(Image);
        }
        LOG_VERBOSE(LogVulkan, "Created swapchain with [{}] images.", this->Vk_SwapchainImages.size())
    }
    if (this->Vk_SwapchainImages.size() != Frontend.Vk_GetNumberOfFramesInFlight())
    {
        LOG_FATAL(LogVulkan, "Number of images in swapchain [{}] does not match number of expected frames in flight reported by the frontend [{}].",
            this->Vk_SwapchainImages.size(), Frontend.Vk_GetNumberOfFramesInFlight()
            )
    }

    this->bPendingResize = false;
    this->PendingResizeExtent = maths::zero_vector<LVec2u32>;
    this->PendingTimeForResizeApply = 0.0f;

    this->__Vk_CreateImageViews();
    this->__Vk_CreateColorResources();
    this->__Vk_CreateSynchObjects();
}

std::optional<vk::SurfaceFormatKHR> Jafg::LSurfaceGlfw3::Vk_GetSwapchainSurfaceFormatKHR(vk::SurfaceFormatKHR DesiredSurfaceFormat)
{
    for (auto const& AvailableFormat: this->Vk_AvailableSurfaceFormats)
    {
        if (   AvailableFormat.format == DesiredSurfaceFormat.format
            && AvailableFormat.colorSpace == DesiredSurfaceFormat.colorSpace)
        {
            return AvailableFormat;
        }

        continue;
    }

    return {};
}

std::optional<vk::PresentModeKHR> Jafg::LSurfaceGlfw3::Vk_GetSwapchainPresentModeKHR(vk::PresentModeKHR DesiredPresentMode)
{
    if (algo::contains(this->Vk_AvailablePresentModes, DesiredPresentMode))
    {
        return DesiredPresentMode;
    }

    /* Hardcoded fallback */
    if (this->GetFrontend().Vk_GetFramework() == rhi::framework::x11 && DesiredPresentMode == vk::PresentModeKHR::eMailbox)
    {
        if (algo::contains(this->Vk_AvailablePresentModes, vk::PresentModeKHR::eImmediate))
        {
            LOG_WARNING(LogVulkan, "Preferred swap present mode [eMailbox] is not available on x11. Falling back to nonblocking eImmediate present mode.")
            return vk::PresentModeKHR::eImmediate;
        }
    }

    return {};
}

void Jafg::LSurfaceGlfw3::__Vk_CreateImageViews()
{
    LOG_VERBOSE(LogVulkan, "Creating image views from swapchain images from surface [{}].", this->GetHumanReadableName())

    auto& Frontend{ this->GetFrontend() };

    check( this->Vk_SwapchainImageViews.empty() )

    for (auto const& SwapchainImage : this->Vk_SwapchainImages)
    {
        this->Vk_SwapchainImageViews.emplace_back(rhi::vk_build(Frontend.Vk_GetDevice(), vk::ImageViewCreateInfo{
            .image = SwapchainImage,
            .viewType = vk::ImageViewType::e2D,
            .format = Frontend.Vk_GetSurfaceFormat().format,
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
                }
            }));
    }

    LOG_VERBOSE(LogVulkan, "Created [{}] image views from swapchain images.", this->Vk_SwapchainImageViews.size())
}

void Jafg::LSurfaceGlfw3::__Vk_CreateColorResources()
{
    LOG_VERBOSE(LogVulkan, "Creating color resources for surface [{}].", this->GetHumanReadableName())

    auto& Frontend{ this->GetFrontend() };

    check(this->SurfaceExtent.width > 0 && this->SurfaceExtent.height > 0)
    vk::ImageCreateInfo ImageCreateInfo{
        .imageType = vk::ImageType::e2D,
        .format = Frontend.Vk_GetSurfaceFormat().format,
        .extent = vk::Extent3D{ this->SurfaceExtent.width, this->SurfaceExtent.height, 1 },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = Frontend.Vk_GetMaxMsaaSampleCount(),
        .tiling = vk::ImageTiling::eOptimal,
        .usage = vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
        .sharingMode = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
        };
    // VmaAllocationCreateInfo AllocationCreateInfo{
    //     .usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    //     };

    this->Vk_ColorImage = Frontend.Vk_CreateDeviceLocalImage(ImageCreateInfo);
    // this->Vk_ColorImage = Frontend.Vk_CreateImage(ImageCreateInfo, AllocationCreateInfo);
    this->Vk_ColorImageView = rhi::vk_build(Frontend.Vk_GetDevice(), vk::ImageViewCreateInfo{
        .image = *this->Vk_ColorImage,
        .viewType = vk::ImageViewType::e2D,
        .format = Frontend.Vk_GetSurfaceFormat().format,
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
            },
        });
}

void Jafg::LSurfaceGlfw3::__Vk_CreateSynchObjects()
{
    LOG_VERBOSE(LogVulkan, "Creating [{}+2*{}] Vulkan synchronization objects for surface [{}].",
        this->Vk_GetNumberOfFramesInFlightInternal(), this->Vk_SwapchainImages.size(), this->GetHumanReadableName()
        )

    auto& Device{this->GetFrontend().Vk_GetDevice()};

    for (auto Idx{0uz}; Idx < this->Vk_GetNumberOfFramesInFlightInternal(); ++Idx)
    {
        auto ImageSemaphoreResult{Device.createSemaphore(vk::SemaphoreCreateInfo{})};
        check(ImageSemaphoreResult.has_value())
        this->Vk_ImageAvailableSemaphores[Idx] = std::move(*ImageSemaphoreResult);
        auto RenderSemaphoreResult{Device.createSemaphore(vk::SemaphoreCreateInfo{})};
        check(RenderSemaphoreResult.has_value())
        this->Vk_RenderSemaphores[Idx] = std::move(*RenderSemaphoreResult);

        auto FenceResult{Device.createFence(vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled })};
        check(FenceResult.has_value())
        this->Vk_FlightFences[Idx] = std::move(*FenceResult);

        continue;
    }

    this->Vk_LastFrameInFlightIndex = 0;
    check( this->Vk_CurrentFrameInFlightIndex.has_value() == false )
}

void Jafg::LSurfaceGlfw3::Vk_CreateCommandBuffers()
{
    LOG_VERBOSE(LogVulkan, "Creating [{}] Vulkan command buffers for surface [{}].", this->Vk_GetNumberOfFramesInFlightInternal(), this->GetHumanReadableName())

    auto CommandBuffers{rhi::vk_allocate(this->GetFrontend().Vk_GetDevice(), vk::CommandBufferAllocateInfo{
        .commandPool = this->Vk_CommandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = static_cast<uint32_t>(this->Vk_GetNumberOfFramesInFlightInternal())
        })};
    check(this->Vk_CommandBuffers.size() == CommandBuffers.size())
    for (auto Idx{0uz}; Idx < CommandBuffers.size(); ++Idx)
    {
        this->Vk_CommandBuffers[Idx] = std::move(CommandBuffers[Idx]);
    }
}

void Jafg::LSurfaceGlfw3::Vk_CreateDescriptorPools()
{
    LOG_VERBOSE(LogVulkan, "Creating descriptor pools for surface [{}].", this->GetHumanReadableName())

    std::array Sizes{
        vk::DescriptorPoolSize{
            .type = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = 10,
            },
        vk::DescriptorPoolSize{
            .type = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = 10,
            },
        };

    for (auto Idx{0uz}; Idx < this->Vk_GetNumberOfFramesInFlightInternal(); ++Idx)
    {
        this->Vk_DescriptorPools[Idx] = rhi::vk_build(this->GetFrontend().Vk_GetDevice(), vk::DescriptorPoolCreateInfo{
            .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            .maxSets = 2,
            .poolSizeCount = static_cast<u32>(Sizes.size()),
            .pPoolSizes = Sizes.data(),
            });

        continue;
    }
}

#endif /* JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */
