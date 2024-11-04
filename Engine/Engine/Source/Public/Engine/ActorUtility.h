// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Actor.h"

namespace Jafg
{

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

template <typename TActor>
TActor* SpawnActor(LWorld* InContext)
{
    return nullptr;
}

template <typename TActor>
TActor* SpawnActor(LWorld* InContext, const LObjectClass* InStaticClass)
{
    return nullptr;
}

AActor* SpawnActor(LWorld* InContext, const LObjectClass* InStaticClass)
{
    return nullptr;
}

AActor* SpawnDeferredActor(LWorld* InContext, const LObjectClass* InStaticClass)
{
    return nullptr;
}

template <typename TActor>
TActor* SpawnDeferredActor(LWorld* InContext)
{
    return nullptr;
}

template <typename TActor>
TActor* SpawnDeferredActor(LWorld* InContext, const LObjectClass* InStaticClass)
{
    return nullptr;
}

} /* ~Namespace Jafg. */
