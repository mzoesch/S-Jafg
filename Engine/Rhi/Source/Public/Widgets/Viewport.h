// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

class WUserWidget;

/**
 * Represents a viewport that can contain widgets.
 * A viewport has in most cases a handle to some sort of platform-specific window instance.
 */
class RHI_API LViewport final
{
public:

    LViewport() = default;
    PROHIBIT_REALLOC_OF_ANY_FROM(LViewport)
    ~LViewport() = default;

    void Initialize();
    void Tick();
    void Draw();
    void TearDown();

    void AddWidget(WUserWidget* Widget);
    void RemoveWidget(WUserWidget* Widget);

    /** The scale factor is based on the physical platform dpi in relation to the base dpi. */
    FORCEINLINE auto GetScaleFactor() const -> float { return this->ScaleFactor; }
    FORCEINLINE auto SetPlatformDpi(const float InDpi) -> void { this->PlatformDpi = InDpi; }
    FORCEINLINE auto GetPlatformDpi() const -> float { return this->PlatformDpi; }
    FORCEINLINE auto GetBaseDpi() const -> float { return this->BaseDpi; }

    auto ChangeDimensions(const LIntVector2& InDimensions) -> void;
    FORCEINLINE auto GetDimensions() const -> LIntVector2 { return this->Dimensions; }

    FORCEINLINE auto GetFrameOrthoZLayerDepth() const -> float
    {
        this->FrameZLayerDepth += 0.0001f;
        return this->FrameZLayerDepth;
    }

private:

    void RecalculateScaleFactor();

    /** The factor with which the entire orthographic projection is scaled. */
    float ScaleFactor =  1.0f;
    /** The dpi fetched from the physical platform. */
    float PlatformDpi =  0.0f;
    /**
     * The base dpi that the application was designed for.
     * All scales are based and calculated from this value, and only for the drawing
     * we use the platform dpi.
     */
    float BaseDpi     = 96.0f;

    /** The dimensions of the viewport in px. */
    LIntVector2            Dimensions;
    /** Top level widgets that this viewport owns. */
    TdhArray<WUserWidget*> TopLevelWidgets;

    mutable float FrameZLayerDepth = 0.0f;
};

} /* ~Namespace Jafg. */
