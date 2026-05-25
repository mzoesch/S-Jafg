// Copyright mzoesch. All rights reserved.

#if JAFG_PLATFORM_WASM

#include "Platform/Surface.h"
#include "Nodes/Viewport.h"
#include "Rhi/RhiVendorInclude.h"
#include "Async/TaskUtility.h"

namespace
{

//#
//# We have to do this with the extra array as we have to poll the keys outside the main engine loop (because of Wasm,
//# how it is implemented and communicates with JavaScript). Basically, when JavaScript is updating
//# the DOM we poll everything. But we, of course, do not want at that time to update player inputs - No, we have to do
//# this in the LPlatformWasm::PollEvents() method.
//#
//# Also this can be a singleton as there will never be more than one DOM in a Wasm application.
//#
Jafg::TArray<Jafg::LRawInput>& GetDownKeys()
{
    static Jafg::TArray<Jafg::LRawInput> DownKeys;
    return DownKeys;
}

Jafg::LKey TranslateKeyFromJavaScript(const u32 InKey)
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

EM_BOOL KeyDownCallback(const i32 EventType, const EmscriptenKeyboardEvent* E, void* UserData)
{
    checkSlow( E )
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

EM_BOOL KeyUpCallback(const i32 EventType, const EmscriptenKeyboardEvent* E, void* UserData)
{
    checkSlow( E )
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

EM_BOOL MouseMoveCallback(const i32 EventType, const EmscriptenMouseEvent* E, void* UserData)
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

EM_BOOL MouseDownCallback(const i32 EventType, const EmscriptenMouseEvent* E, void* UserData)
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

EM_BOOL MouseUpCallback(const i32 EventType, const EmscriptenMouseEvent* E, void* UserData)
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

EM_BOOL MouseWheelCallback(const i32 EventType, const EmscriptenWheelEvent* E, void* UserData)
{
    if (::GetDownKeys().Contains(Jafg::EKeys::MouseWheelAxis))
    {
        return EM_TRUE;
    }

    checkSlow( ::GetDownKeys().Contains(Jafg::EKeys::MouseWheelUp)   == false )
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

Jafg::LSurfaceDom::LSurfaceDom(LSurfaceDom &&Other) noexcept
{
    *this = std::move(Other);
}

Jafg::LSurfaceDom & Jafg::LSurfaceDom::operator=(LSurfaceDom &&Other) noexcept
{
    Super::operator=(std::move(Other));

    this->Handle = Other.Handle;
    this->_Handle = Other._Handle;
    this->bFirstMouseCallback = Other.bFirstMouseCallback;

    ::emscripten_webgl_make_context_current(this->_Handle);

    Other.Handle = nullptr;
    Other._Handle = static_cast<LDomHandle>(NULL);
    Other.bFirstMouseCallback = false;

    return *this;
}

Jafg::LSurfaceDom::~LSurfaceDom()
{
    if (this->Handle)
    {
        LSurfaceDom::TearDown();
    }

    return;
}

void Jafg::LSurfaceDom::Initialize()
{
    Super::Initialize();

    check( Tasks::IsOnMasterThread() )

    ::emscripten_set_canvas_element_size("#canvas", 1920, 1080);
    EmscriptenWebGLContextAttributes Attr;
    ::emscripten_webgl_init_context_attributes(&Attr);
    Attr.majorVersion = 2; // TODO Maybe we want to use WebGL 3.0 in the future? -sFULL_ES3
    Attr.minorVersion = 0;

    /* We create this extra handle here to allow for save valid checks. */
    this->_Handle = ::emscripten_webgl_create_context("#canvas", &Attr);
    this->Handle = &this->_Handle;

    ::emscripten_webgl_make_context_current(this->_Handle);

    this->GetViewport().ChangeDimensions(this->GetDimensions());
    this->GetViewport().SetPlatformDpi(96.0f); // TODO: Fetch from JavaScript.
    glViewport(0, 0, this->GetDimensions().X, this->GetDimensions().Y);

    ::emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, KeyDownCallback);
    ::emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, KeyUpCallback);
    ::emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, MouseMoveCallback);
    ::emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, MouseDownCallback);
    ::emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, MouseUpCallback);
    ::emscripten_set_wheel_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, MouseWheelCallback);

    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);

    return;
}

void Jafg::LSurfaceDom::OnClear()
{
    Super::OnClear();
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )

    ::emscripten_webgl_make_context_current(this->_Handle);

    Super::OnClear();

    return;
}

void Jafg::LSurfaceDom::OnUpdate()
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )
    ::emscripten_webgl_make_context_current(this->_Handle);

    Super::OnUpdate();

    return;
}

void Jafg::LSurfaceDom::TearDown()
{
    Super::TearDown();

    if (this->IsValid())
    {
        LOG_INFO(LogSurface, "Destroying DOM surface.")
        this->_Handle = 0;
        this->Handle = nullptr;
    }

    return;
}

void Jafg::LSurfaceDom::PollInputs()
{
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )

    for (const LRawInput& DownKey : ::GetDownKeys())
    {
        this->AddKeyDown(DownKey);
    }

    // /*
    //  * Keys only get called once by the JavaScript if they change (down / up).
    //  * But the mouse will not call a clean (zero movement) event.
    //  */
    // while (::GetDownKeys().RemoveOnce(LRawInput(EKeys::MouseX)))         { }
    // while (::GetDownKeys().RemoveOnce(LRawInput(EKeys::MouseY)))         { }
    // while (::GetDownKeys().RemoveOnce(LRawInput(EKeys::MouseWheelUp)))   { }
    // while (::GetDownKeys().RemoveOnce(LRawInput(EKeys::MouseWheelDown))) { }
    // while (::GetDownKeys().RemoveOnce(LRawInput(EKeys::MouseWheelAxis))) { }

    return;
}

void Jafg::LSurfaceDom::SetInputMode(const EInputMode::Type InMode, const bool bInShowCursor)
{
    Super::SetInputMode(InMode, bInShowCursor);
    checkSlow( this->Handle )
    checkSlow( Tasks::IsOnMasterThread() )

    if (this->bShowCursor)
    {
        this->bFirstMouseCallback = true;
    }

    ::emscripten_webgl_make_context_current(this->_Handle);
    if (this->bShowCursor)
    {
        ::emscripten_exit_pointerlock();
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
        ::emscripten_request_pointerlock("#canvas", EM_TRUE);
        EM_ASM({
            var canvas = document.getElementById("canvas");
            if (canvas)
            {
                canvas.style.cursor = 'none';
            }
        });
    }

    return;
}

void Jafg::LSurfaceDom::SetMouseCursor(const EMouseCursor::Type InCursor)
{
    LOG_WARNING(LogPlatform, "Custom cursor semantics are not supported on this platform for now.")
}

Jafg::TIntVector2<i32> Jafg::LSurfaceDom::GetDimensions() const
{
    return { 1920, 1080 };
}

bool Jafg::LSurfaceDom::CanVSync() const
{
    return false;
}

void Jafg::LSurfaceDom::SetVSync(const bool bEnabled)
{
    LOG_WARNING(LogPlatform, "VSync is not supported on this platform.")
}

bool Jafg::LSurfaceDom::IsVSync() const
{
    return false;
}

#endif /* JAFG_PLATFORM_WASM */
