// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

class AActor;

struct LHitResult final
{
    //#
    //# Whether this hit result is valid.
    //# If the #Actor is null then all other fields are in UB.
    //#
    FORCEINLINE constexpr bool IsValid() const noexcept { return this->Actor != nullptr; }

    FORCEINLINE constexpr void Reset() noexcept { this->Actor = nullptr; }

    template<typename TActor>
    FORCEINLINE TActor* GetActor() { check( this->IsValid() ) return DynamicCast<TActor>(this->Actor); }
    template<typename TActor>
    FORCEINLINE TActor const* GetActor() const { check( this->IsValid() ) return DynamicCast<TActor>(this->Actor); }

    //#
    //# Hit actor.
    //#
    AActor* Actor{};

    //#
    //# Hit location in world space.
    //#
    LWorldVec3 GlobalWorldLocation;

    //#
    //# Optional hit surface normal.
    //#
    TOptional<LWorldVec3> SurfaceNormal;
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
