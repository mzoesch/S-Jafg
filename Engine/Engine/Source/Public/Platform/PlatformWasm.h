// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Platform/Surface.h"

#if !PLATFORM_WASM
    #error "Tried to include wasm specific platform code on a non-wasm platform."
#endif /* !PLATFORM_WASM */

namespace Jafg
{

typedef EMSCRIPTEN_WEBGL_CONTEXT_HANDLE LDomHandle;

struct LWasmNativeWindow
{
    LDomHandle Handle = 0;
};

class LPlatformWasm final : public LSurface
{
public:

    ~LPlatformWasm() override = default;

    // Surface implementation
    virtual void Initialize() override;
    virtual void OnClear() override;
    virtual void OnUpdate() override;
    virtual void TearDown() override;
    virtual void PollInputs() override;
    virtual void PollEvents() override;
    virtual void SetInputMode(const bool bShowCursor) override;
    virtual auto GetWidth() const -> int32 override;
    virtual auto GetHeight() const -> int32 override;
    virtual auto GetDimensions() const -> TIntVector2<int32> override;
    virtual bool CanVSync() const override;
    virtual auto SetVSync(const bool bEnabled) -> void override;
    virtual auto IsVSync() const -> bool override;
    // ~Surface implementation

    static LWasmNativeWindow* CreateNativeWindow();

private:

    LWasmNativeWindow* NativeWindow = nullptr;
};

} /* ~Namespace Jafg */
