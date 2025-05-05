// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Engine/ObjectBaseUtility.h"

namespace Jafg
{

class AActor;

struct LHitResult final
{
    LHitResult() { this->Initialize(); }

    FORCEINLINE bool IsValid() const { return this->Actor != nullptr; }

    void Initialize();
    void Reset();

    template <typename TActor>
    FORCEINLINE auto GetActor() -> TActor* { return DynamicCast<TActor>(this->Actor); }
    template <typename TActor>
    FORCEINLINE auto GetActor() const -> const TActor* { return DynamicCast<TActor>(this->Actor); }

    //#
    //# Hit actor.
    //#
    AActor* Actor = nullptr;

    //#
    //# Hit location in world space.
    //#
    LVector GlobalWorldLocation = LVector::ZeroVector;

    //#
    //# Optional hit surface normal.
    //#
    TOptional<LVector> SurfaceNormal;
};

inline void LHitResult::Initialize()
{
}

inline void LHitResult::Reset()
{
    this->Actor = nullptr;
    this->GlobalWorldLocation = LVector::ZeroVector;
    this->SurfaceNormal.Reset();

    return;
}

namespace ECollisionChannel
{

enum Type : u8
{
    Invalid,
    Static,
    Dynamic,
    Pawn,
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
