// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Platform/Surface.h"

namespace Jafg
{

/** Generic interface for a desktop platform. */
class RHI_API LDesktopPlatformBase : public LSurface
/*
 * We do not use the name LDesktopPlatform as it is reserved for the actual platform implementation class
 * by typedef'ing itself to LDesktopPlatform.
 */
{
public:

    ~LDesktopPlatformBase() override = default;

    // Surface implementation
    virtual void Initialize() override { }
    virtual void OnClear() override    { }
    virtual void OnUpdate() override   { }
    virtual void TearDown() override   { }
    virtual void PollInputs() override { }
    virtual void PollEvents() override { }
    virtual void SetInputMode(bool bShowCursor) override { }
    // ~Surface implementation

protected:

    struct LDesktopSurfaceProps final
    {
        FORCEINLINE explicit LDesktopSurfaceProps(
            LString Title = "Desktop Platform",
            const int32 Width = 1280,
            const int32 Height = 720
        )
            : Title(std::move(Title)), Width(Width), Height(Height)
        {
            return;
        }

        LString Title;
        int32   Width;
        int32   Height;
    };
};

} /* ~Namespace Jafg */

#if PLATFORM_WINDOWS
    #include "Platform/DesktopPlatformWin.h"
#else /* PLATFORM_WINDOWS */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_WINDOWS */
