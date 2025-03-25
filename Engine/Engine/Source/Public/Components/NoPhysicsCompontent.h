// Copyright mzoesch. All rights reserved.

#pragma once

#include "Physics/PhysicCompontent.h"

namespace Jafg
{

class ENGINE_API LNoPhysicsComponent final : public LPhysicsComponent
{
public:
    LNoPhysicsComponent() = default;
    DEFAULT_REALLOC_OF_ANY_FORM(LNoPhysicsComponent)
    ~LNoPhysicsComponent() override = default;

    bool Overlaps(const LVector& Point) const override { return false; }
    bool Overlaps(const LVector& Point, const float Radius) const override { return false; }
    bool Overlaps(const LPhysicsComponent& Other) const override { return false; }
    bool Sweep(const LVector& Start, const LVector& End, LHitResult& OutHit) const override { return false; }

    static auto GetUsableClass() -> LNoPhysicsComponent*;
};

} /* ~Namespace Jafg */
