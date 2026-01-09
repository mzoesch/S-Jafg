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

#include "Rhi/VkAl.h"
#include "TempModel.h"
#include "Rhi/StaticMesh.h"

static Jafg::LStaticMesh VkTestMesh{"Content/Models/viking_room.obj"};
static Jafg::LDevicePipeline VkTestMeshPipeline{nullptr};

const std::string MODEL_PATH = "Content/Models/viking_room.obj";
const std::string TEXTURE_PATH = "Content/Textures/viking_room.png";

static_assert(std::is_standard_layout_v<Jafg::LStaticMesh::LVertex>);

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};
static_assert(std::is_standard_layout_v<UniformBufferObject>);
static Jafg::LDevicePipeline VkTestPipeline;
static Jafg::LDeviceBuffer VkTestVertexBuffer;
static Jafg::LDeviceBuffer VkTestIndexBuffer;

// const std::vector<Vertex> vertices = {
//     {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//     {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
//     {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
//     {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
//
//     {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//     {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
//     {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
//     {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
//     };
//
// const std::vector<uint16_t> indices = {
//     0, 1, 2, 2, 3, 0,
//     4, 5, 6, 6, 7, 4
//     };

struct LRhiVertex2D
{
    glm::vec2 Position;

    static std::array<vk::VertexInputBindingDescription, 1> const& BindingDescriptions() noexcept
    {
        static std::array<vk::VertexInputBindingDescription, 1> Desc{vk::VertexInputBindingDescription{
            .binding = 0,
            .stride = sizeof(LRhiVertex2D),
            .inputRate = vk::VertexInputRate::eVertex
            }};

        return Desc;
    }

    static std::array<vk::VertexInputAttributeDescription, 1> const& AttributeDescriptions() noexcept
    {
        static std::array<vk::VertexInputAttributeDescription, 1> Desc{vk::VertexInputAttributeDescription{
            .location = 0,
            .binding = 0,
            .format = vk::Format::eR32G32Sfloat,
            .offset = offsetof(LRhiVertex2D, Position)
            }};

        return Desc;
    }
};
static_assert(Jafg::CDeviceVertexInput<LRhiVertex2D>);
static std::vector<LRhiVertex2D> QuadVertices{
    {{-0.9f, -0.9f}},
    {{-0.3f, -0.9f}},
    {{-0.3f, -0.3f}},
    {{-0.9f, -0.3f}}
    };
static std::vector<uint16_t> QuadIndices{
    0, 2, 1, 2, 0, 3
    };

static void TestPipeline(Jafg::LSurface const& Surface);

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
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->_GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->CharCallback(Codepoint);
    }

    static void KeyCallback(::GLFWwindow* Window, const i32 Key, const i32 Scancode, const i32 Action, const i32 Mods)
    {
        LOG_WARNING(LogSurface, "Key: {}, Scancode: {}, Action: {}, Mods: {}", Key, Scancode, Action, Mods)
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->_GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->KeyCallback(Key, Scancode, Action, Mods);
    }

    static void CursorPosCallback(::GLFWwindow* Window, const f64 XPos, const f64 YPos)
    {
        // LOG_TRACE(LogSurface, "XPos: {}, YPos: {}", XPos, YPos)
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->_GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->MouseCallback(XPos, YPos);
    }

    static void MouseButtonCallback(::GLFWwindow* Window, const i32 Button, const i32 Action, const i32 Mods)
    {
        LOG_WARNING(LogSurface, "Button: {}, Action: {}, Mods: {}", Button, Action, Mods)
    }

    static void CursorEnterCallback(::GLFWwindow* Window, const i32 Entered)
    {
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->_GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->MouseEnterCallback(Entered);
    }

    static void FramebufferSizeCallback(::GLFWwindow* Window, const i32 Width, const i32 Height)
    {
        // LOG_TRACE(LogSurface, "Width: {}, Height: {}", Width, Height)
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->_GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->FramebufferSizeCallback(Width, Height);
    }

    static void ScrollCallback(::GLFWwindow* Window, const f64 XOffset, const f64 YOffset)
    {
        LOG_WARNING(LogSurface, "XOffset: {}, YOffset: {}", XOffset, YOffset)
        checkSlow( static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->_GetNativeHandleDangerous() == Window )
        static_cast<LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->ScrollCallback(XOffset, YOffset);
    }
};

} /* ~Namespace Jafg::Private */

Jafg::LSurfaceGlfw3::LSurfaceGlfw3(LSurfaceCreateInfo const& Info)
    : Super{Info}
    , Vk_ImageAvailableSemaphores{vk::raii::Semaphore{nullptr}, vk::raii::Semaphore{nullptr}, vk::raii::Semaphore{nullptr}}
    , Vk_RenderSemaphores{vk::raii::Semaphore{nullptr}, vk::raii::Semaphore{nullptr}, vk::raii::Semaphore{nullptr}}
    , Vk_FlightFences{vk::raii::Fence{nullptr}, vk::raii::Fence{nullptr}, vk::raii::Fence{nullptr}}
    , Vk_CommandBuffers{vk::raii::CommandBuffer{nullptr}, vk::raii::CommandBuffer{nullptr}, vk::raii::CommandBuffer{nullptr}}
{
    STAT_CYCLE_FUNCTION()

    check( Tasks::IsOnMasterThread() )

    LOG_VERBOSE(LogSurface, "Creating Glfw3 window surface.")

#if PLATFORM_LINUX
    // TODO: Do we need this still??
    if (glfwGetPlatform() == GLFW_PLATFORM_WAYLAND)
    {
        this->SetPlatformSupportsRepeatedKey(false);
    }
#endif /* PLATFORM_LINUX */

    check( Info.bFullScreen == false && "Full screen windows are not yet supported." )

    if (this->CanEverResize())
    {
        this->bResizable = Info.bResizable;
        glfwWindowHint(GLFW_RESIZABLE, Info.bResizable ? GLFW_TRUE : GLFW_FALSE);
    }
    else
    {
        check( this->IsResizable() == false )
        if (Info.bResizable)
        {
            LOG_WARNING(LogSurface, "The surface [{}] does not support resizing, ignoring request.", this->GetHumanReadableName())
        }
    }

    // glfwWindowHint(GLFW_DECORATED, Info.bBorderless ? GLFW_FALSE : GLFW_TRUE);
    check( Info.bBorderless == false && "Borderless windows are not yet supported." )

    {
        STAT_QUICK_CYCLE_START("Glfw3WindowCreation")
        this->Handle = glfwCreateWindow(
            Info.DesiredDimensionsPx.X, Info.DesiredDimensionsPx.Y,
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

    check( GEngine )
    this->SetVSync(GetDefault<JUserPreferences>()->bVSyncEnabled);

#if PLATFORM_WINDOWS
    const HWND NativeWindowHandle = glfwGetWin32Window(this->Handle);
    check( NativeWindowHandle )
    const u32 PlatformDpi = ::GetDpiForWindow(NativeWindowHandle);
#else /* PLATFORM_WINDOWS */
    const u32 PlatformDpi = 96; // Sketchy
#endif /* !PLATFORM_WINDOWS */
    // TODO: Update this when the window is moved to another monitor with different DPI.
    this->GetViewport().SetPlatformDpi(static_cast<f32>(PlatformDpi));
    auto WindowDimensions{ this->GetDimensions() };
    LOG_VERBOSE(LogSurface, "Glfw3 window created. Dimensions: [{}x{}], DPI: [{}]", WindowDimensions.X, WindowDimensions.Y, PlatformDpi)

    this->GetViewport().SetBackgroundColor(Lal::LLinearColor::Black);

    auto& Instance{ this->GetFrontend().GetVkInstance() };
    VkSurfaceKHR CSurface;
    if (glfwCreateWindowSurface(*Instance, this->Handle, nullptr, &CSurface) != VK_SUCCESS)
    {
        panic( "Failed to create Vulkan window surface." )
    }
    check( CSurface )
    this->Vk_Surface = vk::raii::SurfaceKHR{Instance, CSurface};

    return;
}

Jafg::LSurfaceGlfw3::~LSurfaceGlfw3()
{
    {
        STAT_QUICK_CYCLE_START("VkDeviceWaitIdle")
        this->GetFrontend().GetVkDevice().waitIdle();
    }

    VkTestPipeline.Free();
    VkTestMesh.FreeFromDevice();
    VkTestMeshPipeline.Free();

    VkTestVertexBuffer.Free();
    VkTestIndexBuffer.Free();

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

struct LVkTestMeshPipelineLayout
{
    static std::array<vk::DescriptorSetLayoutBinding, 2> const& Bindings() noexcept
    {
        static std::array<vk::DescriptorSetLayoutBinding, 2> Bindings{
            vk::DescriptorSetLayoutBinding{
                .binding = 0, .descriptorType = vk::DescriptorType::eUniformBuffer, .descriptorCount = 1, .stageFlags = vk::ShaderStageFlagBits::eVertex, .pImmutableSamplers = nullptr
                },
            vk::DescriptorSetLayoutBinding{
                .binding = 1, .descriptorType = vk::DescriptorType::eCombinedImageSampler, .descriptorCount = 1, .stageFlags = vk::ShaderStageFlagBits::eFragment, .pImmutableSamplers = nullptr
                },
            };

        return Bindings;
    }

};
static_assert(Jafg::CDeviceLayout<LVkTestMeshPipelineLayout>);

void Jafg::LSurfaceGlfw3::LateSetupVk()
{
    this->Vk_CreateCommandPool();
    this->Vk_CreateSwapchain();
    this->Vk_CreateCommandBuffers();

    if (VkTestMesh.ReloadModel() != EStaticMeshResult::Success)
    {
        panic( "Failed to load test static mesh model." )
    }

    VkTestMesh.UploadVertices();
    VkTestMeshPipeline = LDevicePipelineFactory{*this}
        .Shader("Content/Shaders/Spir-V/Test.spv", vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
        .VertexInput<LStaticMesh::LVertex>()
        .Layout<LVkTestMeshPipelineLayout>()
        .Build();






    this->VkCreateTextureImage();
    this->VkCreateTextureSampler();


    this->VkCreateUniformBuffers();
    this->VkCreateDescriptorPool();
    this->VkCreateDescriptorSets();

    TestPipeline(*this);

    return;
}

void Jafg::LSurfaceGlfw3::OnClear()
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    // glfwMakeContextCurrent(this->Handle);

    this->GetViewport().OnClear();

    return;
}

static_assert(UINT64_MAX == std::numeric_limits<u64>::max());

void Jafg::LSurfaceGlfw3::OnUpdate()
{
    STAT_CYCLE_FUNCTION()

    check( this->Handle )
    check( Tasks::IsOnRendererThread() )

    check( this->Vk_LastFrameInFlightIndex < Jafg::Vk_DesiredMaxFramesInFlight )

    check( this->Vk_CommandBuffers.size() == this->Vk_GetNumberOfFramesInFlight() )

    auto& Frontend{ this->GetFrontend() };

    this->Vk_CurrentFrameInFlightIndex = (this->Vk_LastFrameInFlightIndex + 1) % this->Vk_GetNumberOfFramesInFlight();

    while (vk::Result::eTimeout == Frontend.GetVkDevice().waitForFences(*this->Vk_FlightFences[*this->Vk_CurrentFrameInFlightIndex], vk::True, UINT64_MAX))
        ;

    auto [Result, ImageIndex] = this->Vk_VkMySwapchain.acquireNextImage(
        std::numeric_limits<u64>::max(), this->Vk_ImageAvailableSemaphores[*this->Vk_CurrentFrameInFlightIndex], nullptr
        );
    check( ImageIndex < this->Vk_SwapchainImageViews.size() )

    if (Result == vk::Result::eErrorOutOfDateKHR)
    {
        LOG_VERBOSE(LogSurface, "Swapchain is out of date. Recreating swapchain.")
        this->Vk_CurrentFrameInFlightIndex.reset();
        this->Vk_CreateSwapchain();
        return;
    }

    if (this->bPendingResize)
    {
        this->PendingTimeForResizeApply -= Application::GetDeltaTimeAsFloat();

        if (this->PendingTimeForResizeApply <= 0.0f)
        {
            LOG_VERBOSE(LogSurface,
                "Applying pending resize to surface [{}x{}].",
                this->PendingResizeExtent.X,
                this->PendingResizeExtent.Y
                )

            LOG_VERBOSE(LogSurface, "Recreating swapchain due to suboptimal state and pending resize.")
            this->Vk_CurrentFrameInFlightIndex.reset();
            this->Vk_CreateSwapchain();
            check( this->bPendingResize == false )
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
        panicMsgf( "Failed to acquire swapchain image. vk::Result: [{}].", static_cast<u32>(Result) )
    }

    Frontend.GetVkDevice().resetFences(*this->Vk_FlightFences[*this->Vk_CurrentFrameInFlightIndex]);

    auto& CommandBuffer{ this->Vk_CommandBuffers[*this->Vk_CurrentFrameInFlightIndex] };
    CommandBuffer.reset();
    CommandBuffer.begin({});

    Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .srcAccessMask = {},
        .dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eColorAttachmentOptimal,
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

    Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        .dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = this->Vk_ColorImage.Buffer,
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
            },
        });

    Vk_TransitionImageLayout({
        .srcStageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
        .srcAccessMask = vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        .dstStageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
        .dstAccessMask = vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        .oldLayout = vk::ImageLayout::eUndefined,
        .newLayout = vk::ImageLayout::eDepthAttachmentOptimal,
        .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
        .image = this->Vk_DepthImage.Buffer,
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eDepth,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
            },
        });

    constexpr vk::ClearValue ClearColor(vk::ClearColorValue(std::array<f32,4>{0.0f, 0.0f, 0.0f, 1.0f}));
    constexpr vk::ClearValue ClearDepth{.depthStencil = vk::ClearDepthStencilValue{.depth = 1.0f, .stencil = 0}};

    vk::RenderingAttachmentInfo ColorAttachmentInfo{
        .imageView = this->Vk_ColorImageView,
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .resolveMode = vk::ResolveModeFlagBits::eAverage,
        .resolveImageView = this->Vk_SwapchainImageViews[ImageIndex],
        .resolveImageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = ClearColor
        };
    vk::RenderingAttachmentInfo DepthAttachmentInfo{
        .imageView   = this->Vk_DepthImageView,
        .imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
        .loadOp      = vk::AttachmentLoadOp::eClear,
        .storeOp     = vk::AttachmentStoreOp::eDontCare,
        .clearValue  = ClearDepth
        };

    vk::RenderingInfo RenderingInfo{
        .renderArea = { .offset = { 0, 0 }, .extent = this->Vk_SwapchainExtent },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &ColorAttachmentInfo,
        .pDepthAttachment = &DepthAttachmentInfo
        };

    CommandBuffer.beginRendering(RenderingInfo);

    CommandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<f32>(this->Vk_SwapchainExtent.width), static_cast<f32>(this->Vk_SwapchainExtent.height), 0.0f, 1.0f));
    CommandBuffer.setScissor( 0, vk::Rect2D( vk::Offset2D( 0, 0 ), this->Vk_SwapchainExtent ) );

    this->VkUpdateUniformBuffers(*this->Vk_CurrentFrameInFlightIndex);
    CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *::VkTestMeshPipeline.Pipeline);
    CommandBuffer.bindVertexBuffers(0, VkTestMesh.VertexBuffer.Buffer, {0});
    CommandBuffer.bindIndexBuffer(VkTestMesh.IndexBuffer.Buffer, 0, vk::IndexTypeValue<decltype(VkTestMesh.Indices)::value_type>::value);
    CommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *::VkTestMeshPipeline.PipelineLayout, 0 , *this->VkDescriptorSets[*this->Vk_CurrentFrameInFlightIndex], nullptr);
    CommandBuffer.drawIndexed(VkTestMesh.Indices.size(), 1, 0, 0, 0);

    CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *VkTestPipeline.Pipeline);
    CommandBuffer.bindVertexBuffers(0, VkTestVertexBuffer.Buffer, {0});
    CommandBuffer.bindIndexBuffer(VkTestIndexBuffer.Buffer, 0, vk::IndexTypeValue<decltype(QuadIndices)::value_type>::value);
    CommandBuffer.drawIndexed(QuadIndices.size(), 1, 0, 0, 0);

    this->GetViewport().Draw();

    CommandBuffer.endRendering();

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

    CommandBuffer.end();

    vk::PipelineStageFlags DstStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput );
    const vk::SubmitInfo SubmitInfo{
        .waitSemaphoreCount = 1, .pWaitSemaphores = &*this->Vk_ImageAvailableSemaphores[*this->Vk_CurrentFrameInFlightIndex],
        .pWaitDstStageMask = &DstStageMask,
        .commandBufferCount = 1, .pCommandBuffers = &*CommandBuffer,
        .signalSemaphoreCount = 1, .pSignalSemaphores = &*this->Vk_RenderSemaphores[ImageIndex]
        };
    Frontend.GetVkGraphicsQueue().submit(SubmitInfo, *this->Vk_FlightFences[*this->Vk_CurrentFrameInFlightIndex]);

    const vk::PresentInfoKHR PresentInfo{
        .waitSemaphoreCount = 1, .pWaitSemaphores = &*this->Vk_RenderSemaphores[ImageIndex],
        .swapchainCount = 1, .pSwapchains = &*this->Vk_VkMySwapchain,
        .pImageIndices = &ImageIndex
        };
    Result = Frontend.GetVkGraphicsQueue().presentKHR(PresentInfo);

    this->Vk_LastFrameInFlightIndex = *this->Vk_CurrentFrameInFlightIndex;
    this->Vk_CurrentFrameInFlightIndex.reset();

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
        LOG_WARNING(LogVulkan, "vk::Queue::presentKHR returned unexpected vk::Result [{}].", vk::to_string(Result))
        break;
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
    else
    {
        glfwPollEvents();
    }

    return;
}

void Jafg::LSurfaceGlfw3::SetInputMode(EInputMode::Type InMode) noexcept
{
    check( this->Handle )
    check( Tasks::IsOnMasterThread() )

    this->InputMode = InMode;

    if (this->IsShowMouseCursor() == false)
    {
        this->MouseLocation.reset();
        this->LastMouseLocation.reset();
    }

    // glfwMakeContextCurrent(this->Handle);
    // glfwSetInputMode(this->Handle, GLFW_CURSOR, this->bShowCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

    return;
}

void Jafg::LSurfaceGlfw3::_SetMouseCursor(const EMouseCursor::Type InCursor)
{
    check( Tasks::IsOnMasterThread() )
    check( this->Handle )
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

void Jafg::LSurfaceGlfw3::SetVSync(const bool bEnabled)
{
    checkSlow( Tasks::IsOnMasterThread() )
    checkSlow( this->Handle )

    if (this->IsVSync() == bEnabled)
    {
        return;
    }

    this->bVSync = bEnabled;

    /* TODO: How to handle this?? */

    // glfwMakeContextCurrent(this->Handle);
    // glfwSwapInterval(this->bVSync ? 1 : 0);

    return;
}

void Jafg::LSurfaceGlfw3::SetResizable(const bool bResizable)
{
    check( Tasks::IsOnMasterThread() )
    check( this->Handle )

    if (this->IsResizable() == bResizable)
    {
        return;
    }

    this->bResizable = bResizable;
    check( this->IsResizable() == bResizable )

    LOG_VERBOSE(LogSurface, "Setting window resizeability to [{}].", this->IsResizable() ? "true" : "false")
    glfwSetWindowAttrib(this->Handle, GLFW_RESIZABLE, this->IsResizable() ? GLFW_TRUE : GLFW_FALSE);

    return;
}

void Jafg::LSurfaceGlfw3::Vk_TransitionImageLayout(vk::ImageMemoryBarrier2 const& Barrier)
{
    check( this->Vk_CurrentFrameInFlightIndex.has_value() )

    vk::DependencyInfo DependencyInfo{
        .dependencyFlags = {},
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &Barrier
        };

    this->Vk_CommandBuffers[*this->Vk_CurrentFrameInFlightIndex].pipelineBarrier2(DependencyInfo);

    return;
}

void Jafg::LSurfaceGlfw3::FramebufferSizeCallback(const i32 Width, const i32 Height)
{
    this->bPendingResize = true;
    this->PendingResizeExtent = LUIntVector2(static_cast<u32>(Width), static_cast<u32>(Height));

    /*
     * One-point-five-second delay when making changes to the viewport. We might want to change this later depending
     * on some platform parameters.
     */
    // this->PendingTimeForResizeApply = 1.5f;
    this->PendingTimeForResizeApply = 0.2f;

    return;
}

void Jafg::LSurfaceGlfw3::MouseCallback(const f64 XPos, const f64 YPos)
{
    check( this->bIsMouseInsideSurface )

    this->MouseLocation = LVector2D{XPos, YPos};

    // TODO: Do not parse mouse location data when mouse cursor is shown. Only for now. In the future we will handle these cases in the LUserInput.
    if (this->IsShowMouseCursor())
    {
        check( this->LastMouseLocation.has_value() == false )
        return;
    }

    if (   algo::contains(this->GetCurrentlyPressedKeys(), EKeys::MouseX, &LRawInput::Key)
        || algo::contains(this->GetCurrentlyPressedKeys(), EKeys::MouseY, &LRawInput::Key))
    {
        return;
    }

    if (this->LastMouseLocation.has_value() == false)
    {
        this->LastMouseLocation = LVector2D{XPos, YPos};
        return;
    }

    const LVector2D Offset{XPos - this->LastMouseLocation->X, this->LastMouseLocation->Y - YPos};
    this->LastMouseLocation = LVector2D{XPos, YPos};

    this->AddKeyDown(EKeys::MouseX, static_cast<f32>(Offset.Y));
    this->AddKeyDown(EKeys::MouseY, static_cast<f32>(Offset.X));

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
    this->LastMouseLocation.reset();

    if (Entered == GLFW_TRUE)
    {
        this->bIsMouseInsideSurface = true;
    }
    else
    {
        this->bIsMouseInsideSurface = false;
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

void Jafg::LSurfaceGlfw3::Vk_CreateCommandPool()
{
    LOG_VERBOSE(LogVulkan, "Creating command pool for surface.")

    vk::CommandPoolCreateInfo PoolInfo{
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = this->GetFrontend().GetVkGraphicsQueueFamilyIndex()
        };

    this->Vk_CommandPool = vk::raii::CommandPool{this->GetFrontend().GetVkDevice(), PoolInfo};

    return;
}

void Jafg::LSurfaceGlfw3::Vk_CreateSwapchain()
{
    STAT_CYCLE_FUNCTION()

    LOG_VERBOSE(LogVulkan, "Creating Vulkan swapchain for Glfw3 surface.")

    auto& Frontend{ this->GetFrontend() };

    {
        LIntVector2 _FramebufferSize;
        glfwGetFramebufferSize(this->Handle, &_FramebufferSize.X, &_FramebufferSize.Y);
        if (_FramebufferSize.X == 0 || _FramebufferSize.Y == 0)
        {
            LOG_VERBOSE(LogSurface, "Waiting for non-zero dimensions to recreate swapchain.")
            STAT_QUICK_CYCLE_START("GlfwWaitEventsForNonZeroFramebufferSize")
            while (_FramebufferSize.X == 0 || _FramebufferSize.Y == 0)
            {
                glfwWaitEvents();
                glfwGetFramebufferSize(this->Handle, &_FramebufferSize.X, &_FramebufferSize.Y);
                continue;
            }
        }
    }

    {
        STAT_QUICK_CYCLE_START("VkDevice.waitIdle")
        Frontend.GetVkDevice().waitIdle();
    }

    this->Vk_SwapchainImageViews.clear();
    this->Vk_SwapchainImages.clear();
    this->Vk_VkMySwapchain = nullptr;
    for (auto& PresentSemaphore : this->Vk_ImageAvailableSemaphores) { PresentSemaphore = nullptr; }
    for (auto& RenderSemaphore : this->Vk_RenderSemaphores) { RenderSemaphore = nullptr; }
    for (auto& FlightFence : this->Vk_FlightFences) { FlightFence = nullptr; }

    this->Vk_SurfaceCapabilities = Frontend.GetVkPhysicalDevice().getSurfaceCapabilitiesKHR(this->Vk_Surface);
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

    this->Vk_AvailableSurfaceFormats = Frontend.GetVkPhysicalDevice().getSurfaceFormatsKHR(this->Vk_Surface);
    LOG_VERBOSE(LogVulkan, "Available surface formats:")
    for (auto const& SurfaceFormat : this->Vk_AvailableSurfaceFormats)
    {
        LOG_VERBOSE(LogVulkan, "    Format [{}], Color Space [{}]",
            vk::to_string(SurfaceFormat.format),
            vk::to_string(SurfaceFormat.colorSpace)
            )
    }

    this->Vk_AvailablePresentModes = Frontend.GetVkPhysicalDevice().getSurfacePresentModesKHR(this->Vk_Surface);
    LOG_VERBOSE(LogVulkan, "Available present modes:")
    for (auto const& PresentMode : this->Vk_AvailablePresentModes)
    {
        LOG_VERBOSE(LogVulkan, "    Present Mode [{}]", vk::to_string(PresentMode))
    }

    {
        auto AvailableFormat{this->Vk_GetSwapchainSurfaceFormatKHR(this->Vk_AvailableSurfaceFormats, this->Vk_DesiredSurfaceFormat)};
        if (AvailableFormat.has_value() == false)
        {
            /* TODO: Is this even a fatal error?? Should we just use a non-optimal format then?! */
            panicMsgf(
                "Desired surface format [format={}, colorSpace={}] is not available on the current platform.",
                vk::to_string(this->Vk_DesiredSurfaceFormat.format),
                vk::to_string(this->Vk_DesiredSurfaceFormat.colorSpace)
                )
        }
        this->Vk_SurfaceFormat = AvailableFormat.value();
    }

    {
        auto AvailablePresentMode{this->Vk_GetSwapchainPresentModeKHR(this->Vk_AvailablePresentModes, this->Vk_DesiredPresentMode)};
        if (AvailablePresentMode.has_value() == false)
        {
            // We may always use FIFO as the std guarantees its existence.
            // https://docs.vulkan.org/refpages/latest/refpages/source/VkPresentModeKHR.html
            LOG_WARNING(LogVulkan,
                "Desired present mode [{}] is not available on the current platform. Falling back to FIFO.",
                vk::to_string(this->Vk_DesiredPresentMode)
                )
            this->Vk_PresentMode = vk::PresentModeKHR::eFifo;
        }
        else
        {
            this->Vk_PresentMode = AvailablePresentMode.value();
        }
    }

    {
        glfwGetWindowFrameSize(this->Handle
            , &this->WindowFrameSizeTopLeft.X, &this->WindowFrameSizeTopLeft.Y
            , &this->WindowFrameSizeBottomRight.X, &this->WindowFrameSizeBottomRight.Y);
        glfwGetWindowSize(this->Handle, &this->WindowSize.X, &this->WindowSize.Y);
        glfwGetFramebufferSize(this->Handle, &this->FramebufferSize.X, &this->FramebufferSize.Y);
        if (this->Vk_SurfaceCapabilities.currentExtent.width
            == std::numeric_limits<decltype(this->Vk_SurfaceCapabilities.currentExtent.width)>::max())
        {
            this->Vk_SwapchainExtent = {
                Maths::Clamp<decltype(vk::Extent2D::width)>(this->FramebufferSize.X, this->Vk_SurfaceCapabilities.minImageExtent.width, this->Vk_SurfaceCapabilities.maxImageExtent.width),
                Maths::Clamp<decltype(vk::Extent2D::height)>(this->FramebufferSize.Y, this->Vk_SurfaceCapabilities.minImageExtent.height, this->Vk_SurfaceCapabilities.maxImageExtent.height)
                };
        }
    }

    LOG_VERBOSE(LogVulkan, "Using surface format [format={}, colorSpace={}].",
        vk::to_string(this->Vk_SurfaceFormat.format), vk::to_string(this->Vk_SurfaceFormat.colorSpace)
        )
    LOG_VERBOSE(LogVulkan, "Using present mode [{}].", vk::to_string(this->Vk_PresentMode))
    LOG_VERBOSE(LogVulkan, "Using swapchain extent [{}x{}].", this->Vk_SwapchainExtent.width, this->Vk_SwapchainExtent.height)
    if (   (static_cast<u32>(this->FramebufferSize.X) != this->Vk_SwapchainExtent.width)
        || (static_cast<u32>(this->FramebufferSize.Y) != this->Vk_SwapchainExtent.height))
    {
        LOG_WARNING(LogVulkan,
            "Framebuffer size [{}x{}] does not match clamped swapchain extent [{}x{}].",
            this->FramebufferSize.X, this->FramebufferSize.Y,
            this->Vk_SwapchainExtent.width, this->Vk_SwapchainExtent.height
            )
    }

    vk::SwapchainCreateInfoKHR SwapChainCreateInfo{
        .flags = vk::SwapchainCreateFlagsKHR{},
        .surface = this->Vk_Surface,
        .minImageCount = Maths::Clamp<u32>(Jafg::Vk_DesiredMaxFramesInFlight, this->Vk_SurfaceCapabilities.minImageCount, this->Vk_SurfaceCapabilities.maxImageCount),
        .imageFormat = this->Vk_SurfaceFormat.format,
        .imageColorSpace = this->Vk_SurfaceFormat.colorSpace,
        .imageExtent =  this->Vk_SwapchainExtent,
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

    const u32 QueueFamilyIndices[] {Frontend.GetVkGraphicsQueueFamilyIndex(), Frontend.GetVkPresentQueueFamilyIndex()};
    if (Frontend.GetVkGraphicsQueue() == Frontend.GetVkPresentQueue())
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

    this->Vk_VkMySwapchain = vk::raii::SwapchainKHR{Frontend.GetVkDevice(), SwapChainCreateInfo};
    {
        for (const auto Images{ this->Vk_VkMySwapchain.getImages() }; auto Image : Images)
        {
            this->Vk_SwapchainImages.push_back(Image);
        }
        LOG_VERBOSE(LogVulkan, "Created swapchain with [{}] images.", this->Vk_SwapchainImages.size())
    }

    this->bPendingResize = false;
    this->PendingResizeExtent = LUIntVector2::ZeroVector;
    this->PendingTimeForResizeApply = 0.0f;

    this->__Vk_CreateImageViews();
    this->__Vk_CreateColorResources();
    this->__Vk_CreateDepthResources();
    this->__Vk_CreateSynchObjects();

    return;
}

TOptional<vk::SurfaceFormatKHR> Jafg::LSurfaceGlfw3::Vk_GetSwapchainSurfaceFormatKHR(std::vector<vk::SurfaceFormatKHR> const& AvailableFormats, vk::SurfaceFormatKHR DesiredSurfaceFormat)
{
    for (auto const& AvailableFormat : AvailableFormats)
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

TOptional<vk::PresentModeKHR> Jafg::LSurfaceGlfw3::Vk_GetSwapchainPresentModeKHR(std::vector<vk::PresentModeKHR> const& AvailablePresentModes, vk::PresentModeKHR DesiredPresentMode)
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

void Jafg::LSurfaceGlfw3::__Vk_CreateImageViews()
{
    LOG_VERBOSE(LogVulkan, "Creating image views from swapchain images from surface [{}].", this->GetHumanReadableName())

    auto& Frontend{ this->GetFrontend() };

    check( this->Vk_SwapchainImageViews.empty() )

    for (auto const& SwapchainImage : this->Vk_SwapchainImages)
    {
        this->Vk_SwapchainImageViews.emplace_back(Frontend.CreateImageView({
            .image = SwapchainImage,
            .viewType = vk::ImageViewType::e2D,
            .format = this->Vk_SurfaceFormat.format,
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

    return;
}

void Jafg::LSurfaceGlfw3::__Vk_CreateColorResources()
{
    LOG_VERBOSE(LogVulkan, "Creating color resources for surface [{}].", this->GetHumanReadableName())

    auto& Frontend{ this->GetFrontend() };

    vk::ImageCreateInfo ImageCreateInfo{
        .imageType = vk::ImageType::e2D,
        .format = this->Vk_SurfaceFormat.format,
        .extent = vk::Extent3D{ this->Vk_SwapchainExtent.width, this->Vk_SwapchainExtent.height, 1 },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = Frontend.GetMaxMsaaSamples(),
        .tiling = vk::ImageTiling::eOptimal,
        .usage = vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
        .sharingMode = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
        };
    VmaAllocationCreateInfo AllocationCreateInfo{
        .usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        };

    this->Vk_ColorImage = Frontend.VkCreateImage(ImageCreateInfo, AllocationCreateInfo);
    this->Vk_ColorImageView = Frontend.CreateImageView2D(this->Vk_ColorImage.Buffer, this->Vk_SurfaceFormat.format, vk::ImageAspectFlagBits::eColor, 1);

    return;
}

void Jafg::LSurfaceGlfw3::__Vk_CreateDepthResources()
{
    LOG_VERBOSE(LogVulkan, "Creating depth resources for surface [{}].", this->GetHumanReadableName())

    auto& Frontend{ this->GetFrontend() };
    const vk::Format DepthFormat{ Frontend.FindDepthFormat() };

    this->Vk_DepthImage = Frontend.VkCreateDeviceLocalImage(
        vk::ImageCreateInfo{
            .imageType = vk::ImageType::e2D,
            .format = DepthFormat,
            .extent = vk::Extent3D{ this->Vk_SwapchainExtent.width, this->Vk_SwapchainExtent.height, 1 },
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = Frontend.GetMaxMsaaSamples(),
            .tiling = vk::ImageTiling::eOptimal,
            .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
            .sharingMode = vk::SharingMode::eExclusive,
            .initialLayout = vk::ImageLayout::eUndefined,
            }
        );

    this->Vk_DepthImageView = Frontend.CreateImageView2D(this->Vk_DepthImage.Buffer, DepthFormat, vk::ImageAspectFlagBits::eDepth, 1);

    return;
}

void Jafg::LSurfaceGlfw3::__Vk_CreateSynchObjects()
{
    LOG_VERBOSE(LogVulkan, "Creating [{}+2*{}] vulkan synchronization objects for surface [{}].",
        this->Vk_GetNumberOfFramesInFlight(), this->Vk_SwapchainImages.size(), this->GetHumanReadableName()
        )

    auto& Device{ this->GetFrontend().GetVkDevice() };

    for (auto Idx{0uz}; Idx < this->Vk_GetNumberOfFramesInFlight(); ++Idx)
    {
        this->Vk_ImageAvailableSemaphores[Idx] = vk::raii::Semaphore{Device, vk::SemaphoreCreateInfo{}};
        this->Vk_RenderSemaphores[Idx] = vk::raii::Semaphore{Device, vk::SemaphoreCreateInfo{}};

        this->Vk_FlightFences[Idx] = vk::raii::Fence{Device, vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled }};

        continue;
    }

    this->Vk_LastFrameInFlightIndex = 0;
    check( this->Vk_CurrentFrameInFlightIndex.has_value() == false )

    return;
}

void Jafg::LSurfaceGlfw3::Vk_CreateCommandBuffers()
{
    LOG_VERBOSE(LogVulkan, "Creating [{}] vulkan command buffers for surface.", this->Vk_GetNumberOfFramesInFlight())

    vk::CommandBufferAllocateInfo Info{
        .commandPool = this->Vk_CommandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = static_cast<uint32_t>(this->Vk_GetNumberOfFramesInFlight())
        };

    auto CommandBuffers{ vk::raii::CommandBuffers(this->GetFrontend().GetVkDevice(), Info) };
    check( this->Vk_CommandBuffers.size() == CommandBuffers.size() )
    for (auto Idx{ 0uz }; Idx < CommandBuffers.size(); ++Idx)
    {
        this->Vk_CommandBuffers[Idx] = std::move(CommandBuffers[Idx]);
    }

    return;
}







static void TestPipeline(Jafg::LSurface const& Surface)
{
    using namespace Jafg;
    LOG_VERBOSE(LogVulkan, "Creating test pipeline...")

    auto& Frontend{ Surface.GetFrontend() };

    VkTestPipeline = LDevicePipelineFactory{Surface}
        .Shader("Content/Shaders/Spir-V/VisualBox.spv",
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
        .VertexInput<LRhiVertex2D>()
        .Build();

    VkTestVertexBuffer = Frontend.Vk_StageBuffer(LStageBufferCreateInfo::Vertex({
        .BufferCopy = vk::BufferCopy{0, 0, sizeof(QuadVertices[0]) * QuadVertices.size()},
        .Data = QuadVertices.data()
        }));
    VkTestIndexBuffer = Frontend.Vk_StageBuffer(LStageBufferCreateInfo::Index({
        .BufferCopy = vk::BufferCopy{0, 0, sizeof(QuadIndices[0]) * QuadIndices.size()},
        .Data = QuadIndices.data()
        }));

    return;
}

void Jafg::LSurfaceGlfw3::VkCreateTextureImage()
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan texture image for surface.")

    auto& Frontend = this->GetFrontend();

    int texWidth, texHeight, _;
    // stbi_uc* pixels = stbi_load("Content/Textures/statue-1275469.jpg", &texWidth, &texHeight, &_, STBI_rgb_alpha);
    stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &_, STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        char const* error = stbi_failure_reason();
        panicMsgf("Failed to load texture image [Content/Textures/statue-1275469.jpg]. Reason: {}", error)
    }

    this->MipLevels = static_cast<u32>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
    LOG_VERBOSE(LogVulkan, "Texture image will have [{}] mip levels.", this->MipLevels)

    auto StatingBuffer = Frontend.Vk_CreateMappedBuffer(
        vk::BufferCreateInfo{
            .size = imageSize,
            .usage = vk::BufferUsageFlagBits::eTransferSrc,
            },
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        );

    std::memcpy(StatingBuffer.Data, pixels, static_cast<size_t>(imageSize));
    stbi_image_free(pixels);

    vk::ImageCreateInfo TextureImageCreateInfo{
        .flags = {},
        .imageType = vk::ImageType::e2D,
        .format = vk::Format::eR8G8B8A8Srgb,
        .extent = vk::Extent3D{ static_cast<u32>(texWidth), static_cast<u32>(texHeight), 1 },
        .mipLevels = MipLevels,
        .arrayLayers = 1,
        .samples = vk::SampleCountFlagBits::e1,
        .tiling = vk::ImageTiling::eOptimal,
        .usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        .sharingMode = vk::SharingMode::eExclusive,
        .initialLayout = vk::ImageLayout::eUndefined,
        };
    VmaAllocationCreateInfo TextureImageAllocationCreateInfo{
        .usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        };
    this->TextureImage = Frontend.VkCreateImage(TextureImageCreateInfo, TextureImageAllocationCreateInfo);

    Frontend.VkTransitionImageLayout(this->TextureImage.Buffer,
        vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, MipLevels, this->Vk_CommandPool);
    Frontend.CopyBufferToImage(
        StatingBuffer.Buffer,
        this->TextureImage.Buffer,
        static_cast<u32>(texWidth),
        static_cast<u32>(texHeight),
        this->Vk_CommandPool
        );

    this->VkGenerateMipMaps(
        this->TextureImage.Buffer, vk::Format::eR8G8B8A8Srgb, texWidth, texHeight, this->MipLevels
        );

    // Frontend.VkTransitionImageLayout(this->TextureImage.Image, vk::ImageLayout::eTransferDstOptimal,
    //      vk::ImageLayout::eShaderReadOnlyOptimal, MipLevels, this->VkMyCommandPool);

    this->TextureImageView = this->GetFrontend().CreateImageView2D(this->TextureImage.Buffer,
        vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor, this->MipLevels);

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
        .minLod = 0.0f, // increase for worse texture quality
        .maxLod = VK_LOD_CLAMP_NONE,
        .borderColor = vk::BorderColor::eIntOpaqueBlack,
        };

    this->TextureSampler = vk::raii::Sampler(Frontend.GetVkDevice(), SamplerInfo);

    return;
}

void Jafg::LSurfaceGlfw3::VkCreateUniformBuffers()
{
    LOG_VERBOSE(LogTemporal, "Creating Vulkan uniform buffers for surface.")

    this->UniformBuffers.clear();

    auto& Frontend = this->GetFrontend();

    for (LSize Idx{ 0 }; Idx < this->Vk_GetNumberOfFramesInFlight(); ++Idx)
    {
        this->UniformBuffers.emplace_back(Frontend.Vk_CreateMappedBuffer(vk::BufferCreateInfo{
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
        vk::DescriptorPoolSize( vk::DescriptorType::eUniformBuffer, this->Vk_GetNumberOfFramesInFlight()),
        vk::DescriptorPoolSize( vk::DescriptorType::eCombinedImageSampler, this->Vk_GetNumberOfFramesInFlight())
        };

    vk::DescriptorPoolCreateInfo poolInfo{
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = static_cast<uint32_t>(this->Vk_GetNumberOfFramesInFlight()),
        .poolSizeCount = static_cast<uint32_t>(PoolSize.size()),
        .pPoolSizes = PoolSize.data()
        };

    this->VkMyDescriptorPool = vk::raii::DescriptorPool(this->GetFrontend().GetVkDevice(), poolInfo);

    return;
}

void Jafg::LSurfaceGlfw3::VkCreateDescriptorSets()
{
    LOG_VERBOSE(LogVulkan, "Creating Vulkan descriptor sets for surface.")

    auto& Frontend = this->GetFrontend();

    std::vector<vk::DescriptorSetLayout> layouts(this->Vk_GetNumberOfFramesInFlight(), *VkTestMeshPipeline.DescriptorSetLayout);
    vk::DescriptorSetAllocateInfo        allocInfo{.descriptorPool = this->VkMyDescriptorPool,
        .descriptorSetCount = static_cast<uint32_t>(layouts.size()), .pSetLayouts = layouts.data()};

    this->VkDescriptorSets = Frontend.GetVkDevice().allocateDescriptorSets(allocInfo);

    for (size_t i = 0; i < this->Vk_GetNumberOfFramesInFlight(); i++)
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
    ubo.proj  = glm::perspective(glm::radians(45.0f), static_cast<float>(this->Vk_SwapchainExtent.width) / static_cast<float>(this->Vk_SwapchainExtent.height), 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    std::memcpy(this->UniformBuffers[currentImage].Data, &ubo, sizeof(ubo));

    return;
}

void Jafg::LSurfaceGlfw3::VkGenerateMipMaps(vk::Image Image, vk::Format Format, i32 Width, i32 Height, u32 _MipLevels)
{
    auto& Frontend = this->GetFrontend();

    vk::FormatProperties formatProperties = Frontend.GetVkPhysicalDevice().getFormatProperties(Format);
    if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
    {
        panic( "texture image format does not support linear blitting. ")
    }

    vk::raii::CommandBuffer commandBuffer = Frontend.VkBeginSingleTimeCommands(this->Vk_CommandPool);

    vk::ImageMemoryBarrier barrier          = {.srcAccessMask = vk::AccessFlagBits::eTransferWrite,
        .dstAccessMask = vk::AccessFlagBits::eTransferRead, .oldLayout = vk::ImageLayout::eTransferDstOptimal,
        .newLayout = vk::ImageLayout::eTransferSrcOptimal, .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
        .dstQueueFamilyIndex = vk::QueueFamilyIgnored, .image = Image};
    barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.subresourceRange.levelCount     = 1;

    int32_t mipWidth  = Width;
    int32_t mipHeight = Height;

    for (uint32_t i = 1; i < _MipLevels; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout                     = vk::ImageLayout::eTransferSrcOptimal;
        barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask                 = vk::AccessFlagBits::eTransferRead;

        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, barrier);

        vk::ArrayWrapper1D<vk::Offset3D, 2> offsets, dstOffsets;
        offsets[0]          = vk::Offset3D(0, 0, 0);
        offsets[1]          = vk::Offset3D(mipWidth, mipHeight, 1);
        dstOffsets[0]       = vk::Offset3D(0, 0, 0);
        dstOffsets[1]       = vk::Offset3D(mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1);
        vk::ImageBlit blit  = {.srcSubresource = {}, .srcOffsets = offsets, .dstSubresource = {}, .dstOffsets = dstOffsets};
        blit.srcSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i - 1, 0, 1);
        blit.dstSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i, 0, 1);

        commandBuffer.blitImage(Image, vk::ImageLayout::eTransferSrcOptimal, Image, vk::ImageLayout::eTransferDstOptimal, {blit}, vk::Filter::eLinear);

        barrier.oldLayout     = vk::ImageLayout::eTransferSrcOptimal;
        barrier.newLayout     = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barrier);

        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = _MipLevels - 1;
    barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout                     = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask                 = vk::AccessFlagBits::eShaderRead;

    commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barrier);

    Frontend.VkEndSingleTimeCommands(commandBuffer);

    return;
}

#endif /* JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */
