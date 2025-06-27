// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Platform/Surface.h"

#if !JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER
    #error "Tried to include glfw3 specific code on a platform that does not support glfw3."
#endif /* !JAFG_PLATFORM_USES_GLFW3_ABSTRACTION_LAYER */

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

               virtual i32  GetWidth() const override { return this->GetDimensions().X; }
               virtual i32  GetHeight() const override { return this->GetDimensions().Y; }
    ENGINE_API virtual auto GetDimensions() const -> TIntVector2<i32> override;

    ENGINE_API  virtual bool CanVSync() const override;
    ENGINE_API  virtual void SetVSync(const bool bEnabled) override;
    FORCEINLINE virtual bool IsVSync() const override { return this->bVSync; }

    FORCEINLINE bool IsFirstMouseCallback() const noexcept { return this->bFirstMouseCallback; }
    FORCEINLINE f64  GetLastMouseX() const noexcept { return this->LastMouseX; }
    FORCEINLINE f64  GetLastMouseY() const noexcept { return this->LastMouseY; }

    FORCEINLINE GLFWcursor* GetNativeCursorHandleDangerous() const { return this->Cursor; }
    FORCEINLINE GLFWwindow* GetNativeHandleDangerous() const { return this->Handle; }

private:

    void FramebufferSizeCallback(const i32 Width, const i32 Height);
    void MouseCallback(const f64 XPos, const f64 YPos);
    void ScrollCallback(const f64 XOffset, const f64 YOffset);
    void MouseEnterCallback(const i32 Entered);
    void CharCallback(const u32 Codepoint);
    void KeyCallback(const i32 Key, const i32 Scancode, const i32 Action, const i32 Mods);

#if PLATFORM_LINUX
    virtual void EmulateRepeatedContentForBufferedInput() override;
    virtual void EmulateContentForBufferedInput(const LKey InKey) override;
    virtual void EmulateContentForBufferedInputGlfw3(const i32 InKey);
#endif /* PLATFORM_LINUX */

    GLFWcursor* Cursor { nullptr };
    GLFWwindow* Handle { nullptr };

    bool bVSync { false };

    bool bFirstMouseCallback { true };
    f64 LastMouseX { 0.0 };
    f64 LastMouseY { 0.0 };

#if PLATFORM_LINUX
    //#
    //# This is not in the EKeys::Type format but in the Glfw3 format.
    //#
    i32 Glfw3LastNewKey { INDEX_NONE };
#endif /* PLATFORM_LINUX */
};

} /* ~Namespace Jafg */
