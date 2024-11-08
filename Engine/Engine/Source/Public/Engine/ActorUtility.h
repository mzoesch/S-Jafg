// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Actor.h"
#include "Engine/World.h"

namespace Jafg
{

class AActor;

namespace Private
{

struct LWorldMiscellaneousAccessor;

} /* ~Namespace Private. */

template <typename TActor>
FORCEINLINE auto SpawnActor(LWorld* InContext) -> TActor*;
template <typename TActor>
FORCEINLINE auto SpawnActor(LWorld* InContext, const LObjectClass* InStaticClass) -> TActor*;
FORCEINLINE auto SpawnActor(LWorld* InContext, const LObjectClass* InStaticClass) -> AActor*;

template <typename TActor>
FORCEINLINE auto SpawnDeferredActor(LWorld* InContext) -> TActor*;
template <typename TActor>
FORCEINLINE auto SpawnDeferredActor(LWorld* InContext, const LObjectClass* InStaticClass) -> TActor*;
FORCEINLINE auto SpawnDeferredActor(LWorld* InContext, const LObjectClass* InStaticClass) -> AActor*;

FORCEINLINE void MakeDeferredActorFinal(AActor* InActor);

namespace Private
{

struct LWorldMiscellaneousAccessor final
{
    FORCEINLINE static AActor* SpawnActor(LWorld* InContext, const LObjectClass* InStaticClass);
    FORCEINLINE static AActor* SpawnDeferredActor(LWorld* InContext, const LObjectClass* InStaticClass);
};

} /* ~Namespace Private. */

template <typename TActor>
TActor* SpawnActor(LWorld* InContext)
{
    return SpawnActor<TActor>(InContext, TActor::StaticClass());
}

template <typename TActor>
TActor* SpawnActor(LWorld* InContext, const LObjectClass* InStaticClass)
{
    return reinterpret_cast<TActor*>(SpawnActor(InContext, InStaticClass));
}

AActor* SpawnActor(LWorld* InContext, const LObjectClass* InStaticClass)
{
    return Private::LWorldMiscellaneousAccessor::SpawnActor(InContext, InStaticClass);
}

template <typename TActor>
TActor* SpawnDeferredActor(LWorld* InContext)
{
    return SpawnDeferredActor<TActor>(InContext, TActor::StaticClass());
}

template <typename TActor>
TActor* SpawnDeferredActor(LWorld* InContext, const LObjectClass* InStaticClass)
{
    return reinterpret_cast<TActor*>(SpawnDeferredActor(InContext, InStaticClass));
}

AActor* SpawnDeferredActor(LWorld* InContext, const LObjectClass* InStaticClass)
{
    return Private::LWorldMiscellaneousAccessor::SpawnDeferredActor(InContext, InStaticClass);
}

void MakeDeferredActorFinal(AActor* InActor)
{
    MakeDeferredObjectFinal(InActor);
}

FORCEINLINE AActor* Private::LWorldMiscellaneousAccessor::SpawnActor(LWorld* InContext, const LObjectClass* InStaticClass)
{
    AActor* Actor = SpawnDeferredActor(InContext, InStaticClass);
    MakeDeferredActorFinal(Actor);
    return Actor;
}

FORCEINLINE AActor* Private::LWorldMiscellaneousAccessor::SpawnDeferredActor(LWorld* InContext, const LObjectClass* InStaticClass)
{
    check( InContext )

    AActor* Actor = NewDeferredObject<AActor>(InContext, InStaticClass);
    InContext->Actors.Add(Actor);

    if (Actor->CanEverTick())
    {
        InContext->TickableObjects.Add(Actor);
    }

    return Actor;
}

} /* ~Namespace Jafg. */
