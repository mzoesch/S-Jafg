// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"
#include "Platform/Surface.h"

#if !JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER
    #error "Tried to include glfw3 specific code on a platform that does not support glfw3."
#endif /* !JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */

struct GLFWwindow;
struct GLFWcursor;

#include "Rhi/VkCommon.h"

namespace Jafg
{

namespace Private
{

struct LGlfw3Bridge;

} /* ~Namespace Jafg::Private */

class LSurfaceGlfw3 final : public LSurfaceBase
{
public:

    friend Private::LGlfw3Bridge;

    typedef LSurfaceBase Super;

    static_assert(std::is_same_v<LSurfaceGlfw3, LSurface>);

    ENGINE_API LSurfaceGlfw3();
    PROHIBIT_REALLOC_OF_ANY_FORM(LSurfaceGlfw3)
    ENGINE_API virtual ~LSurfaceGlfw3() override;

    void LateSetupVk();

    virtual void OnClear() override;
    virtual void OnUpdate() override;

    FORCEINLINE virtual bool IsValid() override { return this->Handle != nullptr; }

    virtual void PollInputs() override;
    virtual void PollEvents() override;

    ENGINE_API virtual void SetInputMode(const EInputMode::Type InMode, const bool bInShowCursor) override;
    ENGINE_API virtual void SetMouseCursor(const EMouseCursor::Type InCursor) override;

               virtual i32  GetWidth() const override { return this->GetDimensions().X; }
               virtual i32  GetHeight() const override { return this->GetDimensions().Y; }
    ENGINE_API virtual auto GetDimensions() const -> LIntVector2 override;

    ENGINE_API  virtual bool CanVSync() const override;
    ENGINE_API  virtual void SetVSync(const bool bEnabled) override;
    FORCEINLINE virtual bool IsVSync() const override { return this->bVSync; }

    FORCEINLINE bool IsFirstMouseCallback() const noexcept { return this->bFirstMouseCallback; }
    FORCEINLINE f64  GetLastMouseX() const noexcept { return this->LastMouseX; }
    FORCEINLINE f64  GetLastMouseY() const noexcept { return this->LastMouseY; }

    FORCEINLINE GLFWcursor* GetNativeCursorHandleDangerous() const { return this->Cursor; }
    FORCEINLINE GLFWwindow* GetNativeHandleDangerous() const { return this->Handle; }

    FORCEINLINE vk::raii::SurfaceKHR const& GetVkSurface() const { return this->VkMySurface; }

private:

    void FramebufferSizeCallback(const i32 Width, const i32 Height);
    void MouseCallback(const f64 XPos, const f64 YPos);
    void ScrollCallback(const f64 XOffset, const f64 YOffset);
    void MouseEnterCallback(const i32 Entered);
    void CharCallback(const u32 Codepoint);
    void KeyCallback(const i32 Key, const i32 Scancode, const i32 Action, const i32 Mods);

#if PLATFORM_LINUX
    virtual void EmulateRepeatedContentForBufferedInput() override;
    virtual void EmulateContentForBufferedInput(const LKey InKey) override;
    virtual void EmulateContentForBufferedInputGlfw3(const i32 InKey);
#endif /* PLATFORM_LINUX */

    void VkCreateSwapchain();
    vk::SurfaceFormatKHR ChooseVkSwapSurfaceFormatKHR(std::vector<vk::SurfaceFormatKHR> const& AvailableFormats) const;
    vk::PresentModeKHR ChooseVkSwapPresentModeKHR(std::vector<vk::PresentModeKHR> const& AvailablePresentModes) const;
    vk::Extent2D ChooseVkSwapExtent(vk::SurfaceCapabilitiesKHR const& Capabilities) const;
    void VkCreateImageViews();
    void VkCreateGraphicsPipeline();
    vk::raii::ShaderModule CreateShaderModule(TArray<u8> const& Code) const;
    void VkCreateCommandPool();
    void VkCreateCommandBuffers();
    void VkCreateSynchObjects();

    void VkCleanSwapchain();
    void WaitForSemaphore(vk::raii::Semaphore const& Semaphore);
    void WaitForSemaphores(TArray<vk::raii::Semaphore> const& Semaphores);
    void VkRecreateSwapchain();

    void VkCreateBuffer(
          vk::DeviceSize Size
        , vk::BufferUsageFlags Usage
        , vk::MemoryPropertyFlags Properties
        , vk::raii::Buffer& Buffer
        , vk::raii::DeviceMemory& BufferMemory
        );

    void RecordCommandBuffer(u32 ImageIndex);

    void TransitionImageLayout(
        u32 ImageIndex,
        vk::ImageLayout OldLayout,
        vk::ImageLayout NewLayout,
        vk::AccessFlags2 SrcAccessMask,
        vk::AccessFlags2 DstAccessMask,
        vk::PipelineStageFlags2 SrcStage,
        vk::PipelineStageFlags2 DstStage
        );

    u32 FindMemoryType(u32 TypeFilter, vk::MemoryPropertyFlags Properties) const;

    GLFWcursor* Cursor{ nullptr };
    GLFWwindow* Handle{ nullptr };

    bool bVSync{ false };

    bool bFirstMouseCallback{ true };
    f64 LastMouseX{ 0.0 };
    f64 LastMouseY{ 0.0 };

    bool bPendingResize{ false };
    i32 PendingWidth{ 0 };
    i32 PendingHeight{ 0 };
    f32 PendingTimeForResizeApply{ 0.0f };

#if PLATFORM_LINUX
    //#
    //# This is not in the EKeys::Type format but in the Glfw3 format.
    //#
    i32 Glfw3LastNewKey{ INDEX_NONE };
#endif /* PLATFORM_LINUX */

    vk::raii::SurfaceKHR VkMySurface{ nullptr };
    vk::SurfaceFormatKHR VkMySwapchainSurfaceFormat{ vk::Format::eUndefined };
    vk::PresentModeKHR VkMySwapchainPresentMode;
    vk::Extent2D VkMySwapchainExtent;
    vk::raii::SwapchainKHR VkMySwapchain{ nullptr };
    TArray<vk::Image> VkSwapchainImages;
    TArray<vk::raii::ImageView> VkSwapchainImageViews;
    vk::raii::PipelineLayout VkMyPipelineLayout{ nullptr };
    vk::raii::Pipeline VkMyPipeline{ nullptr };
    vk::raii::CommandPool VkMyCommandPool{ nullptr };
    std::vector<vk::raii::CommandBuffer> VkCommandBuffers;
    u32 VkFlightSyncFrameIndex{ 0 };
    u32 VkSemaphoreSyncIndex{ 0 };
    TArray<vk::raii::Semaphore> VkPresentSemaphores;
    TArray<vk::raii::Semaphore> VkRenderSemaphores;
    TArray<vk::raii::Fence> VkFlightFences;

    LVmaBuffer VertexBuffer;
    LVmaBuffer IndexBuffer;

    void VkCreateVertexBuffer();
    void VkCreateIndexBuffer();
};

} /* ~Namespace Jafg */
