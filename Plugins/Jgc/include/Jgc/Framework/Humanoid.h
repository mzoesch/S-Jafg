// Copyright mzoesch. All rights reserved.

#pragma once

#include "JgcMinimal.afx"

namespace Jafg
{
class AActor;
class LWorld;
class ASkeletalMeshComponent;
} /* ~Namespace Jafg */

namespace Jgc
{

//# What a humanoid body is made of. All paths are relative to the content dir of Jgc.
struct LHumanoidBodyInfo final
{
    LStringView Mesh{ "Rigged/Human/FacelessMale.glb" };
    //# The files with the clips. @see Documentation/Animation.md
    TArray<LStringView> Animations{ "Rigged/Human/FacelessMale_Locomotion.glb" };
    LStringView PbrMaterial{ "Materials/Skin" };
    //# The joints that are drawn in first person (prefixes of their names). Rigify names by default.
    TArray<LString> FirstPersonJoints{ "DEF-upper_arm", "DEF-forearm", "DEF-hand", "DEF-palm", "DEF-thumb", "DEF-f_" };
    //#
    //# Whether to add a third person view behind the head (an eye on a camera boom, #ACameraBoomComponent).
    //# Only worth it for characters that humans possess. @see #ACharacterInputComponent::ToggleView
    //#
    bool bThirdPersonView{};
};

//#
//# Gives a character a body: A skeletal mesh below the root that is animated from the movement of the character
//# (#ACharacterAnimationComponent) and can be reshaped (#AHumanoidShapeComponent). The head eye becomes a
//# first-person eye, from which only the arms are seen.
//#
//# The character must have its root at the feet, a head eye (the first eye directly below the root) and a
//# #Jafg::ACharacterMovementComponent. Works for living actors and inside the callback of a deferred spawn.
//#
//# @return The body. Null if the mesh could not be loaded (logged); the character then stays without a body.
//#
JGC_API Jafg::ASkeletalMeshComponent* EquipHumanoidBody(Jafg::AActor& Character, LHumanoidBodyInfo const& Info = {});

//#
//# Spawns a humanoid character that runs between #Waypoints (#APatrolComponent). E.g., to see animations without
//# a second player.
//#
//# @param Waypoints At least one. The character starts at the first one, with its feet there.
//#
JGC_API Jafg::AActor& SpawnPatrollingHumanoid(Jafg::LWorld& World, TArray<LWorldVec3> Waypoints, LHumanoidBodyInfo const& Info = {});

} /* ~Namespace Jgc */
