// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/WorldObject.h"
#include "Engine/TickableObject.h"
#include "Components/ActorComponent.h"
#include "Actor.generated.h"

namespace Jafg
{

class LRendererComponent;
class LPhysicsComponent;

DECLARE_JAFG_CLASS()
class ENGINE_API AActor : public JWorldObject, public LTickableObject
{
    GENERATED_CLASS_BODY()

    friend LWorld;

protected:

    DEFAULT_OBJECT_CTOR(AActor)
    DEFAULT_OBJECT_CDR_CTOR(AActor)

public:

    virtual void BeginLife() override { Super::BeginLife(); }
    virtual void Tick(const f32 DeltaTime) override { check( this->IsGarbage() == false ) }
    virtual void EndLife() override;

    virtual void OnGarbage(ECxxRecordTearDownReason::Type Reason, LClassOuter& PreviousOuter) override;

    template<typename T, typename... TArgs> requires std::is_base_of_v<LActorComponent, T> && std::is_constructible_v<T, TArgs...>
    FORCEINLINE decltype(auto) EmplaceComponent(TArgs&&... Args) noexcept { return this->Components.emplace_back<T>(std::forward<TArgs>(Args)...); }

    FORCEINLINE auto const& GetComponents() const noexcept { return this->Components; }

    NODISCARD FORCEINLINE bool CanEverTick() const noexcept { return this->bCanEverTick; }
    NODISCARD FORCEINLINE bool ShouldTick() const noexcept { return this->bShouldTick; }
    FORCEINLINE void SetShouldTick(bool bInShouldTick) noexcept { this->bShouldTick = bInShouldTick; }

protected:

    FORCEINLINE void SetEverTickConstructorOnlyFlag() { this->bCanEverTick = true; }
    FORCEINLINE void CancelEverTickConstructorOnlyFlag() { this->bCanEverTick = false; }

private:

    //# Make this virtual private to not confuse it with #ShouldTick.
    FORCEINLINE virtual bool ShouldTickableObjectTick() const override final { return this->ShouldTick(); }

    //#
    //# Whether this Actor should ever be able to tick or not.
    //# This bool flag can only be set in the constructor of the actor - new objects of this class
    //# will not be registered in the context tickable registry.
    //#
    bool bCanEverTick : 1 = false;

    //#
    //# Whether this actor should tick now or not. This flag does nothing if bCanEverTick is false.
    //#
    bool bShouldTick : 1  = true;

    struct LComponentArray
    {
        constexpr LComponentArray() noexcept = default;
        constexpr LComponentArray(LComponentArray const& Other) noexcept
        {
            // TODO: Implement this...
            check( Other.Elements.empty() && "Currently not possible..." )
        }
        constexpr ~LComponentArray() noexcept = default;
        template<typename T, typename ...TArgs> requires std::is_base_of_v<LActorComponent, T> && std::is_constructible_v<T, TArgs...>
        inline decltype(auto) emplace_back(TArgs&&... Args) noexcept
        {
            return this->Elements.emplace_back(std::make_unique<T>(std::forward<TArgs>(Args)...));
        }

        NODISCARD FORCEINLINE constexpr decltype(auto) begin() const noexcept
        {
            return this->Elements.begin();
        }
        NODISCARD FORCEINLINE constexpr decltype(auto) end() const noexcept
        {
            return this->Elements.end();
        }

        TArray<TUnique<LActorComponent>> Elements;
    } Components;
};

template<typename TActor> requires(std::is_base_of_v<AActor, TActor>)
FORCEINLINE TActor* SpawnActor(LWorld* World);
template<typename TActor> requires(std::is_base_of_v<AActor, TActor>)
FORCEINLINE TActor* SpawnActor(LWorld* World, TSubclassOf<TActor> Class);
FORCEINLINE AActor* SpawnActor(LWorld* World, TSubclassOf<AActor> Class);
FORCEINLINE AActor* SpawnActor(LWorld* World, LString const& ClassName);

template<typename TActor> requires(std::is_base_of_v<AActor, TActor>)
FORCEINLINE TActor* SpawnDeferredActor(LWorld* World);
template<typename TActor> requires(std::is_base_of_v<AActor, TActor>)
FORCEINLINE TActor* SpawnDeferredActor(LWorld* World, TSubclassOf<TActor> Class);
FORCEINLINE AActor* SpawnDeferredActor(LWorld* World, TSubclassOf<AActor> Class);
FORCEINLINE AActor* SpawnDeferredActor(LWorld* World, LString const& ClassName);

FORCEINLINE void MakeDeferredActorFinal(AActor* Actor) { MakeDeferredObjectFinal(Actor);}


///////////////////////////////////////////////////////////////////////////////
// Impl
///////////////////////////////////////////////////////////////////////////////

template<typename TActor> requires(std::is_base_of_v<AActor, TActor>)
FORCEINLINE TActor* SpawnActor(LWorld* World) { return SpawnActor<TActor>(World, TSubclassOf<TActor>(TActor::StaticClass())); }
template<typename TActor> requires(std::is_base_of_v<AActor, TActor>)
FORCEINLINE TActor* SpawnActor(LWorld* World, TSubclassOf<TActor> Class) { return StaticCastChecked<TActor>(SpawnActor(World, TSubclassOf<AActor>{Class})); }
FORCEINLINE AActor* SpawnActor(LWorld* World, TSubclassOf<AActor> Class) { AActor* Out{ SpawnDeferredActor(World, Class) }; MakeDeferredActorFinal(Out); return Out; }
FORCEINLINE AActor* SpawnActor(LWorld* World, LString const& ClassName) { return SpawnActor(World, Private::GetGlobalCxxRecordRegistry().GetClassByNameChecked(ClassName)->StaticClass); }

template<typename TActor> requires(std::is_base_of_v<AActor, TActor>)
FORCEINLINE TActor* SpawnDeferredActor(LWorld* World) { return SpawnDeferredActor<TActor>(World, TSubclassOf<TActor>(TActor::StaticClass())); }
template<typename TActor> requires(std::is_base_of_v<AActor, TActor>)
FORCEINLINE TActor* SpawnDeferredActor(LWorld* World, TSubclassOf<TActor> Class) { return StaticCastChecked<TActor>(SpawnDeferredActor(World, static_cast<TSubclassOf<AActor>>(Class))); }
FORCEINLINE AActor* SpawnDeferredActor(LWorld* World, TSubclassOf<AActor> Class)
{
    check( World && Class.HasClass() && Class.IsValidType() )
    AActor* Out{ NewDeferredObject<AActor>(World, *Class.GetClass()) };
    if (Out->CanEverTick()) { World->RegisterTickableObject(Out); }
    return Out;
}
FORCEINLINE AActor* SpawnDeferredActor(LWorld* World, LString const& ClassName) { return SpawnDeferredActor(World, Private::GetGlobalCxxRecordRegistry().GetClassByNameChecked(ClassName)->StaticClass); }

} /* ~Namespace Jafg */
