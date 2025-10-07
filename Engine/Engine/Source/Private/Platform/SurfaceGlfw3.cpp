// Copyright mzoesch. All rights reserved.

#if JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER

#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/UserPreferences.h"
#include "Engine/CoreGlobals.h"
#include "Engine/Engine.h"
#include "Async/TaskUtility.h"
#include "Rhi/RhiVendorInclude.h"
#include "GLFW/glfw3.h"
#if PLATFORM_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
#endif /* PLATFORM_WINDOWS */
#include "User/Input/GlfwInputTranslation.h"
#include "Widgets/Viewport.h"
#include "Stats/Stats.h"

namespace
{

bool bInitializedGlfw { false };

void OpenGlErrorCallback(int error_code, const char* description);

void OpenGlErrorCallback(int error_code, const char* description)
{
    panicMsgf( "OpenGl encountered an error [{}]: [{}].", error_code, description )
}

} /* ~Namespace <Anonymous> */

namespace Jafg::Private
{

struct LGlfw3Bridge final
{
    LGlfw3Bridge() = delete;
    PROHIBIT_REALLOC_OF_ANY_FORM(LGlfw3Bridge)
    ~LGlfw3Bridge() = delete;

    static void FramebufferSizeCallback(::GLFWwindow* Window, const i32 Width, const i32 Height);
    static void MouseCallback(::GLFWwindow* Window, const double XPos, const double YPos);
    static void ScrollCallback(::GLFWwindow* Window, const double XOffset, const double YOffset);
    static void MouseEnterCallback(::GLFWwindow* Window, const i32 Entered);
    static void CharCallback(::GLFWwindow* Window, const u32 Codepoint);
    static void KeyCallback(::GLFWwindow* Window, const i32 Key, const i32 Scancode, const i32 Action, const i32 Mods);
};

void LGlfw3Bridge::FramebufferSizeCallback(::GLFWwindow* Window, const i32 Width, const i32 Height)
{
    checkSlow( static_cast<::Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
    static_cast<Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->FramebufferSizeCallback(Width, Height);
    return;
}

void LGlfw3Bridge::MouseCallback(GLFWwindow* Window, const double XPos, const double YPos)
{
    checkSlow( static_cast<::Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
    static_cast<Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->MouseCallback(XPos, YPos);
    return;
}

void LGlfw3Bridge::ScrollCallback(GLFWwindow* Window, const double XOffset, const double YOffset)
{
    checkSlow( static_cast<::Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
    static_cast<Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->ScrollCallback(XOffset, YOffset);
    return;
}

void LGlfw3Bridge::MouseEnterCallback(GLFWwindow* Window, const i32 Entered)
{
    checkSlow( static_cast<::Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
    static_cast<Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->MouseEnterCallback(Entered);
    return;
}

void LGlfw3Bridge::CharCallback(GLFWwindow* Window, const u32 Codepoint)
{
    checkSlow( static_cast<::Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
    static_cast<Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->CharCallback(Codepoint);
    return;
}

void LGlfw3Bridge::KeyCallback(GLFWwindow* Window, const i32 Key, const i32 Scancode, const i32 Action, const i32 Mods)
{
    checkSlow( static_cast<::Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
    static_cast<Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->KeyCallback(Key, Scancode, Action, Mods);
    return;
}

} /* ~Namespace Jafg::Private */

Jafg::LSurfaceGlfw3::LSurfaceGlfw3(LSurfaceGlfw3&& Other) noexcept
{
    *this = std::move(Other);
}

Jafg::LSurfaceGlfw3& Jafg::LSurfaceGlfw3::operator=(LSurfaceGlfw3&& Other) noexcept
{
    Super::operator=(std::move(Other));

    this->Cursor = Other.Cursor;
    this->Handle = Other.Handle;
    this->bVSync = Other.bVSync;
    this->bFirstMouseCallback = Other.bFirstMouseCallback;
    this->LastMouseX = Other.LastMouseX;
    this->LastMouseY = Other.LastMouseY;

    glfwMakeContextCurrent(this->Handle);
    glfwSetWindowUserPointer(this->Handle, reinterpret_cast<void*>(this));

    Other.Cursor = nullptr;
    Other.Handle = nullptr;

    return *this;
}

Jafg::LSurfaceGlfw3::~LSurfaceGlfw3()
{
    if (this->Handle)
    {
        LSurfaceGlfw3::TearDown();
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
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

        const int Platform { glfwGetPlatform() };
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

    {
        STAT_QUICK_CYCLE_START("Glfw3WindowCreation")

        // Min 640 480
        this->Handle = glfwCreateWindow(1280, 720, "Jafg - mzoesch", nullptr, nullptr);
    }
    if (this->Handle == nullptr)
    {
        panic( "Failed to create glfw window." )
        return;
    }

    glfwMakeContextCurrent(this->Handle);
    glfwSetWindowUserPointer(this->Handle, reinterpret_cast<void*>(this));

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        panic( "Failed to initialize glad." )
        return;
    }

    /*
     * We have to call this, as there is no default set by glfw. The default is open for the
     * implementer to decide. Not calling this method directly after initializing the window
     * will cause undefined behavior (There could be a mismatch between the context window and
     * the member variable DesktopPlatformWin#bVSync).
     *
     * @see Official GLFW Documentation:
     *      This function is not called during context creation, leaving the swap interval set to whatever is the
     *      default for that API. This is done because some swap interval extensions used by GLFW do not allow the
     *      swap interval to be reset to zero once it has been set to a non-zero value.
     *      Some GPU drivers do not honor the requested swap interval, either because of a user setting that
     *      overrides the application's request or due to bugs in the driver.
     */
    glfwSwapInterval(0);
    this->bVSync = false;

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

    this->GetViewport().SetPlatformDpi(static_cast<float>(PlatformDpi));

    const LIntVector2 WindowDimensions = this->GetDimensions();
    glViewport(0, 0, WindowDimensions.X, WindowDimensions.Y);
    this->GetViewport().ChangeDimensions(WindowDimensions);

    glfwSetErrorCallback(::OpenGlErrorCallback);
    glfwSetFramebufferSizeCallback(this->Handle, Private::LGlfw3Bridge::FramebufferSizeCallback);
    glfwSetCursorPosCallback(this->Handle, Private::LGlfw3Bridge::MouseCallback);
    glfwSetScrollCallback(this->Handle, Private::LGlfw3Bridge::ScrollCallback);
    glfwSetCursorEnterCallback(this->Handle, Private::LGlfw3Bridge::MouseEnterCallback);

    /*
     * Move this into the if statement later, when we have a native way, to read input reliable.
     */
    glfwSetCharCallback(this->Handle, Private::LGlfw3Bridge::CharCallback);
    if (this->IsPlatformSupportsRepeatedKey())
    {
        glfwSetKeyCallback(this->Handle, Private::LGlfw3Bridge::KeyCallback);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    return;
}

void Jafg::LSurfaceGlfw3::OnClear()
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    glfwMakeContextCurrent(this->Handle);

    Super::OnClear();

    return;
}

void Jafg::LSurfaceGlfw3::OnUpdate()
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    glfwMakeContextCurrent(this->Handle);

    Super::OnUpdate();

    glfwSwapBuffers(this->Handle);

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

    if (IsEngineExitRequested() && GEngine->GetLocalEgo()->GetFrontend()->GetSurfaceCount() == 0)
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
    glfwMakeContextCurrent(this->Handle);

    LKey KeyCursor = EKeys::A;
    while (KeyCursor <= EKeys::LastKey)
    {
        const i32 TranslatedKey = Glfw3::TranslateKeyToGlfw(KeyCursor);
        if (TranslatedKey == INDEX_NONE)
        {
            ++KeyCursor;
            continue;
        }

        if (glfwGetKey(this->Handle, TranslatedKey) == GLFW_PRESS)
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

    if (glfwGetMouseButton(this->Handle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        this->AddKeyDown(EKeys::LeftMouseButton);
    }
    if (glfwGetMouseButton(this->Handle, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        this->AddKeyDown(EKeys::RightMouseButton);
    }
    if (glfwGetMouseButton(this->Handle, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        this->AddKeyDown(EKeys::MiddleMouseButton);
    }

    return;
}

void Jafg::LSurfaceGlfw3::PollEvents()
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    glfwMakeContextCurrent(this->Handle);

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

    glfwMakeContextCurrent(this->Handle);
    glfwSetInputMode(this->Handle, GLFW_CURSOR, this->bShowCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

    return;
}

void Jafg::LSurfaceGlfw3::SetMouseCursor(const EMouseCursor::Type InCursor)
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    glfwMakeContextCurrent(this->Handle);

    if (this->Cursor)
    {
        glfwDestroyCursor(this->Cursor);
        this->Cursor = nullptr;
    }

    if (InCursor == EMouseCursor::Default)
    {
        check( this->Cursor == nullptr )
    }
    else if (InCursor == EMouseCursor::Arrow)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    }
    else if (InCursor == EMouseCursor::Beam)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    }
    else if (InCursor == EMouseCursor::Crosshair)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    }
    else if (InCursor == EMouseCursor::Hand)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    }
    else if (InCursor == EMouseCursor::ResizeNS)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);
    }
    else if (InCursor == EMouseCursor::ResizeEW)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);
    }
    else if (InCursor == EMouseCursor::ResizeNESW)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
    }
    else if (InCursor == EMouseCursor::ResizeNWSE)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    }
    else if (InCursor == EMouseCursor::ResizeOmni)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
    }
    else if (InCursor == EMouseCursor::NotAllowed)
    {
        this->Cursor = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
    }
    else
    {
        LOG_WARNING(LogSystem, "Unknown cursor type {}[{}].", static_cast<i32>(InCursor), LexToString(InCursor))
    }

    if (InCursor != EMouseCursor::None)
    {
        glfwSetCursor(this->Handle, this->Cursor);
    }

    return;
}

LIntVector2 Jafg::LSurfaceGlfw3::GetDimensions() const
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    glfwMakeContextCurrent(this->Handle);

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

    glfwMakeContextCurrent(this->Handle);
    glfwSwapInterval(this->bVSync ? 1 : 0);

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
    this->PendingTimeForResizeApply = 1.5f;

    return;
}

void Jafg::LSurfaceGlfw3::FramebufferSizeCallbackImpl(const i32 Width, const i32 Height)
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    glfwMakeContextCurrent(this->Handle);

    glViewport(0, 0, Width, Height);
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
