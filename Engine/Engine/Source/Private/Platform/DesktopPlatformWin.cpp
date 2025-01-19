// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"

#if PLATFORM_WINDOWS

#include "Forward/EngineForward.h"
#include "Platform/DesktopPlatformWin.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.inl>
#include "Rhi/Shader.h"
#include <glm/glm.hpp>
#include "Rhi/RendererApplier.h"
#include "Widgets/Viewport.h"
#include "User/Input/GlfwInputTranslation.h"
#include "Core/LaunchProgress.h"

namespace
{

void OpenGlErrorCallback(int error_code, const char* description)
{
    panic( "OpenGl encountered an error." )
}

} /* ~Namespace <Anonymous> */

void Jafg::LDesktopPlatformWin::Initialize()
{
    LDesktopPlatformBase::Initialize();

    if (LaunchProgress::Private::GProgressWindow)
    {
        LaunchProgress::Private::bOwnerShipToken = true;
        this->MasterWindow = LaunchProgress::Private::GProgressWindow;
    }
    else
    {
        if (RendererApplier::IsGlfwInitialized() == false)
        {
            const bool bOk = RendererApplier::InitializeGlfw();
            if (bOk == false)
            {
                JAFG_ENGINE_FORWARD_REQUEST_EXIT(EPlatformExit::Fatal, "Failed to initialize glfw window.")
                return;
            }
        }
        this->MasterWindow = LDesktopPlatformWin::CreateNativeWindow(LDesktopSurfaceProps());
    }

    // glfwInit();
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // this->MasterWindow = LDesktopPlatformWin::CreateNativeWindow(LDesktopSurfaceProps());
    if (this->MasterWindow == nullptr)
    {
        JAFG_ENGINE_FORWARD_REQUEST_EXIT(EPlatformExit::Fatal, "Failed to initialize glfw window.")
        return;
    }
    glfwSetWindowUserPointer(this->MasterWindow->GetNativeWindow(), reinterpret_cast<void*>(this));

    check( this->GetViewport() )
    const HWND NativeWindowHandle = glfwGetWin32Window(this->MasterWindow->GetNativeWindow());
    check( NativeWindowHandle )
    const uint32 PlatformDpi = GetDpiForWindow(NativeWindowHandle);
    this->GetViewport()->SetPlatformDpi(static_cast<float>(PlatformDpi));

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
    this->SetVSync(false);

    // if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))  // NOLINT(clang-diagnostic-cast-function-type-strict)
    // {
    //     JAFG_ENGINE_FORWARD_REQUEST_EXIT(EPlatformExit::Fatal, "Failed to initialize glad.")
    //     return;
    // }

    const TIntVector2 WindowDimensions = this->GetDimensions();
    glViewport(0, 0, WindowDimensions.X, WindowDimensions.Y);

    if (this->GetViewport())
    {
        this->GetViewport()->ChangeDimensions(WindowDimensions);
    }

    glfwSetFramebufferSizeCallback(this->MasterWindow->GetNativeWindow(), [] (::GLFWwindow* Window, const int32 Width, const int32 Height)
    {
        static_cast<LDesktopPlatformWin*>(glfwGetWindowUserPointer(Window))->FramebufferSizeCallback(Window, Width, Height);
    });
    glfwSetCursorPosCallback(this->MasterWindow->GetNativeWindow(), [] (::GLFWwindow* Window, const double XPos, const double YPos)
    {
        static_cast<LDesktopPlatformWin*>(glfwGetWindowUserPointer(Window))->MouseCallback(Window, XPos, YPos);
    });
    glfwSetScrollCallback(this->MasterWindow->GetNativeWindow(), [] (::GLFWwindow* Window, const double XOffset, const double YOffset)
    {
        static_cast<LDesktopPlatformWin*>(glfwGetWindowUserPointer(Window))->ScrollCallback(Window, XOffset, YOffset);
    });
    glfwSetCursorEnterCallback(this->MasterWindow->GetNativeWindow(), [] (::GLFWwindow* Window, const int32 Entered)
    {
        static_cast<LDesktopPlatformWin*>(glfwGetWindowUserPointer(Window))->MouseEnterCallback(Window, Entered);
    });
    glfwSetCharCallback(this->MasterWindow->GetNativeWindow(), [] (::GLFWwindow* Window, const uint32 Codepoint)
    {
        static_cast<LDesktopPlatformWin*>(glfwGetWindowUserPointer(Window))->CharCallback(Window, Codepoint);
    });
    glfwSetKeyCallback(this->MasterWindow->GetNativeWindow(), [] (::GLFWwindow* Window, int32 Key, int32 Scancode, int32 Action, int32 Mods)
    {
        static_cast<LDesktopPlatformWin*>(glfwGetWindowUserPointer(Window))->KeyCallback(Window, Key, Scancode, Action, Mods);
    });

    glfwSetErrorCallback(::OpenGlErrorCallback);

    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    return;
}

void Jafg::LDesktopPlatformWin::OnClear()
{
    LDesktopPlatformBase::OnClear();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return;
}

void RenderText(Jafg::LShader &shader, std::string text, float x, float y, float scale, glm::vec3 color);
void Jafg::LDesktopPlatformWin::OnUpdate()
{
    LDesktopPlatformBase::OnUpdate();

    glfwSwapBuffers(this->MasterWindow->GetNativeWindow());

    return;
}

void Jafg::LDesktopPlatformWin::TearDown()
{
    LDesktopPlatformBase::TearDown();

    if (this->MasterWindow)
    {
        glfwDestroyWindow(this->MasterWindow->GetNativeWindow());
        delete this->MasterWindow;
        this->MasterWindow = nullptr;
    }

    glfwTerminate();

    return;
}

void Jafg::LDesktopPlatformWin::PollInputs()
{
    LDesktopPlatformBase::PollInputs();

    LKey KeyCursor = EKeys::A;
    while (KeyCursor <= EKeys::LastKey)
    {
        const int32 TranslatedKey = Glfw3::TranslateKeyToGlfw(KeyCursor);
        if (TranslatedKey == INDEX_NONE)
        {
            ++KeyCursor;
            continue;
        }

        if (glfwGetKey(this->MasterWindow->GetNativeWindow(), TranslatedKey) == GLFW_PRESS)
        {
            this->AddKeyDown(KeyCursor);
        }

        ++KeyCursor;

        continue;
    }

    if (glfwGetMouseButton(this->MasterWindow->GetNativeWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        this->AddKeyDown(EKeys::LeftMouseButton);
    }
    if (glfwGetMouseButton(this->MasterWindow->GetNativeWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        this->AddKeyDown(EKeys::RightMouseButton);
    }
    if (glfwGetMouseButton(this->MasterWindow->GetNativeWindow(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        this->AddKeyDown(EKeys::MiddleMouseButton);
    }

    return;
}

void Jafg::LDesktopPlatformWin::PollEvents()
{
    LDesktopPlatformBase::PollEvents();

    if (this->MasterWindow)
    {
        if (glfwWindowShouldClose(this->MasterWindow->GetNativeWindow()))
        {
            JAFG_ENGINE_FORWARD_REQUEST_EXIT(INDEX_NONE, "Window closed by user.")
        }
    }

    glfwPollEvents();

    return;
}

void Jafg::LDesktopPlatformWin::SetInputMode(const EInputMode::Type InMode, const bool bInShowCursor)
{
    LDesktopPlatformBase::SetInputMode(InMode, bInShowCursor);

    if (this->bShowCursor)
    {
        this->bFirstMouseCallback = true;
    }

    if (this->MasterWindow)
    {
        glfwSetInputMode(this->MasterWindow->GetNativeWindow(), GLFW_CURSOR, this->bShowCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    return;
}

void Jafg::LDesktopPlatformWin::SetMouseCursor(const EMouseCursor::Type InCursor)
{
    LDesktopPlatformBase::SetMouseCursor(InCursor);

    if (this->MasterWindow == nullptr)
    {
        return;
    }

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
        glfwSetCursor(this->MasterWindow->GetNativeWindow(), this->Cursor);
    }

    return;
}

int32 Jafg::LDesktopPlatformWin::GetWidth() const
{
    return this->GetDimensions().X;
}

int32 Jafg::LDesktopPlatformWin::GetHeight() const
{
    return this->GetDimensions().Y;
}

Jafg::TIntVector2<int32> Jafg::LDesktopPlatformWin::GetDimensions() const
{
    /*
     * Do we want to cache this value?
     * How long does it take to get the window size?
     */

    int32 Width, Height;
    glfwGetWindowSize(this->MasterWindow->GetNativeWindow(), &Width, &Height);

    return TIntVector2<int32>(Width, Height);
}

bool Jafg::LDesktopPlatformWin::CanVSync() const
{
    return true;
}

void Jafg::LDesktopPlatformWin::SetVSync(const bool bEnabled)
{
    this->bVSync = bEnabled;
    glfwSwapInterval(bEnabled ? 1 : 0);

    return;
}

bool Jafg::LDesktopPlatformWin::IsVSync() const
{
    return this->bVSync;
}

Jafg::LNativeWindowWin* Jafg::LDesktopPlatformWin::CreateNativeWindow(const LDesktopSurfaceProps& Props)
{
    ::GLFWwindow* Window = glfwCreateWindow(Props.Width, Props.Height, Props.Title.ToC(), nullptr, nullptr);
    if (Window == nullptr)
    {
        return nullptr;
    }

    glfwMakeContextCurrent(Window);

    LNativeWindowWin* NativeWindow = new LNativeWindowWin();
    NativeWindow->NativeWindow = Window;
    return NativeWindow;
}

void Jafg::LDesktopPlatformWin::FramebufferSizeCallback(::GLFWwindow*, const int32 Width, const int32 Height)
{
    glViewport(0, 0, Width, Height);

    if (this->GetViewport())
    {
        this->GetViewport()->ChangeDimensions(LIntVector2(Width, Height));
    }

    return;
}

void Jafg::LDesktopPlatformWin::MouseCallback(::GLFWwindow* Window, const double XPos, const double YPos)
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

void Jafg::LDesktopPlatformWin::ScrollCallback(::GLFWwindow* Window, const double XOffset, const double YOffset)
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

void Jafg::LDesktopPlatformWin::MouseEnterCallback(GLFWwindow* Window, const int32 Entered)
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

void Jafg::LDesktopPlatformWin::CharCallback(GLFWwindow* Window, const uint32 Codepoint)
{
    std::u32string Char;
    Char.push_back(Codepoint);

#pragma warning( push )
#pragma warning(disable: 4996)
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::string utf8String = converter.to_bytes(Char);
#pragma warning( pop )

    this->AddBufferedPlatformInput(utf8String.c_str());

    return;
}

void Jafg::LDesktopPlatformWin::KeyCallback(::GLFWwindow* Window, const int32 Key, const int32 Scancode, const int32 Action, const int32 Mods)
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

#endif /* PLATFORM_WINDOWS */
