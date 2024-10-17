// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Platform/Surface.h"

namespace Jafg
{

/** Generic interface for a desktop platform. */
class ENGINE_API DesktopPlatformBase : public Surface
/*
 * We do not use the name DesktopPlatform as it is reserved for the actual platform implementation class
 * by typedef'ing itself to DesktopPlatform.
 */
{
public:

    ~DesktopPlatformBase() override = default;

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

    struct DesktopSurfaceProps
    {
        explicit DesktopSurfaceProps(LString Title = "Desktop Platform", const int32 Width = 1280, const int32 Height = 720)
            : Title(std::move(Title)), Width(Width), Height(Height)
        {
        }

        LString Title;
        int32   Width;
        int32   Height;
    };
};

} /* Namespace Jafg */

#if PLATFORM_WINDOWS
    #include "Platform/DesktopPlatformWin.h"
#else
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_WINDOWS */
