// Copyright mzoesch. All rights reserved.

#pragma once

#include "JgcMinimal.afx"

namespace Jafg
{

class AGimbalComponent;

} /* ~Namespace Jafg */

namespace Jgc
{

//# How #LGimbalFreeLook turns mouse input into changes of a gimbal.
struct LGimbalFreeLookSettings final
{
    //# Radians per pixel.
    f32 Sensitivity{ 0.003f };
    bool bInvertPitch{};
    //# Meters per pixel at a zoom of one. Scales with the zoom.
    f32 PanSensitivity{ 0.03f };
    //# The factor of the distance per notch of the mouse wheel.
    f32 ZoomStep{ 1.15f };
    //# Whether the mouse wheel only zooms while free look is held. Otherwise it always does.
    bool bZoomOnlyWhileHeld{ true };
    //#
    //# Less mouse movement than this (in pixels) while held counts as a tap, which centers the gimbal again when it
    //# is released. Zooming is never a tap. Zero: Taps do nothing.
    //#
    f32 RecenterThreshold{ 3.0f };
};

//#
//# Free look with the mouse around a pivot: Turns, pans and zooms a gimbal (#Jafg::AGimbalComponent), e.g., to look
//# around a vehicle or a character. The owner forwards the input while it is dispatched (#OnHeld, #OnLook, ...) and
//# calls #Apply once per frame.
//#
//#   - Held and mouse: Turns the gimbal (or pans it while pan is held, too).
//#   - Mouse wheel: Changes the distance to the pivot.
//#   - Only tapped (see #LGimbalFreeLookSettings::RecenterThreshold): Centers the gimbal again.
//#
//# Holding is polled every frame, so that a lost release (e.g., focus loss) never leaves free look active.
//#
class JGC_API LGimbalFreeLook final
{
public:

    //# Call in every frame in which free look is held.
    void OnHeld() noexcept { this->bFrameHeld = true; }
    //# Call in every frame in which panning is held. Only pans while free look is held as well.
    void OnPanHeld() noexcept { this->bFramePanHeld = true; }
    //# In pixels.
    void OnLook(LVec2F const& Delta) noexcept { this->FrameLook += Delta; }
    //# In notches of the mouse wheel. Positive zooms in (closer to the pivot).
    void OnZoom(f32 Notches) noexcept { this->FrameZoom += Notches; }

    //#
    //# Applies the input collected since the last call and starts collecting anew.
    //# @param Gimbal What to change. If null, the input is used up all the same.
    //#
    void Apply(Jafg::AGimbalComponent* Gimbal, LGimbalFreeLookSettings const& Settings) noexcept;
    //# Forgets all input and leaves free look without centering (e.g., when the controls are given away).
    void Reset() noexcept;

    //# Whether free look was held in the last applied frame.
    NODISCARD FORCEINLINE constexpr bool IsActive() const noexcept { return this->bActive; }

private:

    //# Collected while the input is dispatched. Applied and reset once per frame.
    bool bFrameHeld{};
    bool bFramePanHeld{};
    LVec2F FrameLook{ 0.0f };
    f32 FrameZoom{};

    bool bActive{};
    //# How far the mouse moved during the current free look. In pixels.
    f32 Travel{};
};

} /* ~Namespace Jgc */
