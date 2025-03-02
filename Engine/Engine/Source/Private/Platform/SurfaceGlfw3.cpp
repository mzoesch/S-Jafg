// Copyright mzoesch. All rights reserved.

#include "User/LocalEgo.h"
#if PLATFORM_USES_GLFW3_ABSTRACTION_LAYER

#include "Platform/Surface.h"
#include "User/UserPreferences.h"
#include "Engine/CoreGlobals.h"
#include "Engine/Engine.h"
#include "Async/TaskUtility.h"
#include "Rhi/RhiVendorInclude.h"
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "User/Input/GlfwInputTranslation.h"
#include "Widgets/Viewport.h"

namespace
{

bool bInitializedGlfw = false;

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

    static void FramebufferSizeCallback(::GLFWwindow* Window, const int32 Width, const int32 Height);
    static void MouseCallback(::GLFWwindow* Window, const double XPos, const double YPos);
    static void ScrollCallback(::GLFWwindow* Window, const double XOffset, const double YOffset);
    static void MouseEnterCallback(::GLFWwindow* Window, const int32 Entered);
    static void CharCallback(::GLFWwindow* Window, const uint32 Codepoint);
    static void KeyCallback(::GLFWwindow* Window, const int32 Key, const int32 Scancode, const int32 Action, const int32 Mods);
};

void LGlfw3Bridge::FramebufferSizeCallback(::GLFWwindow* Window, const int32 Width, const int32 Height)
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

void LGlfw3Bridge::MouseEnterCallback(GLFWwindow* Window, const int32 Entered)
{
    checkSlow( static_cast<::Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
    static_cast<Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->MouseEnterCallback(Entered);
    return;
}

void LGlfw3Bridge::CharCallback(GLFWwindow* Window, const uint32 Codepoint)
{
    checkSlow( static_cast<::Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->GetNativeHandleDangerous() == Window )
    static_cast<Jafg::LSurfaceGlfw3*>(glfwGetWindowUserPointer(Window))->CharCallback(Codepoint);
    return;
}

void LGlfw3Bridge::KeyCallback(GLFWwindow* Window, const int32 Key, const int32 Scancode, const int32 Action, const int32 Mods)
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
    Super::Initialize();

    check( Tasks::IsOnMasterThread() )

    if (bInitializedGlfw == false)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        bInitializedGlfw = true;
    }

    this->Handle = glfwCreateWindow(1280, 720, "Jafg - mzoesch", nullptr, nullptr);
    if (this->Handle == nullptr)
    {
        panic( "Failed to create glfw window." )
        return;
    }

    glfwMakeContextCurrent(this->Handle);
    glfwSetWindowUserPointer(this->Handle, reinterpret_cast<void*>(this));

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) // NOLINT(clang-diagnostic-cast-function-type-strict)
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
    const uint32 PlatformDpi = ::GetDpiForWindow(NativeWindowHandle);
#else /* PLATFORM_WINDOWS */
    const uint32 PlatformDpi = 96; // Sketchy
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
    glfwSetCharCallback(this->Handle, Private::LGlfw3Bridge::CharCallback);
    glfwSetKeyCallback(this->Handle, Private::LGlfw3Bridge::KeyCallback);

    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);

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

void Jafg::LSurfaceGlfw3::PollInputs()
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    glfwMakeContextCurrent(this->Handle);

    LKey KeyCursor = EKeys::A;
    while (KeyCursor <= EKeys::LastKey)
    {
        const int32 TranslatedKey = Glfw3::TranslateKeyToGlfw(KeyCursor);
        if (TranslatedKey == INDEX_NONE)
        {
            ++KeyCursor;
            continue;
        }

        if (glfwGetKey(this->Handle, TranslatedKey) == GLFW_PRESS)
        {
            this->AddKeyDown(KeyCursor);
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

    glfwMakeContextCurrent( this->Handle );
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
        LOG_WARNING(LogSystem, "Unknown cursor type {}[{}].", static_cast<int32>(InCursor), LexToString(InCursor))
    }

    if (InCursor != EMouseCursor::None)
    {
        glfwSetCursor(this->Handle, this->Cursor);
    }

    return;
}

Jafg::TIntVector2<int> Jafg::LSurfaceGlfw3::GetDimensions() const
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    glfwMakeContextCurrent(this->Handle);

    /*
     * Do we want to cache this value?
     * How long does it take to get the window size?
     */
    int32 Width, Height;
    glfwGetWindowSize(this->Handle, &Width, &Height);
    return LIntVector2(Width, Height);
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

void Jafg::LSurfaceGlfw3::FramebufferSizeCallback(const int32 Width, const int32 Height)
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

    if (this->GetCurrentlyPressedKeys().Contains(EKeys::MouseX))
    {
        return;
    }
    if (this->GetCurrentlyPressedKeys().Contains(EKeys::MouseY))
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
    if (this->GetCurrentlyPressedKeys().Contains(EKeys::MouseWheelAxis))
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

void Jafg::LSurfaceGlfw3::MouseEnterCallback(const int32 Entered)
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

void Jafg::LSurfaceGlfw3::CharCallback(const uint32 Codepoint)
{
    std::u32string Char;
    Char.push_back(Codepoint);

#if WITH_MSVC
    #pragma warning( push )
    #pragma warning(disable: 4996)
#endif /* WITH_MSVC */
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::string utf8String = converter.to_bytes(Char);
#if WITH_MSVC
    #pragma warning( pop )
#endif /* WITH_MSVC */

    this->AddBufferedPlatformInput(utf8String.c_str());

    return;
}

void Jafg::LSurfaceGlfw3::KeyCallback(const int32 Key, const int32 Scancode, const int32 Action, const int32 Mods)
{
    if (Action == GLFW_REPEAT)
    {
        const LKey TranslatedKey = Glfw3::TranslateKeyFromGlfw(Key);
        if (TranslatedKey != EKeys::Unresolved)
        {
            this->SetRepeatedKeyDown(TranslatedKey);
        }
    }

    return;
}

#endif /* PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */
