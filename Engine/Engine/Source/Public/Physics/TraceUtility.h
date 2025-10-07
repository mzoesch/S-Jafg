// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBaseUtility.h"

namespace Jafg
{

class AActor;

struct LHitResult final
{
    FORCEINLINE bool IsValid() const { return this->Actor != nullptr; }

    void Reset();

    template <typename TActor>
    FORCEINLINE TActor* GetActor() { return DynamicCast<TActor>(this->Actor); }
    template <typename TActor>
    FORCEINLINE const TActor* GetActor() const { return DynamicCast<TActor>(this->Actor); }

    //#
    //# Hit actor.
    //#
    AActor* Actor { nullptr };

    //#
    //# Hit location in world space.
    //#
    LVector GlobalWorldLocation;

    //#
    //# Optional hit surface normal.
    //#
    TOptional<LVector> SurfaceNormal;
};

inline void LHitResult::Reset()
{
    this->Actor = nullptr;
    this->GlobalWorldLocation = LVector::ZeroVector;
    this->SurfaceNormal.reset();

    return;
}

namespace ECollisionChannel
{

enum Type : u8
{
    Invalid  = 0x0 << 0,
    Static   = 0x1 << 1,
    Dynamic  = 0x1 << 2,
    Pawn     = 0x1 << 3,
};

} /* ~Namespace ECollisionChannel */
ENUM_CLASS_FLAGS(ECollisionChannel::Type)

struct LCollisionQueryParams final
{
    bool bSingleHit      { false };
    bool bSortByDistance { false };
    bool bSortDescending { false };
};

} /* ~Namespace Jafg */
