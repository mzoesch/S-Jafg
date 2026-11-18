// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SceneComponent.h"
#include "GimbalComponent.generated.h"

namespace Jafg
{

//#
//# Rotates its children around itself, e.g., an eye that looks around a vehicle. The rotation is a yaw around the up
//# of the parent followed by a pitch around the right. Positive yaw turns the view right, positive pitch turns it up.
//#
//# The gimbal itself sits at the pivot. Put the eyes below it at their distance (e.g., behind and above). Sprung eyes
//# follow a turning gimbal rigidly (see #AEyeComponent).
//#
//# Additionally:
//#   - Pan: The pivot moves away from where it was placed, along the right and up of the gimbal (so in the plane of
//#     the view). Panning right moves what is looked at to the left.
//#   - Zoom: The distance of all children to the pivot is scaled (by the scale of this component).
//#
//# The placement of the pivot is the local translation of this component when it is attached.
//#
DECLARE_JAFG_CLASS()
class ENGINE_API AGimbalComponent : public ASceneComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(AGimbalComponent)
    {
        this->bTick = true;
    }

public:

    virtual void OnAttach(AActor& InOwner) override;
    virtual void ParentTick(f32 Dt) override;

    //# In radians.
    NODISCARD FORCEINLINE constexpr f32 GetYaw() const noexcept { return this->Yaw; }
    NODISCARD FORCEINLINE constexpr f32 GetPitch() const noexcept { return this->Pitch; }
    //# In meters. X is right, Y is up.
    NODISCARD FORCEINLINE constexpr LVec2F GetPan() const noexcept { return this->Pan; }
    //# The scale of the distance of the children. One is as placed.
    NODISCARD FORCEINLINE constexpr f32 GetZoom() const noexcept { return this->Zoom; }
    NODISCARD FORCEINLINE constexpr bool IsCentered() const noexcept
    {
        return this->Yaw == 0.0f && this->Pitch == 0.0f && this->Pan == LVec2F{0.0f} && this->Zoom == 1.0f;
    }
    NODISCARD FORCEINLINE constexpr bool IsRecentering() const noexcept { return this->bRecentering; }

    //# Turn by a delta in radians. Stops a recentering.
    void AddRotation(f32 YawDelta, f32 PitchDelta) noexcept;
    //# In radians. Stops a recentering.
    void SetRotation(f32 InYaw, f32 InPitch) noexcept;
    //# Move the pivot by a delta in meters (X right, Y up). Stops a recentering.
    void AddPan(LVec2F const& Delta) noexcept;
    //# Multiply the zoom. Above one moves the children away from the pivot. Stops a recentering.
    void MultiplyZoom(f32 Factor) noexcept;
    //# Go back to the center: No rotation, no pan, a zoom of one. Smoothly with #RecenterSpeed, unless #bInstant.
    void Recenter(bool bInstant = false) noexcept;

    //# The first gimbal above #Comp in its actor, or #Comp itself. Null if there is none.
    NODISCARD static AGimbalComponent* FindAbove(ASceneComponent& Comp) noexcept;

    //# How far it may yaw to either side. In degrees. 180 or more turns all around.
    CLASS_FIELD(EditorEditable)
    f32 MaxYaw{ 180.0f };
    //# How far it may pitch up or down. In degrees.
    CLASS_FIELD(EditorEditable)
    f32 MaxPitch{ 80.0f };
    //# How fast it goes back to the center. In 1/s.
    CLASS_FIELD(EditorEditable)
    f32 RecenterSpeed{ 6.0f };
    //# How far the pivot may move sideways. In meters.
    CLASS_FIELD(EditorEditable)
    f32 MaxPanLateral{ 128.0f };
    //# How far the pivot may move up or down. In meters.
    CLASS_FIELD(EditorEditable)
    f32 MaxPanVertical{ 128.0f };
    CLASS_FIELD(EditorEditable)
    f32 MinZoom{ 0.25f };
    CLASS_FIELD(EditorEditable)
    f32 MaxZoom{ 1024.0f };

private:

    //# Limits everything and writes the transform.
    void Apply() noexcept;

    //# Where the pivot was placed. Relative to the parent.
    LWorldVec3 Pivot{ maths::zero_vector<LWorldVec3> };
    f32 Yaw{};
    f32 Pitch{};
    LVec2F Pan{ 0.0f };
    f32 Zoom{ 1.0f };
    bool bRecentering{};
};

} /* ~Namespace Jafg */
