// Copyright mzoesch. All rights reserved.

#pragma once

#include "Async/TaskUtility.h"
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
FORCEINLINE TActor* SpawnActor(LWorld* InContext);
template <typename TActor>
FORCEINLINE TActor* SpawnActor(LWorld* InContext, const LObjectClass* InStaticClass);
FORCEINLINE AActor* SpawnActor(LWorld* InContext, const LObjectClass* InStaticClass);

template <typename TActor>
FORCEINLINE TActor* SpawnDeferredActor(LWorld* InContext);
template <typename TActor>
FORCEINLINE TActor* SpawnDeferredActor(LWorld* InContext, const LObjectClass* InStaticClass);
FORCEINLINE AActor* SpawnDeferredActor(LWorld* InContext, const LObjectClass* InStaticClass);

FORCEINLINE void MakeDeferredActorFinal(AActor* InActor);

namespace Private
{

struct LWorldMiscellaneousAccessor final
{
    FORCEINLINE static AActor* SpawnActor(LWorld* InContext, const LObjectClass* InStaticClass);
    FORCEINLINE static AActor* SpawnActorWeak(LWorld* InContext, const LObjectClass* InStaticClass);
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

FORCEINLINE AActor* Private::LWorldMiscellaneousAccessor::SpawnActorWeak(LWorld* InContext, const LObjectClass* InStaticClass)
{
    check( InContext )
    check( Tasks::IsOnMasterThread() )

    AActor* Actor = NewDeferredObject<AActor, true, false>(InContext, InStaticClass);

    if (Actor->CanEverTick())
    {
        InContext->TickableObjects.Add(Actor);
    }

    return Actor;
}

FORCEINLINE AActor* Private::LWorldMiscellaneousAccessor::SpawnDeferredActor(LWorld* InContext, const LObjectClass* InStaticClass)
{
    AActor* Actor = SpawnActorWeak(InContext, InStaticClass);
    InContext->Actors.Add(Actor);
    return Actor;
}

} /* ~Namespace Jafg. */
