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

class LSurfaceGlfw3 final : public LSurfaceBase
{
public:

    friend Private::LGlfw3Bridge;

    typedef LSurfaceBase Super;

    static_assert(std::is_same_v<LSurfaceGlfw3, LSurface>);

    ENGINE_API LSurfaceGlfw3() = default;
    PROHIBIT_COPY(LSurfaceGlfw3)
    ENGINE_API LSurfaceGlfw3(LSurfaceGlfw3&& Other) noexcept;
    ENGINE_API LSurfaceGlfw3& operator=(LSurfaceGlfw3&& Other) noexcept;
    ENGINE_API virtual ~LSurfaceGlfw3() override;

    virtual void Initialize() override;
    virtual void OnClear() override;
    virtual void OnUpdate() override;
    virtual void TearDown() override;

    FORCEINLINE virtual bool IsValid() override { return this->Handle != nullptr; }

    virtual void PollInputs() override;
    virtual void PollEvents() override;

    ENGINE_API virtual void SetInputMode(const EInputMode::Type InMode, const bool bInShowCursor) override;
    ENGINE_API virtual void SetMouseCursor(const EMouseCursor::Type InCursor) override;

               virtual int32 GetWidth() const override { return this->GetDimensions().X; }
               virtual int32 GetHeight() const override { return this->GetDimensions().Y; }
    ENGINE_API virtual auto  GetDimensions() const -> TIntVector2<int32> override;

    ENGINE_API  virtual bool CanVSync() const override;
    ENGINE_API  virtual void SetVSync(const bool bEnabled) override;
    FORCEINLINE virtual bool IsVSync() const override { return this->bVSync; }

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
