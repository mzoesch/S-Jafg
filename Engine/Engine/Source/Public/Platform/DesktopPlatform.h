// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Platform/Surface.h"

#if !PLATFORM_DESKTOP
    #error "Tried to include desktop platform code on a non-desktop platform."
#endif /* !PLATFORM_DESKTOP */

struct GLFWwindow;

namespace Jafg
{

struct LDesktopSurfaceProps final
{
    FORCEINLINE explicit LDesktopSurfaceProps(
        LSimpleString Title  = "Jafg Desktop Platform",
        const int32   Width  = 1280,
        const int32   Height = 720
    )
        : Title(std::move(Title)), Width(Width), Height(Height)
    {
        return;
    }

    LSimpleString Title;
    int32         Width;
    int32         Height;
};

struct LNativeDesktopWindowBase // <-- Do not use this struct directly but the platform-specific typedef'ed version
                                //     instead to make code platform-agnostic.
{
    // This struct always defines the following member function:
    //   - GetNativeWindow() const -> Ptr*
};

/** Generic interface for a desktop platform. */
class ENGINE_API LDesktopPlatformBase : public LSurface
/*
 * We do not use the name LDesktopPlatform as it is reserved for the actual platform implementation class
 * by typedef'ing itself to LDesktopPlatform.
 */
{
public:

    ~LDesktopPlatformBase() override = default;

    // Surface implementation
    virtual void Initialize() override { LSurface::Initialize(); }
    virtual void OnClear() override    { LSurface::OnClear();    }
    virtual void OnUpdate() override   { LSurface::OnUpdate();   }
    virtual void TearDown() override   { LSurface::TearDown();   }
    virtual void PollInputs() override { }
    virtual void PollEvents() override { }
    virtual void SetInputMode(const EInputMode::Type InMode, const bool bInShowCursor) override { LSurface::SetInputMode(InMode, bInShowCursor); }
    virtual void SetMouseCursor(const EMouseCursor::Type InCursor) override { }
    // ~Surface implementation
};

} /* ~Namespace Jafg */

#if PLATFORM_WINDOWS
    #include "Platform/DesktopPlatformWin.h"
#else /* PLATFORM_WINDOWS */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_WINDOWS */
