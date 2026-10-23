// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

namespace Detail
{

struct LPhysicsSubsystem;

} /* ~Namespace Detail */

enum struct EPhysicsMotion: u8
{
    //# Non movable.
    Static,
    //# Movable using velocities only, does not respond to forces.
    Kinematic,
    //# Responds to forces as a normal physics object.
    Dynamic,
};

enum struct EPhysicsMotionQuality: u8
{
    //# Update the body in discrete steps. Body will tunnel through thin objects if its velocity is high enough.
    //# This is the cheapest way of simulating a body.
    Discrete,

    //# Update the body using linear casting. When stepping the body, its collision shape is cast from
    //# start to destination using the starting rotation. The body will not be able to tunnel through thin
    //# objects at high velocity, but tunneling is still possible if the body is long and thin and has high
    //# angular velocity. Time is stolen from the object (which means it will move up to the first collision
    //# and will not bounce off the surface until the next integration step). This will make the body appear
    //# to go slower when it collides with high velocity. In order to not get stuck, the body is always
    //# allowed to move by a fraction of it's inner radius, which may eventually lead it to pass through geometry.
    //#
    //# Note that if you're using a collision listener, you can receive contact added/persisted notifications of contacts
    //# that may in the end not happen. This happens between bodies that are using casting: If bodies A and B collide at t1
    //# and B and C collide at t2 where t2 < t1 and A and C don't collide. In this case you may receive an incorrect contact
    //# point added callback between A and B (which will be removed the next frame).
    LinearCast,
};

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
SERDE_ENUM(ESoftBodyConstraintColor, ConstraintType, ConstraintGroup, ConstraintOrder)

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
SERDE_ENUM(EShapeColor, Instance, ShapeType, MotionType, Sleep, Island, Material)
#endif /* !JAFG_IN_SHIPPING */

} /* ~Namespace Physx */

} /* ~Namespace Jafg */

JAFG_PREF_OF(Jafg::Physx::ESoftBodyConstraintColor)
JAFG_PREF_OF(Jafg::Physx::EShapeColor)
