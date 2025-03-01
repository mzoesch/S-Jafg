// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Platform/Surface.h"

#if !PLATFORM_USES_GLFW3_ABSTRACTION_LAYER
    #error "Tried to include glfw3 specific code on a platform that does not support glfw3."
#endif /* !PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */

struct GLFWwindow;
struct GLFWcursor;

namespace Jafg
{

namespace Private
{

struct LGlfw3Bridge;

} /* ~Namespace Jafg::Private */

class LSurfaceGlfw3 : public LSurfaceBase
{
public:

    friend Private::LGlfw3Bridge;

    typedef LSurfaceBase Super;

    static_assert(std::is_same_v<LSurfaceGlfw3, LSurface>);

    LSurfaceGlfw3() = default;
    PROHIBIT_COPY(LSurfaceGlfw3)
    LSurfaceGlfw3(LSurfaceGlfw3&& Other) noexcept;
    LSurfaceGlfw3& operator=(LSurfaceGlfw3&& Other) noexcept;
    ~LSurfaceGlfw3() override;

    virtual void Initialize() override;
    virtual void OnClear() override;
    virtual void OnUpdate() override;
    virtual void TearDown() override;

    virtual bool IsValid() override { return this->Handle != nullptr; }

    virtual void PollInputs() override;
    virtual void PollEvents() override;

    virtual void SetInputMode(const EInputMode::Type InMode, const bool bInShowCursor) override;
    virtual void SetMouseCursor(const EMouseCursor::Type InCursor) override;

    virtual int32 GetWidth() const override { return this->GetDimensions().X; }
    virtual int32 GetHeight() const override { return this->GetDimensions().Y; }
    virtual auto  GetDimensions() const -> TIntVector2<int32> override;

    virtual bool CanVSync() const override;
    virtual void SetVSync(const bool bEnabled) override;
    virtual bool IsVSync() const override { return this->bVSync; }

    FORCEINLINE GLFWcursor* GetNativeCursorHandleDangerous() const { return this->Cursor; }
    FORCEINLINE GLFWwindow* GetNativeHandleDangerous() const { return this->Handle; }

private:

    void FramebufferSizeCallback(const int32 Width, const int32 Height);
    void MouseCallback(const double XPos, const double YPos);
    void ScrollCallback(const double XOffset, const double YOffset);
    void MouseEnterCallback(const int32 Entered);
    void CharCallback(const uint32 Codepoint);
    void KeyCallback(const int32 Key, const int32 Scancode, const int32 Action, const int32 Mods);

    GLFWcursor* Cursor = nullptr;
    GLFWwindow* Handle = nullptr;

    bool bVSync = false;

    bool   bFirstMouseCallback = true;
    double LastMouseX = 0.0;
    double LastMouseY = 0.0;
};

} /* ~Namespace Jafg */
