// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Platform/DesktopPlatform.h"

#if !PLATFORM_WINDOWS
    #error "Tried to include Windows specific platform code on a non-Windows platform."
#endif /* !PLATFORM_WINDOWS */

struct GLFWwindow;

namespace Jafg
{

struct LNativeWindowWin final : public LNativeDesktopWindowBase
{
    FORCEINLINE auto GetNativeWindow() const -> ::GLFWwindow* { return this->NativeWindow; }

    ::GLFWwindow* NativeWindow = nullptr;
};

class ENGINE_API LDesktopPlatformWin final : public LDesktopPlatformBase
{
public:

    ~LDesktopPlatformWin() override = default;

    // DesktopPlatformBase implementation
    virtual void Initialize() override;
    virtual void OnClear() override;
    virtual void OnUpdate() override;
    virtual void TearDown() override;
    virtual void PollInputs() override;
    virtual void PollEvents() override;
    virtual void SetInputMode(const EInputMode::Type InMode, const bool bInShowCursor) override;
    // ~DesktopPlatformBase implementation

    // Surface implementation
    virtual auto GetWidth() const -> int32 override;
    virtual auto GetHeight() const -> int32 override;
    virtual auto GetDimensions() const -> TIntVector2<int32> override;
    virtual auto CanVSync() const -> bool override;
    virtual auto SetVSync(const bool bEnabled) -> void override;
    virtual auto IsVSync() const -> bool override;
    // ~Surface implementation

    /**
     * You are the owner.
     */
    static LNativeWindowWin* CreateNativeWindow(const LDesktopSurfaceProps& Props);

private:

    void FramebufferSizeCallback(::GLFWwindow* Window, const int32 Width, const int32 Height);
    void MouseCallback(::GLFWwindow* Window, const double XPos, const double YPos);
    void ScrollCallback(::GLFWwindow* Window, const double XOffset, const double YOffset);
    void MouseEnterCallback(::GLFWwindow* Window, const int32 Entered);

    LNativeWindowWin* MasterWindow = nullptr;

    bool bVSync = false;

    bool bFirstMouseCallback = true;
    double LastMouseX = 0.0;
    double LastMouseY = 0.0;
};

} /* ~Namespace Jafg */
