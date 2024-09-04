// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

struct GLFWwindow;

class DesktopPlatformWin final : public DesktopPlatformBase
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

    class VertexArray* Va = nullptr;
    class IndexBuffer* Ib = nullptr;
    class VertexBuffer* Vb = nullptr;
    class Shader* Sh = nullptr;
    class Texture* Tx = nullptr;
    class Renderer* MyRenderer = nullptr;

    float r = 0.0f;
    float increment = 0.005f;
};

typedef DesktopPlatformWin DesktopPlatform;
