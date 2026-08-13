// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

namespace Detail
{

struct LPhysicsSubsystem;

} /* ~Namespace Detail */

namespace EPhysicsLayer
{

#ifndef JAFG_PHYSICS_LAYER_BITS
    #error "JAFG_PHYSICS_LAYER_BITS is not defined."
#elif JAFG_PHYSICS_LAYER_BITS == 16
    typedef u16 value_type;
#elif JAFG_PHYSICS_LAYER_BITS == 32
    typedef u32 value_type;
#else /* JAFG_PHYSICS_LAYER_BITS != 16 && JAFG_PHYSICS_LAYER_BITS != 32 */
    #error "JAFG_PHYSICS_LAYER_BITS is invalid."
#endif /* JAFG_PHYSICS_LAYER_BITS != 16 && JAFG_PHYSICS_LAYER_BITS != 32 */

inline constexpr value_type Static{ 0 };
inline constexpr value_type Dynamic{ 1 };

inline constexpr value_type Count{ 2 };

} /* ~Namespace PhysicsLayer */

namespace EPhysicsBroadPhase
{

typedef u8 value_type;

inline constexpr value_type Static{ 0 };
inline constexpr value_type Dynamic{ 1 };

inline constexpr value_type Count{ 2 };

} /* ~Namespace PhysicsBroadPhase */

namespace Physx
{

//# Objects closes than this distance are considered to be colliding.
inline constexpr f32 DefaultCollisionTolerance{ maths::not_so_small_number_f };
//#
//# A factor that determines the accuracy of the penetration depth calculation. If the change of the
//# squared distance is less than tolerance*current_penetration_depth^2 the algorithm will terminate.
//# Stop when there's less than 1% change
//#
inline constexpr f32 DefaultPenetrationTolerance{ maths::not_so_small_number_f };
//# How much padding to add around objects
constexpr float DefaultConvexRadius{ 0.05f };

#if !JAFG_IN_SHIPPING
enum struct ESoftBodyConstraintColor
{
    //# Draw different types of constraints in different colors.
    ConstraintType,
    //# Draw constraints in the same group in the same color, non-parallel group will be red.
    ConstraintGroup,
    //# Draw constraints in the same group in the same color, non-parallel group will be red, and order within each group will be indicated with gradient.
    ConstraintOrder,
};

enum struct EShapeColor
{
    //# Random color per instance.
    Instance,
    //# Convex = green, scaled = yellow, compound = orange, mesh = red.
    ShapeType,
    //# Static = grey, keyframed = green, dynamic = random color per instance.
    MotionType,
    //# Static = grey, keyframed = green, dynamic = yellow, sleeping = red.
    Sleep,
    //# Static = grey, active = random color per island, sleeping = light grey.
    Island,
    //# Color as defined by the PhysicsMaterial of the shape.
    Material,
};
#endif /* !JAFG_IN_SHIPPING */

} /* ~Namespace Physx */

} /* ~Namespace Jafg */

JAFG_PREF_OF(Jafg::Physx::ESoftBodyConstraintColor)
JAFG_PREF_OF(Jafg::Physx::EShapeColor)
