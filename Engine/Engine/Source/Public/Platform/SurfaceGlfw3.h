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

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

namespace Jafg
{

namespace Private
{

struct LGlfw3Bridge;

} /* ~Namespace Jafg::Private */

struct Vertex
{
    glm::vec3 pos;
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
                .location = 0, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, pos)
                },
            vk::VertexInputAttributeDescription{
                .location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, color)
                },
            vk::VertexInputAttributeDescription{
                .location = 2, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(Vertex, texCoord)
                }
        };
    }

    bool operator==(const Vertex &other) const
    {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

} /* ~Namespace Jafg */

template <>
struct std::hash<Jafg::Vertex>
{
    size_t operator()(Jafg::Vertex const &vertex) const noexcept
    {
        return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
    }
};

namespace Jafg
{

class LSurfaceGlfw3 final : public LSurfaceBase
{
public:

    friend Private::LGlfw3Bridge;

    typedef LSurfaceBase Super;

    static_assert(std::is_same_v<LSurfaceGlfw3, LSurface>);

    ENGINE_API LSurfaceGlfw3(LSurfaceCreateInfo const& Info);
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
    FORCEINLINE virtual bool IsVSync() const noexcept override { return this->bVSync; }

    ENGINE_API  virtual bool CanResize() const override;
    ENGINE_API  virtual void SetResizable(const bool bInResizable) override;
    FORCEINLINE virtual bool IsResizable() const noexcept override { return this->bResizable; }

    FORCEINLINE bool IsFirstMouseCallback() const noexcept { return this->bFirstMouseCallback; }
    FORCEINLINE f64  GetLastMouseX() const noexcept { return this->LastMouseX; }
    FORCEINLINE f64  GetLastMouseY() const noexcept { return this->LastMouseY; }

    FORCEINLINE GLFWcursor* GetNativeCursorHandleDangerous() const { return this->Cursor; }
    FORCEINLINE GLFWwindow* GetNativeHandleDangerous() const { return this->Handle; }

    FORCEINLINE auto const& GetVkSurface() const { return this->VkMySurface; }

    FORCEINLINE auto const& GetVkSwapchainSurfaceFormat() const { return this->VkMySwapchainSurfaceFormat; }
    FORCEINLINE auto const& GetVkSwapchainPresentMode() const { return this->VkMySwapchainPresentMode; }
    FORCEINLINE auto const& GetVkSwapchainExtent() const { return this->VkMySwapchainExtent; }
    FORCEINLINE auto const& GetVkSwapchain() const { return this->VkMySwapchain; }

    FORCEINLINE auto const& GetVkCommandPool() const { return this->VkMyCommandPool; }

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
    void VkCreateDescriptorSetLayout();
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
        vk::Image Image,
        vk::ImageLayout OldLayout,
        vk::ImageLayout NewLayout,
        vk::AccessFlags2 SrcAccessMask,
        vk::AccessFlags2 DstAccessMask,
        vk::PipelineStageFlags2 SrcStageMask,
        vk::PipelineStageFlags2 DstStageMask,
        vk::ImageAspectFlags AspectMask
        );

    u32 FindMemoryType(u32 TypeFilter, vk::MemoryPropertyFlags Properties) const;

    GLFWcursor* Cursor{ nullptr };
    GLFWwindow* Handle{ nullptr };

    bool bVSync{ false };
    bool bResizable{ false };

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
    vk::raii::DescriptorSetLayout VkMyDescriptorSetLayout{ nullptr };
    vk::raii::PipelineLayout VkMyPipelineLayout{ nullptr };
    vk::raii::Pipeline VkMyPipeline{ nullptr };
    vk::raii::CommandPool VkMyCommandPool{ nullptr };
    std::vector<vk::raii::CommandBuffer> VkCommandBuffers;
    vk::raii::DescriptorPool VkMyDescriptorPool{ nullptr };
    TArray<vk::raii::DescriptorSet> VkDescriptorSets;
    u32 VkFlightSyncFrameIndex{ 0 };
    u32 VkSemaphoreSyncIndex{ 0 };
    TArray<vk::raii::Semaphore> VkPresentSemaphores;
    TArray<vk::raii::Semaphore> VkRenderSemaphores;
    TArray<vk::raii::Fence> VkFlightFences;

    LVmaBuffer VertexBuffer;
    LVmaBuffer IndexBuffer;

    TArray<LVmaMappedBuffer> UniformBuffers;

    LVmaImage TextureImage;
    vk::raii::ImageView TextureImageView{ nullptr };
    vk::raii::Sampler TextureSampler{ nullptr };

    LVmaImage DepthImage;
    vk::raii::ImageView DepthImageView{ nullptr };

    std::vector<Vertex> Vertices;
    std::vector<u32> Indices;

    u32 MipLevels = INDEX_NONE;

    LVmaImage ColorImage;
    vk::raii::ImageView ColorImageView{ nullptr };

    void VkCreateColorResources();
    void VkCreateDepthResources();
    void VkCreateTextureImage();
    void VkCreateTextureImageView();
    void VkCreateTextureSampler();

    void VkLoadModel();
    void VkCreateVertexBuffer();
    void VkCreateIndexBuffer();
    void VkCreateUniformBuffers();

    void VkCreateDescriptorPool();
    void VkCreateDescriptorSets();
    void VkUpdateUniformBuffers(uint32_t currentImage);

    void VkGenerateMipMaps(vk::Image, vk::Format Format, i32 Width, i32 Height, u32 MipLevels);
};

} /* ~Namespace Jafg */
