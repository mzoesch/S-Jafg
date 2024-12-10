// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
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

    AActor* Actor               = nullptr;
    LVector GlobalWorldLocation = LVector::Zero();
    LVector SurfaceNormal       = LVector::Zero();
};

inline void LHitResult::Initialize()
{
}

inline void LHitResult::Reset()
{
    this->Actor = nullptr;
    this->GlobalWorldLocation = LVector::Zero();
    this->SurfaceNormal       = LVector::Zero();

    return;
}

namespace ECollisionChannel
{

enum Type : uint8
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
    bool bSingleHit = false;
};

} /* ~Namespace Jafg */
