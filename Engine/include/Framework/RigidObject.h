// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/PhysicsCore.h"

namespace JPH
{

class Body;
class BodyInterface;

} /* ~Namespace JPH */

namespace Jafg
{

//#
//# Each rigid body has a shape attached that determines its collision.
//# The following are available (in order of computational complexity):
//#
enum struct EShape: u8
{
    //# A sphere centered around zero.
    Sphere,
    //# A box centered around zero.
    Box,
    //# A capsule centered around zero.
    Capsule,
    //# A capsule with different radii at the bottom and top.
    TaperedCapsule,
    //# A cylinder shape. Note that cylinders are the least stable of all shapes, so use another shape if possible.
    Cylinder,
    //# A cylinder with different radii at the bottom and top. Note that cylinders are the least stable of
    //# all shapes, so use another shape if possible.
    TaperedCylinder,
    //# A convex hull defined by a set of points.
    ConvexHull,
    //# A single triangle. Use a MeshShape if you have multiple triangles.
    Triangle,
    //# An infinite plane. Negative half space is considered solid.
    Plane,
    //# A shape containing other shapes. This shape is constructed once and cannot be changed afterwards.
    MutableCompound,
    //# A shape consisting of triangles. They are mostly used for static geometry.
    Mesh,
    //# A shape consisting of NxN points that define the height at each point, very suitable for representing
    //# hilly terrain. Any body that uses this shape needs to be static.
    HeightField,
    //# A shape that collides with nothing and that can be used as a placeholder or for dummy bodies.
    Empty,

    //#
    //# Next to this there are a number of decorator shapes that change the behavior of their children:
    //#

    //# This shape can scale a child shape. Note that if a shape is rotated first and then scaled,
    //# you can introduce shearing which is not supported by the library.
    Scaled,
    //# This shape can rotate and translate a child shape, it can e.g. be used to offset a sphere from the origin.
    RotatedTranslated,
    //# This shape does not change its child shape but it does shift the calculated center of mass for that shape.
    //# It allows you to e.g. shift the center of mass of a vehicle down to improve its handling.
    OffsetCenterOfMass,
};

enum struct EMotion: u8
{
    //# Non-movable.
    Static,
    //# Child shapes are organized in a tree to speed up collision detection.
    StaticCompound,
    //# A shape containing other shapes. This shape can be constructed/changed at runtime and trades
    //# construction time for runtime performance. Child shapes are organized in a list to make modification easy.
    //# Movable using velocities only, does not respond to forces.
    Kinematic,
    //# Responds to forces as a normal physics object.
    Dynamic,
};

//# Determines the behavior of a rigid body when added to a simulation.
enum struct ESimulationAddingBehavior: u8
{
    //# Will always active the new body.
    Activate,
    //# Will leave the activation state as is.
    Preserve,
};

struct LRigidObjectCreateInfo final
{
    LWorldVec3 Translation;
    LWorldQuat Rotation;
    EMotion Motion;
    EPhysicsLayer::value_type Layer;
};

struct LRigidObject final
{
    constexpr LRigidObject() noexcept = default;
    constexpr LRigidObject(std::nullptr_t) noexcept {}
    constexpr LRigidObject(LRigidObject const&) noexcept = default;
    constexpr LRigidObject(LRigidObject&& O) noexcept: Body{std::exchange(O.Body, nullptr)} {}
    constexpr LRigidObject& operator=(std::nullptr_t) noexcept { this->Body = nullptr; return *this; }
    constexpr LRigidObject& operator=(LRigidObject const&) noexcept = default;
    constexpr LRigidObject& operator=(LRigidObject&& O) noexcept { this->Body = std::exchange(O.Body, nullptr); return *this; }
    constexpr ~LRigidObject() noexcept = default;

    NODISCARD static FORCEINLINE constexpr LRigidObject FromNative(JPH::Body* Ref) noexcept { return LRigidObject{Ref}; }

    NODISCARD constexpr bool IsValid() const noexcept { return !!this->Body; }
    NODISCARD constexpr explicit operator bool() const noexcept { return !!this->Body; }

    NODISCARD constexpr explicit operator JPH::Body&() const noexcept { check(this->Body) return *this->Body; }
    NODISCARD constexpr explicit operator JPH::Body*() const noexcept { return this->Body; }
    NODISCARD constexpr JPH::Body& operator*() noexcept { check(this->Body) return *this->Body; }
    NODISCARD constexpr JPH::Body const& operator*() const noexcept { check(this->Body) return *this->Body; }
    NODISCARD constexpr JPH::Body* operator->() noexcept { check(this->Body) return this->Body; }
    NODISCARD constexpr JPH::Body const* operator->() const noexcept { check(this->Body) return this->Body; }

private:

    constexpr explicit LRigidObject(JPH::Body* Ref) noexcept : Body{Ref} {}

    JPH::Body* Body{};
};

} /* ~Namespace Jafg */
