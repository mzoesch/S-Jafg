// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"

#if PLATFORM_WASM

#include "Core/LaunchProgress.h"
#include "Widgets/Viewport.h"
#include "Platform/PlatformWasm.h"

namespace
{

/**
 * Singleton instance of the native window - this is the document, DOM or window object.
 *
 * @see https://developer.mozilla.org/en-US/docs/Web/API/Document
 * @see https://developer.mozilla.org/en-US/docs/Web/API/Document_Object_Model/Using_the_Document_Object_Model#what_is_a_dom_tree
 */
Jafg::LWasmNativeWindow* DocumentObjectModel = nullptr;

/*
 * We have to do this with the extra array as we have to poll the keys outside the main engine loop (because of Wasm,
 * how it is implemented and communicates with JavaScript). Basically, when JavaScript is updating
 * the DOM we poll everything. But we, of course, do not want at that time to update player inputs - No, we have to do
 * this in the LPlatformWasm::PollEvents() method.
 *
 * Also this can be a singleton as there will never be more than one DOM in a Wasm application.
 */
Jafg::TdhArray<Jafg::LRawInput>& GetDownKeys()
{
    static Jafg::TdhArray<Jafg::LRawInput> DownKeys;
    return DownKeys;
}

Jafg::LKey TranslateKeyFromJavaScript(const uint32 InKey)
{
    if (InKey >= 65 && InKey <= 90) /* A-Z */
    {
        return static_cast<Jafg::LKey>(InKey -  64 +   0);
    }
    if (InKey >= 48 && InKey <= 57) /* 0-9 */
    {
        return static_cast<Jafg::LKey>(InKey -  48 +  55);
    }
    if (InKey >= 96 && InKey <= 105) /* NumPad 0-9 */
    {
        return static_cast<Jafg::LKey>(InKey -  95 +  64);
    }
    if (InKey >= 112 && InKey <= 123) /* F1-F12 */
    {
        return static_cast<Jafg::LKey>(InKey - 111 +  81);
    }
    if (InKey == 27) /* Escape */
    {
        return Jafg::EKeys::Escape;
    }

    return Jafg::EKeys::Unresolved;
}

EM_BOOL KeyDownCallback(const int32 EventType, const EmscriptenKeyboardEvent *E, void *UserData)
{
    check( E )
    const Jafg::LKey TargetJafgKey = ::TranslateKeyFromJavaScript(E->keyCode);

    if (TargetJafgKey == Jafg::EKeys::Unresolved)
    {
        return EM_FALSE;
    }

    if (::GetDownKeys().Contains(TargetJafgKey) == false)
    {
        ::GetDownKeys().Emplace(TargetJafgKey);
    }

    return EM_TRUE;
}

EM_BOOL KeyUpCallback(const int32 EventType, const EmscriptenKeyboardEvent *E, void *UserData)
{
    check( E )
    const Jafg::LKey TargetJafgKey = ::TranslateKeyFromJavaScript(E->keyCode);

    if (TargetJafgKey == Jafg::EKeys::Unresolved)
    {
        return EM_FALSE;
    }

    if (::GetDownKeys().RemoveOnce(Jafg::LRawInput(TargetJafgKey)) == false)
    {
        LOG_WARNING(LogUserInput, "Key {}, was not found in the down keys list.", Jafg::LexToString(TargetJafgKey))
    }

    return EM_TRUE;
}

EM_BOOL MouseMoveCallback(const int32 EventType, const EmscriptenMouseEvent *E, void *UserData)
{
    if (Jafg::Maths::Absolute(E->movementX) > 0.0)
    {
        if (Jafg::LRawInput* Input = ::GetDownKeys().FindRef(Jafg::EKeys::MouseX); Input)
        {
            Input->Value += static_cast<float>(E->movementX);
        }
    }

    if (Jafg::Maths::Absolute(E->movementY) > 0.0)
    {
        if (Jafg::LRawInput* Input = ::GetDownKeys().FindRef(Jafg::EKeys::MouseY); Input)
        {
            Input->Value += static_cast<float>(E->movementY);
        }
    }

    return EM_TRUE;
}

EM_BOOL MouseDownCallback(const int32 EventType, const EmscriptenMouseEvent *E, void *UserData)
{
    if (E->button == 0)
    {
        if (::GetDownKeys().Contains(Jafg::EKeys::LeftMouseButton) == false)
        {
            ::GetDownKeys().Emplace(Jafg::EKeys::LeftMouseButton);
        }
    }

    if (E->button == 1)
    {
        if (::GetDownKeys().Contains(Jafg::EKeys::MiddleMouseButton) == false)
        {
            ::GetDownKeys().Emplace(Jafg::EKeys::MiddleMouseButton);
        }
    }

    if (E->button == 2)
    {
        if (::GetDownKeys().Contains(Jafg::EKeys::RightMouseButton) == false)
        {
            ::GetDownKeys().Emplace(Jafg::EKeys::RightMouseButton);
        }
    }

    if (E->button > 2)
    {
        return EM_FALSE;
    }

    return EM_TRUE;
}

EM_BOOL MouseUpCallback(const int32 EventType, const EmscriptenMouseEvent *E, void *UserData)
{
    if (E->button == 0)
    {
        if (::GetDownKeys().RemoveOnce(Jafg::LRawInput(Jafg::EKeys::LeftMouseButton)) == false)
        {
            LOG_WARNING(LogUserInput, "Key {}, was not found in the down keys list.", Jafg::LexToString(Jafg::EKeys::LeftMouseButton))
        }
    }

    if (E->button == 1)
    {
        if (::GetDownKeys().RemoveOnce(Jafg::LRawInput(Jafg::EKeys::MiddleMouseButton)) == false)
        {
            LOG_WARNING(LogUserInput, "Key {}, was not found in the down keys list.", Jafg::LexToString(Jafg::EKeys::MiddleMouseButton))
        }
    }

    if (E->button == 2)
    {
        if (::GetDownKeys().RemoveOnce(Jafg::LRawInput(Jafg::EKeys::RightMouseButton)) == false)
        {
            LOG_WARNING(LogUserInput, "Key {}, was not found in the down keys list.", Jafg::LexToString(Jafg::EKeys::RightMouseButton))
        }
    }

    if (E->button > 2)
    {
        return EM_FALSE;
    }

    return EM_TRUE;
}

EM_BOOL MouseWheelCallback(const int32 EventType, const EmscriptenWheelEvent *E, void *UserData)
{
    checkSlow( ::GetDownKeys().Contains(Jafg::EKeys::MouseWheelUp) == false )
    checkSlow( ::GetDownKeys().Contains(Jafg::EKeys::MouseWheelDown) == false )
    checkSlow( ::GetDownKeys().Contains(Jafg::EKeys::MouseWheelAxis) == false )

    /*
     * We ignore the actual value of the wheel as this is always in relation the DOM which would lead to unexpected
     * results with different screen sizes and resolutions.
     * This is not optimal and should be fixed in the future. But this would be too sophisticated for now.
     */
    if (E->deltaY > 0.0)
    {
        ::GetDownKeys().Emplace(Jafg::EKeys::MouseWheelUp);
    }
    else if (E->deltaY < 0.0)
    {
        ::GetDownKeys().Emplace(Jafg::EKeys::MouseWheelDown);
    }
    if (Jafg::Maths::Absolute(E->deltaY) > 0.0)
    {
        ::GetDownKeys().Emplace(Jafg::EKeys::MouseWheelAxis, E->deltaY > 0.0 ? 1.0f : -1.0f);
    }

    return EM_TRUE;
}

} /* ~Namespace <Anonymous> */

void Jafg::LPlatformWasm::Initialize()
{
    LSurface::Initialize();

    check( this->NativeWindow == nullptr )

    if (LaunchProgress::Private::GProgressWindow)
    {
        LaunchProgress::Private::bOwnerShipToken = true;
        this->NativeWindow = LaunchProgress::Private::GProgressWindow;
    }
    else
    {
        LOG_WARNING(LogSystem, "Launch progress did not create a window. Creating one now.")
        this->NativeWindow = LPlatformWasm::CreateNativeWindow();
    }

    check( DocumentObjectModel == nullptr )
    DocumentObjectModel = this->NativeWindow;

    if (ensure(this->GetViewport()))
    {
        this->GetViewport()->ChangeDimensions(this->GetDimensions());
        this->GetViewport()->SetPlatformDpi(96.0f); // TODO: Fetch from JavaScript.
        glViewport(0, 0, this->GetDimensions().X, this->GetDimensions().Y);
    }
    else
    {
        panic( "Currently a viewport must be provided." )
    }

    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, KeyDownCallback);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, KeyUpCallback);
    emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, MouseMoveCallback);
    emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, MouseDownCallback);
    emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, MouseUpCallback);
    emscripten_set_wheel_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, MouseWheelCallback);

    return;
}

void Jafg::LPlatformWasm::OnClear()
{
    LSurface::OnClear();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return;
}

void Jafg::LPlatformWasm::OnUpdate()
{
    LSurface::OnUpdate();
    return;
}

void Jafg::LPlatformWasm::TearDown()
{
    LSurface::TearDown();

    if (this->NativeWindow)
    {
        check( NativeWindow == DocumentObjectModel )
        delete this->NativeWindow;
        this->NativeWindow = nullptr;
        DocumentObjectModel = nullptr;
    }

    check( DocumentObjectModel == nullptr )

    return;
}

void Jafg::LPlatformWasm::PollInputs()
{
    for (const LRawInput& DownKey : ::GetDownKeys())
    {
        this->AddKeyDown(DownKey);
    }

    /*
     * Keys only get called once by the JavaScript if they change (down / up).
     * But the mouse will not call a clean (zero movement) event.
     */
    ::GetDownKeys().RemoveOnce(LRawInput(EKeys::MouseX));
    ::GetDownKeys().RemoveOnce(LRawInput(EKeys::MouseY));
    ::GetDownKeys().RemoveOnce(LRawInput(EKeys::MouseWheelUp));
    ::GetDownKeys().RemoveOnce(LRawInput(EKeys::MouseWheelDown));
    ::GetDownKeys().RemoveOnce(LRawInput(EKeys::MouseWheelAxis));

    return;
}

void Jafg::LPlatformWasm::PollEvents()
{
    /*
     * Nothing to do here. Events are polled in JavaScript.
     */
}

void Jafg::LPlatformWasm::SetInputMode(const bool bShowCursor)
{
    if (bShowCursor)
    {
        this->bFirstMouseCallback = true;
    }

    if (bShowCursor)
    {
        emscripten_exit_pointerlock();
        EM_ASM({
            var canvas = document.getElementById("canvas");
            if (canvas)
            {
                canvas.style.cursor = 'auto';
            }
        });
    }
    else
    {
        emscripten_request_pointerlock("#canvas", EM_TRUE);
        EM_ASM({
            var canvas = document.getElementById("canvas");
            if (canvas)
            {
                canvas.style.cursor = 'none';
            }
        });
    }

    this->bShowMouseCursor = bShowCursor;

    return;
}

int32 Jafg::LPlatformWasm::GetWidth() const
{
    return this->GetDimensions().X;
}

int32 Jafg::LPlatformWasm::GetHeight() const
{
    return this->GetDimensions().Y;
}

Jafg::TIntVector2<int32> Jafg::LPlatformWasm::GetDimensions() const
{
    return LIntVector2(1920, 1080);
}

bool Jafg::LPlatformWasm::CanVSync() const
{
    return false;
}

void Jafg::LPlatformWasm::SetVSync(const bool bEnabled)
{
    LOG_WARNING(LogPlatform, "VSync is not supported on this platform.");
}

bool Jafg::LPlatformWasm::IsVSync() const
{
    return false;
}

Jafg::LWasmNativeWindow* Jafg::LPlatformWasm::CreateNativeWindow()
{
    jassert( DocumentObjectModel == nullptr )

    LWasmNativeWindow* Window = new LWasmNativeWindow();

    emscripten_set_canvas_element_size("#canvas", 1920, 1080);
    EmscriptenWebGLContextAttributes Attr;
    emscripten_webgl_init_context_attributes(&Attr);
    Attr.majorVersion = 3;
    Attr.minorVersion = 0;

    Window->Handle = emscripten_webgl_create_context("#canvas", &Attr);

    /*
     * We can do this as Wasm will never allow for more than one window.
     */
    emscripten_webgl_make_context_current(Window->Handle);

    return Window;
}

#endif /* PLATFORM_WASM */
