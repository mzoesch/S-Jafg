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

//#
//# Weak reference to a canvas element in the DOM.
//# @see https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API
//#
class LSurfaceDom final : public LSurface
{
public:

    typedef LSurface Super;

    virtual void Initialize() override;
    virtual void OnClear() override;
    virtual void TearDown() override;

    virtual bool IsValid() override { return this->Handle != nullptr; }

    virtual void PollInputs() override;
    virtual void PollEvents() override { /* No need to poll events in wasm this is done in Java Script. */ }

    virtual void SetInputMode(const EInputMode::Type InMode, const bool bInShowCursor) override;
    virtual void SetMouseCursor(const EMouseCursor::Type InCursor) override;

    virtual int32 GetWidth() const override { return this->GetDimensions().X; }
    virtual int32 GetHeight() const override { return this->GetDimensions().Y; }
    virtual auto  GetDimensions() const -> TIntVector2<int32> override;

    virtual bool CanVSync() const override;
    virtual void SetVSync(const bool bEnabled) override;
    virtual bool IsVSync() const override;

    FORCEINLINE LDomHandle* GetNatvieHandleDangerous() const { return this->Handle; }

private:

    LDomHandle* Handle = nullptr;
    LDomHandle _Handle = 0;
    bool bFirstMouseCallback = true;
};

} /* ~Namespace Jafg */
