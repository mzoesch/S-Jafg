// Copyright mzoesch. All rights reserved.

#pragma once

#if !PLATFORM_WINDOWS
    #error "Tried to include Windoews specific platform code on a non-Windows platform."
#endif /* !PLATFORM_WINDOWS */

#include "CoreAFX.h"
#include "Platform/DesktopPlatform.h"

struct GLFWwindow;

class DesktopPlatformWin final : public DesktopPlatformBase
{
public:

    ~DesktopPlatformWin() override = default;

    // DesktopPlatformBase implementation
    virtual void Initialize() override;
    virtual void OnClear() override;
    virtual void OnUpdate() override;
    virtual void TearDown() override;
    virtual void PollInputs() override;
    virtual void PollEvents() override;
    virtual void SetInputMode(const bool bShowCursor) override;
    // ~DesktopPlatformBase implementation

    // Surface implementation
    virtual auto GetWidth() const -> int32 override;
    virtual auto GetHeight() const -> int32 override;
    virtual auto GetDimensions() const -> TIntVector2<int32> override;
    virtual auto SetVSync(const bool bEnabled) -> void override;
    virtual auto IsVSync() const -> bool override;
    // ~Surface implementation

    FORCEINLINE auto GetMasterWindow() const -> GLFWwindow* { return this->MasterWindow; }

private:

    GLFWwindow* CreateNativeWindow(const DesktopSurfaceProps& Props) const;

    void FramebufferSizeCallback(GLFWwindow* Window, const int32 Width, const int32 Height);
    void MouseCallback(GLFWwindow* Window, const double XPos, const double YPos);
    void ScrollCallback(GLFWwindow* Window, const double XOffset, const double YOffset);

    GLFWwindow* MasterWindow = nullptr;

    bool bVSync = false;
};

/** The currently active desktop platform. */
typedef DesktopPlatformWin DesktopPlatform;
