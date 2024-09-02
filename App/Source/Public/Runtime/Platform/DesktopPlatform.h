// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Surface.h"

class DesktopPlatformBase : public Surface
{
public:

    ~DesktopPlatformBase() override = default;

    virtual void Init() = 0;
    virtual void TearDown() = 0;

protected:

    struct DesktopSurfaceProps
    {
        explicit DesktopSurfaceProps(String Title = "Desktop Platform", const int32 Width = 1280, const int32 Height = 720)
            : Title(std::move(Title)), Width(Width), Height(Height)
        {
        }

        String Title;
        int32 Width;
        int32 Height;
    };
};

#if PLATFORM_WINDOWS
    #include "Runtime/Platform/DesktopPlatformWin.h"
#else /* PLATFORM_WINDOWS */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_WINDOWS */
