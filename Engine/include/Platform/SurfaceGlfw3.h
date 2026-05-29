// Copyright mzoesch. All rights reserved.

#pragma once

#include "Platform/Surface.h"
#include "Rhi/RenderInfo.h"

#if !JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER
    #error "Tried to include glfw3 specific code on a platform that does not support glfw3."
#endif /* !JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */

#include "Platform/Cursor.h"
#include "Rhi/RendererCore.h"
#include "Rhi/DeviceBuffers.h"

struct GLFWwindow;
struct GLFWcursor;

namespace Jafg
{

struct LNodeRenderInfo;

namespace Private
{

struct LGlfw3Bridge;

} /* ~Namespace Private */

struct LRenderInfo;

//#
//# Vk methods are only available if vulkan is enabled for a given platform.
//# _ methods are platform dependent, and it should be avoided to use them outside of platform specific code.
//#
class LSurfaceGlfw3 final : public LSurfaceBase
{
public:

    friend Private::LGlfw3Bridge;

    typedef LSurfaceBase Super;

    static_assert(std::is_same_v<LSurfaceGlfw3, LSurface>);

    ENGINE_API LSurfaceGlfw3(LSurfaceCreateInfo const& Info);
    PROHIBIT_REALLOC_OF_ANY_FORM(LSurfaceGlfw3)
    ENGINE_API virtual ~LSurfaceGlfw3() override;

    //# Vk specific setup methods that may only be called after vk was fully initialized in the frontend.
    void LateSetupVk();

    void PollPlatformEvents();
    mutable MULTI_EVENT_DECL_VERBOSE(LSurfaceGlfw3, OnPreRender, LRenderInfo const& Info)
    void OnRender();

    ENGINE_API void SetInputMode(EInputMode InMode) noexcept;
    ENGINE_API void _SetMouseCursor(ECursor Cursor);

    FORCEINLINE GLFWcursor* _GetNativeCursorHandleDangerous() const noexcept { return this->Cursor; }
    FORCEINLINE GLFWwindow* _GetNativeHandleDangerous() const noexcept { return this->Handle; }

    NODISCARD FORCEINLINE bool CanEverResize() const noexcept { return true; }
    ENGINE_API void SetResizable(bool bResizable);
    NODISCARD FORCEINLINE bool IsResizable() const noexcept { return this->bResizable; }

    FORCEINLINE bool _HasPendingResize() const noexcept { return this->bPendingResize; }
    FORCEINLINE auto _GetPendingResizeExtent() const noexcept { return this->PendingResizeExtent; }
    FORCEINLINE f32  _GetPendingTimeForResizeApply() const noexcept { return this->PendingTimeForResizeApply; }

    FORCEINLINE auto const& Vk_GetCommandPool() const noexcept { return this->Vk_CommandPool; }

    FORCEINLINE auto const& Vk_GetSurface() const noexcept { return this->Vk_Surface; }

    FORCEINLINE auto const& Vk_GetSurfaceCapabilities() const noexcept { return this->Vk_SurfaceCapabilities; }

    FORCEINLINE auto const& Vk_GetAvailableSurfaceFormats() const noexcept { return this->Vk_AvailableSurfaceFormats; }
    FORCEINLINE auto Vk_GetDesiredSurfaceFormat() const noexcept { return this->Vk_DesiredSurfaceFormat; }

    FORCEINLINE auto const& Vk_GetAvailablePresentModes() const noexcept { return this->Vk_AvailablePresentModes; }
    FORCEINLINE auto const& Vk_GetPresentMode() const noexcept { return this->Vk_PresentMode; }

    FORCEINLINE auto _GetWindowFrameSizeTopLeft() const noexcept { return this->WindowFrameSizeTopLeft; }
    FORCEINLINE auto _GetWindowFrameSizeBottomRight() const noexcept { return this->WindowFrameSizeBottomRight; }
    FORCEINLINE auto _GetWindowSize() const noexcept { return this->WindowSize; }
    FORCEINLINE auto _GetFramebufferSize() const noexcept { return this->FramebufferSize; }
    FORCEINLINE auto Vk_GetSwapchainExtent() const noexcept { return this->GetSurfaceExtent(); }
    FORCEINLINE auto const& Vk_GetSwapchain() const noexcept { return this->Vk_VkMySwapchain; }

    FORCEINLINE auto const& Vk_GetSwapchainImages() const noexcept { return this->Vk_SwapchainImages; }
    FORCEINLINE auto const& Vk_GetSwapchainImageViews() const noexcept { return this->Vk_SwapchainImageViews; }
    FORCEINLINE auto const& Vk_GetColorImage() const noexcept { return this->Vk_ColorImage; }
    FORCEINLINE auto const& Vk_GetColorImageView() const noexcept { return this->Vk_ColorImageView; }

    FORCEINLINE auto const& Vk_GetPresentSemaphores() const noexcept { return this->Vk_ImageAvailableSemaphores; }
    FORCEINLINE auto const& Vk_GetRenderSemaphores() const noexcept { return this->Vk_RenderSemaphores; }
    FORCEINLINE auto const& Vk_GetFlightFences() const noexcept { return this->Vk_FlightFences; }
    FORCEINLINE auto Vk_GetLastFrameInFlightIndex() const noexcept { return this->Vk_LastFrameInFlightIndex; }
    FORCEINLINE auto Vk_GetCurrentFrameInFlightIndex() const noexcept { check( this->Vk_CurrentFrameInFlightIndex.has_value() ) return this->Vk_CurrentFrameInFlightIndex.value(); }

    FORCEINLINE auto const& Vk_GetCommandBuffers() const noexcept { return this->Vk_CommandBuffers; }

    template<typename TRenderInfo> requires std::is_base_of_v<LRenderInfo, TRenderInfo>
    FORCEINLINE auto const& Vk_GetDescriptorPool(TRenderInfo const& Info) const noexcept { return this->Vk_DescriptorPools[Info.Frame]; }

    //#
    //# Transitions an image layout for the current frame's command buffer.
    //# @note If no flight frame is currently progressed, this method would then trigger an engine panic.
    //#
    ENGINE_API void Vk_TransitionImageLayout(vk::ImageMemoryBarrier2 const& Barrier);

private:

    FORCEINLINE auto Vk_GetNumberOfFramesInFlightInternal() const noexcept { return this->Vk_SwapchainImages.size(); }

    void FramebufferSizeCallback(i32 Width, i32 Height);
    void MouseCallback(f64 XPos, f64 YPos);
    void ScrollCallback(f64 XOffset, f64 YOffset);
    void MouseEnterCallback(i32 Entered);
    void CharCallback(u32 Codepoint);
    void KeyCallback(i32 Key, i32 Scancode, i32 Action, i32 Mods);
    void MouseButtonCallback(i32 Button, i32 Action, i32 Mods);

    void Vk_CreateCommandPool();

    void Vk_CreateSwapchain();
    static std::optional<vk::SurfaceFormatKHR> Vk_GetSwapchainSurfaceFormatKHR(
          std::vector<vk::SurfaceFormatKHR> const& AvailableFormats
        , vk::SurfaceFormatKHR DesiredSurfaceFormat
        );
    static std::optional<vk::PresentModeKHR> Vk_GetSwapchainPresentModeKHR(
          std::vector<vk::PresentModeKHR> const& AvailablePresentModes
        , vk::PresentModeKHR DesiredPresentMode
        );
        void __Vk_CreateImageViews();
        void __Vk_CreateColorResources();
        void __Vk_CreateSynchObjects();

    void Vk_CreateCommandBuffers();
    void Vk_CreateDescriptorPools();

    GLFWcursor* Cursor{};
    GLFWwindow* Handle{};

    bool bResizable:1{};

    bool bPendingResize{};
    LVec2u32 PendingResizeExtent{ maths::zero_vector<LVec2u32> };
    f32 PendingTimeForResizeApply{};

    vk::raii::SurfaceKHR Vk_Surface{ nullptr };
    vk::raii::CommandPool Vk_CommandPool{ nullptr };

    vk::SurfaceCapabilitiesKHR Vk_SurfaceCapabilities;
    std::vector<vk::SurfaceFormatKHR> Vk_AvailableSurfaceFormats;
    vk::SurfaceFormatKHR Vk_DesiredSurfaceFormat{.format = vk::Format::eB8G8R8A8Srgb, .colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear};

    std::vector<vk::PresentModeKHR> Vk_AvailablePresentModes;
    vk::PresentModeKHR Vk_PresentMode{ vk::PresentModeKHR::eFifo };

    LVec2i32 WindowFrameSizeTopLeft{ maths::zero_vector<LVec2i32> };
    LVec2i32 WindowFrameSizeBottomRight{ maths::zero_vector<LVec2i32> };
    LVec2i32 WindowSize{ maths::zero_vector<LVec2i32> };
    LVec2i32 FramebufferSize{ maths::zero_vector<LVec2i32> };
    vk::raii::SwapchainKHR Vk_VkMySwapchain{ nullptr };

    TStackArray<vk::Image, Jafg::Vk_DesiredMaxFramesInFlight> Vk_SwapchainImages;
    TArray<vk::raii::ImageView> Vk_SwapchainImageViews;
    LDeviceImage Vk_ColorImage;
    vk::raii::ImageView Vk_ColorImageView{ nullptr };

    TFrameArray<vk::raii::Semaphore> Vk_ImageAvailableSemaphores JAFG_VK_FRAME_ARRAY_INIT(nullptr);
    TFrameArray<vk::raii::Semaphore> Vk_RenderSemaphores JAFG_VK_FRAME_ARRAY_INIT(nullptr);
    TFrameArray<vk::raii::Fence> Vk_FlightFences JAFG_VK_FRAME_ARRAY_INIT(nullptr);
    u32 Vk_LastFrameInFlightIndex{};
    std::optional<u32> Vk_CurrentFrameInFlightIndex{};

    TFrameArray<vk::raii::CommandBuffer> Vk_CommandBuffers JAFG_VK_FRAME_ARRAY_INIT(nullptr);
    TFrameArray<vk::raii::DescriptorPool> Vk_DescriptorPools JAFG_VK_FRAME_ARRAY_INIT(nullptr);
};

} /* ~Namespace Jafg */
