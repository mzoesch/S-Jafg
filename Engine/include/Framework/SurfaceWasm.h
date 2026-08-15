// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Surface.h"

#if !JAFG_PLATFORM_WASM
    #error "Tried to include wasm specific platform code on a non-wasm platform."
#endif /* !JAFG_PLATFORM_WASM */

namespace Jafg
{

typedef EMSCRIPTEN_WEBGL_CONTEXT_HANDLE LDomHandle;

//#
//# Weak reference to a canvas element in the DOM.
//# @see https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API
//#
class LSurfaceDom final : public LSurfaceBase
{
public:

    typedef LSurfaceBase Super;

    static_assert(std::is_same_v<LSurfaceDom, LSurface>);

    LSurfaceDom() = default;
    PROHIBIT_COPY(LSurfaceDom);
    LSurfaceDom(LSurfaceDom&& Other) noexcept;
    LSurfaceDom& operator=(LSurfaceDom&& Other) noexcept;
    virtual ~LSurfaceDom() override;

    virtual void Initialize() override;
    virtual void OnClear() override;
    virtual void OnUpdate() override;
    virtual void TearDown() override;

    virtual bool IsValid() override { return this->Handle != nullptr; }

    virtual void PollInputs() override;
    virtual void PollEvents() override { /* No need to poll events in wasm this is done in JavaScript. */ }

    virtual void SetInputMode(const EInputMode::Type InMode, const bool bInShowCursor) override;
    virtual void SetMouseCursor(const EMouseCursor::Type InCursor) override;

    virtual i32 GetWidth() const override { return this->GetDimensions().X; }
    virtual i32 GetHeight() const override { return this->GetDimensions().Y; }
    virtual auto  GetDimensions() const -> TIntVector2<i32> override;

    virtual bool CanVSync() const override;
    virtual void SetVSync(const bool bEnabled) override;
    virtual bool IsVSync() const override;

    FORCEINLINE LDomHandle* GetNativeHandleDangerous() const { return this->Handle; }

private:

    LDomHandle* Handle = nullptr;
    LDomHandle _Handle = 0;
    bool bFirstMouseCallback = true;
};

} /* ~Namespace Jafg */
