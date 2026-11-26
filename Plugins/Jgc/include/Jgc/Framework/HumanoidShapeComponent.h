// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/ActorComponent.h"
#include "HumanoidShapeComponent.generated.h"

namespace Jafg
{

class ASkeletalMeshComponent;

} /* ~Namespace Jafg */

namespace Jgc
{

//#
//# The proportions of a humanoid body (@see #AHumanoidShapeComponent). Every value is a factor of the authored
//# body: One keeps it as it is. Only the mesh changes, nothing else of the look (e.g., the materials), and nothing of
//# the movement.
//#
struct LHumanoidShape final
{
    //# All of the head.
    f32 HeadSize{ 1.0f };
    f32 HeadWidth{ 1.0f };
    f32 HeadHeight{ 1.0f };
    f32 HeadDepth{ 1.0f };
    f32 NeckLength{ 1.0f };
    //# From the hips to the neck. Moves everything above up.
    f32 TorsoLength{ 1.0f };
    //# The ribcage and the upper chest. Moves the arms outward.
    f32 ChestWidth{ 1.0f };
    f32 ChestDepth{ 1.0f };
    //# The belly.
    f32 WaistWidth{ 1.0f };
    f32 WaistDepth{ 1.0f };
    //# Moves the legs outward.
    f32 HipWidth{ 1.0f };
    //# The collar bones. Moves the arms outward.
    f32 ShoulderWidth{ 1.0f };
    f32 UpperArmLength{ 1.0f };
    f32 LowerArmLength{ 1.0f };
    f32 HandSize{ 1.0f };
    f32 UpperLegLength{ 1.0f };
    f32 LowerLegLength{ 1.0f };
    f32 FootSize{ 1.0f };

    NODISCARD bool operator==(LHumanoidShape const&) const = default;
};

//# One value of #LHumanoidShape, e.g., to show it as a slider.
struct LHumanoidShapeParameter final
{
    //# The part of the body (e.g., "Head"). Parameters of the same group follow each other.
    LStringView Group;
    LStringView DisplayName;
    f32 LHumanoidShape::* Value;
    //# The smallest and largest factor that the value may have.
    f32 Min;
    f32 Max;
};

//# Every value of #LHumanoidShape, grouped by the part of the body, from the head to the feet.
NODISCARD JGC_API std::span<LHumanoidShapeParameter const> GetHumanoidShapeParameters() noexcept;

//#
//# The shape of the body of a humanoid character (@see #EquipHumanoidBody), changed on top of every animation
//# (@see #Jafg::LJointProportion). Bones are made longer or shorter along themselves, the torso and the head are
//# scaled along the axes of the body, hands and feet as a whole. The joints are found by their Rigify names; the ones
//# that the body does not have are left out.
//#
//# Only visual: The mesh is lifted (the body component, not the character), so that longer or shorter legs and other
//# feet still stand on the ground. Nothing of the movement changes (e.g., its capsule or its eye heights).
//#
//# The shape is not saved. It lives as long as the actor.
//#
DECLARE_JAFG_CLASS()
class JGC_API AHumanoidShapeComponent : public Jafg::AActorComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(AHumanoidShapeComponent)

public:

    virtual void OnAttach(Jafg::AActor& InOwner) override;

    NODISCARD FORCEINLINE constexpr LHumanoidShape const& GetShape() const noexcept { return this->Shape; }
    //# Changes the body at once. Values are clamped to the ranges of #GetHumanoidShapeParameters.
    void SetShape(LHumanoidShape const& NewShape);

    //# The first skeletal mesh below the root. Null if the character has no body.
    NODISCARD Jafg::ASkeletalMeshComponent* GetBody() const noexcept;

private:

    void Apply();

    LHumanoidShape Shape;

    //# How far #Apply moved the body up, so that it can be changed again (by others as well). In meters.
    f32 AppliedLift{};
};

} /* ~Namespace Jgc */
