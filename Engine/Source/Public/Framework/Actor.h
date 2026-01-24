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

private:

    struct LComponentArray
    {
        constexpr LComponentArray() noexcept = default;
        constexpr LComponentArray(LComponentArray const& Other) noexcept
        {
            // TODO: Implement this...
            check( Other.Elements.empty() && "Currently not possible..." )
        }
        constexpr LComponentArray(LComponentArray&& Other) noexcept
            : Elements(std::move(Other.Elements))
        {
            check( Other.Elements.empty() )
        }
        constexpr LComponentArray& operator=(LComponentArray&& Rhs) noexcept
        {
            this->Elements = std::move(Rhs.Elements);
            check( Rhs.Elements.empty() )
            return *this;
        }
        constexpr ~LComponentArray() noexcept = default;

        NODISCARD FORCEINLINE constexpr decltype(auto) begin() const noexcept
        {
            return this->Elements.begin();
        }
        NODISCARD FORCEINLINE constexpr decltype(auto) end() const noexcept
        {
            return this->Elements.end();
        }

        template<typename T> requires std::is_base_of_v<JActorComponent, T>
        inline decltype(auto) emplace_back_deferred(LClassOuter* Outer, TFunction<void(T& Comp)> const& Callback)
        {
            T* Comp{static_cast<T*>(&*this->Elements.emplace_back(TJxxUnique<T>{NewDeferredObject<T>(Outer)}))};
            if (Callback.IsValid())
            {
                Callback(*Comp);
            }
            return Comp;
        }

        template<typename T> requires std::is_base_of_v<JActorComponent, T>
        inline decltype(auto) emplace_back(AActor* Who, LClassOuter* Outer, TFunction<void(T& Comp)> const& Callback)
        {
            auto* Comp{this->emplace_back_deferred<T>(Outer, Callback)};
            MakeDeferredObjectFinal(Comp);
            Comp->OnAttach(Who);
            return Comp;
        }

    private:

        TArray<TJxxUnique<JActorComponent>> Elements;
    };

protected:

    DEFAULT_OBJECT_CTOR(AActor)
    DEFAULT_OBJECT_CDR_CTOR(AActor)

public:

    virtual void BeginLife() override;
    virtual void Tick(const f32 DeltaTime) override { check( this->IsGarbage() == false ) }
    virtual void EndLife() override;

    virtual void OnGarbage(ECxxRecordTearDownReason::Type Reason, LClassOuter& PreviousOuter) override;

    template<typename T> requires std::is_base_of_v<JActorComponent, T>
    FORCEINLINE T* EmplaceDeferredComponent(TFunction<void(T& Comp)> const& Callback = {}) noexcept
    {
        check( this->HasBegunLife() == false && "Deferred component placement is not allowed for already living objects.")
        return this->Components.emplace_back_deferred<T>(this->GetOuterChecked(), Callback);
    }

    template<typename T> requires std::is_base_of_v<JActorComponent, T>
    FORCEINLINE T* EmplaceComponent(TFunction<void(T& Comp)> const& Callback = {}) noexcept
    {
        check( this->HasBegunLife() && "Non-deferred component placement is not allowed for living objects.")
        return this->Components.emplace_back<T>(this, this->GetOuterChecked(), Callback);
    }

    template<typename T> requires std::is_base_of_v<JActorComponent, T>
    FORCEINLINE T* GetComponent() const noexcept
    {
        for (auto const& Comp : this->Components)
        {
            if (Comp->IsA<T>())
            {
                return StaticCastChecked<T>(&*Comp);
            }
        }

        return {};
    }

    FORCEINLINE auto const& GetComponents() const noexcept { return this->Components; }

    NODISCARD FORCEINLINE bool CanEverTick() const noexcept { return this->bCanEverTick; }
    NODISCARD FORCEINLINE bool ShouldTick() const noexcept { return this->bShouldTick; }
    FORCEINLINE void SetShouldTick(bool bInShouldTick) noexcept { this->bShouldTick = bInShouldTick; }

protected:

    FORCEINLINE void SetEverTickConstructorOnlyFlag() noexcept { this->bCanEverTick = true; }
    FORCEINLINE void CancelEverTickConstructorOnlyFlag() noexcept { this->bCanEverTick = false; }

private:

    //# Make this virtual private to not confuse it with #ShouldTick.
    FORCEINLINE virtual bool ShouldTickableObjectTick() const override final { return this->ShouldTick(); }

    //#
    //# Whether this Actor should ever be able to tick or not.
    //# This bool flag can only be set in the constructor of the actor - new objects of this class
    //# will not be registered in the context tickable registry.
    //#
    bool bCanEverTick:1{false};

    //#
    //# Whether this actor should tick now or not. This flag does nothing if bCanEverTick is false.
    //#
    bool bShouldTick:1{true};

    LComponentArray Components;
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
