// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "DesktopPlatform.h"

struct GLFWwindow;

class DesktopPlatformWin : public DesktopPlatformBase
{
public:

    ~DesktopPlatformWin() override = default;

    // DesktopPlatformBase implementation
    void Init() override;
    void TearDown() override;
    // ~DesktopPlatformBase implementation

    // Surface implementation
    auto OnUpdate() -> void override;
    auto GetWidth() const -> int32 override;
    auto GetHeight() const -> int32 override;
    auto SetVSync(const bool bEnabled) -> void override;
    auto IsVSync() const -> bool override;
    // ~Surface implementation

protected:

    GLFWwindow* CreateNativeWindow(const DesktopSurfaceProps& Props);

private:

    GLFWwindow* MasterWindow = nullptr;

    int32 WindowCount = 0;
};

typedef DesktopPlatformWin DesktopPlatform;
