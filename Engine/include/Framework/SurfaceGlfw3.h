// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Frontend.h"
#include "Framework/Surface.h"
#include "Rhi/RenderInfo.h"

#if !JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER
    #error "Tried to include glfw3 specific code on a platform that does not support glfw3."
#endif /* !JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */

#include "Framework/Cursor.h"
#include "Rhi/RendererCore.h"
#include "Rhi/DeviceBuffers.h"

struct GLFWwindow;
struct GLFWcursor;

namespace Jafg
{

struct LNodeRenderInfo;
struct LPhysicalViewport;

struct LFileDialogInfo
{
    struct Filter final
    {
        LStringView DisplayName;
        LStringView Specs;
    };
    TArray<Filter> Filters;
};

struct LSaveFileDialogInfo : public LFileDialogInfo
{
    LPath Path;
    LPath Name;
};

namespace FileDialogFilterPresets
{

inline constexpr LFileDialogInfo::Filter All{
    .DisplayName = "All files",
    .Specs = "",
    };
inline constexpr LFileDialogInfo::Filter Headers{
    .DisplayName = "Header files",
    .Specs = "h,hpp,hh,hxx,afx",
    };
inline constexpr LFileDialogInfo::Filter Source{
    .DisplayName = "Source files",
    .Specs = "c,cpp,cc,cxx",
    };
inline constexpr LFileDialogInfo::Filter CppC{
    .DisplayName = "C++/C files",
    .Specs = "h,hpp,hh,hxx,afx,c,cpp,cc,cxx",
    };
inline constexpr LFileDialogInfo::Filter Shaders{
    .DisplayName = "Shader files",
    .Specs = "slang,vert,frag",
    };
inline constexpr LFileDialogInfo::Filter JAssets{
    .DisplayName = "JAssets",
    .Specs = "jasset",
    };
inline constexpr LFileDialogInfo::Filter Textures{
    .DisplayName = "Textures",
    .Specs = "png,jpg,jpeg",
    };
inline constexpr LFileDialogInfo::Filter Models{
    .DisplayName = "Models",
    .Specs = "gltf,glb",
    };

inline TArray<LFileDialogInfo::Filter> AllFilters() noexcept
{
    return {
        All,
        Headers,
        Source,
        CppC,
        Shaders,
        JAssets,
        Textures,
        Models,
        };
}

} /* FileDialogFilterPresets */

namespace Private
{

struct LGlfw3Bridge;

} /* ~Namespace Private */

struct LRenderInfo;

//# The pixels of one frame that a surface presented (@see #LSurfaceGlfw3::RequestCapture).
struct LSurfaceCapture final
{
    //# In physical pixels.
    rhi::extent2 Extent;
    //# The format of the swapchain. It describes #Bytes until #ToRgba8 was called.
    vk::Format Format{ vk::Format::eUndefined };
    //# #BytesPerPixel bytes per pixel. Rows from top to bottom without padding.
    TArray<u8> Bytes;

    //# Of every supported format (@see #IsSupportedFormat).
    static constexpr std::size_t BytesPerPixel{ 4uz };

    //# Whether #ToRgba8 understands the format. Only these formats can be captured.
    NODISCARD ENGINE_API static bool IsSupportedFormat(vk::Format Format) noexcept;
    //#
    //# Rewrites #Bytes as 8 bit RGBA with an opaque alpha. The values stay as the surface showed them (e.g., sRGB
    //# encoded). May be called on any thread.
    //#
    ENGINE_API void ToRgba8() noexcept;
};

typedef TFunction2<void(LSurfaceCapture Capture)> LOnSurfaceCaptured;

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

    //# Whether the platform allows to move the mouse cursor programmatically (e.g., not on Wayland).
    NODISCARD ENGINE_API bool CanWarpMouseCursor() const noexcept;
    //#
    //# Moves the mouse cursor to a location in surface space.
    //# @return False if the platform does not support it. See #CanWarpMouseCursor.
    //#
    ENGINE_API bool WarpMouseCursor(LVec2F Location) noexcept;

    //# @return The UTF-8 text content of the platform clipboard. Empty if the clipboard is empty or holds no text.
    NODISCARD ENGINE_API LString GetClipboardText() const;
    //# Replaces the content of the platform clipboard with the given UTF-8 text.
    ENGINE_API void SetClipboardText(LString const& Text) const;

    //# Whether #RequestCapture can copy what this surface presents (the platform and the surface format allow it).
    NODISCARD FORCEINLINE bool CanCapture() const noexcept { return this->bVk_CanCapture; }
    //#
    //# Copies the next frame that this surface presents: all of it, in its extent. #OnCaptured is called on the
    //# renderer thread once the device has finished the copy, usually some frames later. It is not called if the
    //# surface closes before it presents another frame.
    //#
    //# @return False if the surface cannot capture (@see #CanCapture). #OnCaptured is not called then.
    //#
    ENGINE_API bool RequestCapture(LOnSurfaceCaptured OnCaptured);

    FORCEINLINE GLFWcursor* _GetNativeCursorHandleDangerous() const noexcept { return this->Cursor; }
    FORCEINLINE GLFWwindow* _GetNativeHandleDangerous() const noexcept { return this->Handle; }

    NODISCARD FORCEINLINE bool CanEverResize() const noexcept { return true; }
    ENGINE_API void SetResizable(bool bResizable);
    NODISCARD FORCEINLINE bool IsResizable() const noexcept { return this->bResizable; }
    ENGINE_API void SetWindowSize(LVec2u32 Size);
    ENGINE_API bool CanBorderless() const noexcept;
    ENGINE_API bool IsBorderless() const noexcept;
    ENGINE_API void SetBorderless(bool bBorderless);
    ENGINE_API bool IsFullscreen() const noexcept;
    ENGINE_API void SetFullscreen(bool bFullscreen);

    FORCEINLINE bool _HasPendingResize() const noexcept { return this->bPendingResize; }
    FORCEINLINE auto _GetPendingResizeExtent() const noexcept { return this->PendingResizeExtent; }
    FORCEINLINE f32  _GetPendingTimeForResizeApply() const noexcept { return this->PendingTimeForResizeApply; }

    FORCEINLINE auto const& Vk_GetCommandPool() const noexcept { return this->Vk_CommandPool; }

    FORCEINLINE auto const& Vk_GetSurface() const noexcept { return this->Vk_Surface; }

    FORCEINLINE auto const& Vk_GetSurfaceCapabilities() const noexcept { return this->Vk_SurfaceCapabilities; }

    FORCEINLINE auto const& Vk_GetAvailableSurfaceFormats() const noexcept { return this->Vk_AvailableSurfaceFormats; }
    FORCEINLINE auto Vk_GetDesiredSurfaceFormat() const noexcept { return this->Vk_DesiredSurfaceFormat; }

    NODISCARD FORCEINLINE bool Vk_HasPresentModesFetched() const noexcept { return !this->Vk_AvailablePresentModes.empty(); /* There as to be at least eFifo. */ }
    NODISCARD FORCEINLINE auto const& Vk_GetAvailablePresentModes() const noexcept { check(!this->Vk_AvailablePresentModes.empty() && "Not yet initialized.") return this->Vk_AvailablePresentModes; }
    NODISCARD FORCEINLINE auto const& Vk_GetPresentMode() const noexcept { return this->Vk_PresentMode; }

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
    FORCEINLINE bool Vk_HasLastFrameInFlight() const noexcept { return this->Vk_LastFrameInFlightIndex != NoLastFrameInFlight; }
    FORCEINLINE auto Vk_GetLastFrameInFlightIndex() const noexcept { check(this->Vk_HasLastFrameInFlight()) return this->Vk_LastFrameInFlightIndex; }
    FORCEINLINE auto Vk_GetCurrentFrameInFlightIndex() const noexcept { check( this->Vk_CurrentFrameInFlightIndex.has_value() ) return this->Vk_CurrentFrameInFlightIndex.value(); }

    FORCEINLINE auto const& Vk_GetCommandBuffers() const noexcept { return this->Vk_CommandBuffers; }

    template<typename TRenderInfo> requires std::is_base_of_v<LRenderInfo, TRenderInfo>
    FORCEINLINE auto const& Vk_GetDescriptorPool(TRenderInfo const& Info) const noexcept { return this->Vk_DescriptorPools[Info.Frame]; }

    //#
    //# Transitions an image layout for the current frame's command buffer.
    //# @note If no flight frame is currently progressed, this method would then trigger an engine panic.
    //#
    ENGINE_API void Vk_TransitionImageLayout(vk::ImageMemoryBarrier2 const& Barrier);

    //# @param Default The default path. If not set, the operating system will decide.
    ENGINE_API std::optional<LPath> OpenBlockingDialogForFile(LFileDialogInfo Info, LPath const& Default = finder::current_path());
    ENGINE_API std::optional<TArray<LPath>> OpenBlockingDialogForFiles(LFileDialogInfo Info, LPath const& Default = finder::current_path());
    ENGINE_API std::optional<LPath> OpenBlockingDialogForFileToSave(LSaveFileDialogInfo Info);
    ENGINE_API std::optional<LPath> OpenBlockingDialogForDirectory(LPath Default = finder::current_path());
    ENGINE_API std::optional<TArray<LPath>> OpenBlockingDialogForDirectories(LPath Default = finder::current_path());

private:

    FORCEINLINE auto Vk_GetNumberOfFramesInFlightInternal() const noexcept { return this->Vk_SwapchainImages.size(); }

    void FramebufferSizeCallback(i32 Width, i32 Height);
    void MouseCallback(f64 XPos, f64 YPos);
    void ScrollCallback(f64 XOffset, f64 YOffset);
    void MouseEnterCallback(i32 Entered);
    void CharCallback(u32 Codepoint);
    void KeyCallback(i32 Key, i32 Scancode, i32 Action, i32 Mods);
    void MouseButtonCallback(i32 Button, i32 Action, i32 Mods);

    LPhysicalViewport const& GetPreferredPhysicalViewport();
    LPhysicalViewport::VideoMode GetPreferredVideoMode(LPhysicalViewport const& Viewport);

    void Vk_CreateCommandPool();

    void Vk_CreateSwapchain();
        std::optional<vk::SurfaceFormatKHR> Vk_GetSwapchainSurfaceFormatKHR(vk::SurfaceFormatKHR DesiredSurfaceFormat);
        std::optional<vk::PresentModeKHR> Vk_GetSwapchainPresentModeKHR(vk::PresentModeKHR DesiredPresentMode);
        void __Vk_CreateImageViews();
        void __Vk_CreateColorResources();
        void __Vk_CreateSynchObjects();

    void Vk_CreateCommandBuffers();
    void Vk_CreateDescriptorPools();

    //#
    //# Records the copy of the swapchain image that the current frame draws to, for the requested captures. Takes care
    //# of the transition of the image to the present layout as well.
    //#
    void Vk_RecordCapture(LRenderInfo const& Info);
    //#
    //# Hands the finished copies to their callbacks. Only the ones of the given frame in flight, whose fence has to have
    //# been waited for. Empty: All of them, the device has to be idle.
    //#
    void Vk_FinishCaptures(std::optional<u32> Frame);

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

    std::inplace_vector<vk::Image, rhi::max_frames_in_flight> Vk_SwapchainImages;
    TArray<vk::raii::ImageView> Vk_SwapchainImageViews;
    rhi::device_image Vk_ColorImage;
    vk::raii::ImageView Vk_ColorImageView{ nullptr };

    rhi::frame_array<vk::raii::Semaphore> Vk_ImageAvailableSemaphores JAFG_VK_FRAME_ARRAY_INIT(nullptr);
    rhi::frame_array<vk::raii::Semaphore> Vk_RenderSemaphores JAFG_VK_FRAME_ARRAY_INIT(nullptr);
    rhi::frame_array<vk::raii::Fence> Vk_FlightFences JAFG_VK_FRAME_ARRAY_INIT(nullptr);
    static constexpr u32 NoLastFrameInFlight{ std::numeric_limits<u32>::max() };
    u32 Vk_LastFrameInFlightIndex{ NoLastFrameInFlight };
    std::optional<u32> Vk_CurrentFrameInFlightIndex;

    rhi::frame_array<vk::raii::CommandBuffer> Vk_CommandBuffers JAFG_VK_FRAME_ARRAY_INIT(nullptr);
    rhi::frame_array<vk::raii::DescriptorPool> Vk_DescriptorPools JAFG_VK_FRAME_ARRAY_INIT(nullptr);

    //# Whether the swapchain images can be copied from, and their format is understood (@see #LSurfaceCapture).
    bool bVk_CanCapture{};
    //# Waiting for the next frame that is presented.
    TArray<LOnSurfaceCaptured> RequestedCaptures;
    //# A copy that the device makes in the frame in flight #Frame.
    struct LVk_PendingCapture final
    {
        u32 Frame;
        rhi::extent2 Extent;
        vk::Format Format;
        rhi::mapped_device_buffer Buffer;
        TArray<LOnSurfaceCaptured> Callbacks;
    };
    TArray<LVk_PendingCapture> Vk_PendingCaptures;
};

} /* ~Namespace Jafg */
