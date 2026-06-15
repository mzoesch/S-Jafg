// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

class AActor;
class AActorComponent;

struct LHitResult final
{
    //# Hit actor.
    AActor& Actor;
    //# Hit component.
    AActorComponent& Component;
    //# Hit location in world space.
    LWorldVec3 GlobalWorldLocation;
    //# Optional hit surface normal.
    std::optional<LWorldVec3> SurfaceNormal;
};

enum struct ECollisionChannelBits
{
    Invalid  = 0x0 << 0,
    Static   = 0x1 << 1,
    Dynamic  = 0x1 << 2,
    Pawn     = 0x1 << 3,
};
ENUM_STRUCT_FLAGS(ECollisionChannelBits, ECollisionChannels)

struct LCollisionQueryParams final
{
    bool bSingleHit:1{};
    bool bSortByDistance:1{};
    bool bSortDescending:1{};
};

} /* ~Namespace Jafg */
