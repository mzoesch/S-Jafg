// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Core/LaunchProgress.h"

#if PLATFORM_WASM

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


    return;
}

void Jafg::LPlatformWasm::OnClear()
{
    LSurface::OnClear();
}

void Jafg::LPlatformWasm::OnUpdate()
{
    LSurface::OnUpdate();
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
}

void Jafg::LPlatformWasm::PollEvents()
{
}

void Jafg::LPlatformWasm::SetInputMode(const bool bShowCursor)
{
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
