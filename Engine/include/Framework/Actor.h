// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/WorldObject.h"
#include "Engine/TickableObject.h"
#include "Framework/ActorComponent.h"
#include "Framework/SceneComponent.h"
#include "Actor.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API AActor : public AWorldObject, private LTickableObject
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(AActor)

public:

    virtual void BeginLife() override;
    virtual void Tick(f32 Dt) override
    {
        check(this->bLives && this->_IsGarbage() == false)
        for (auto& Comp: this->Components)
        {
            if (Comp->bTick)
            {
                Comp->ParentTick(Dt);
            }
        }
    }
    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

    //# Emplace a new actor component to this actor that is not a scene component.
    template<typename TActorComponent> requires(std::is_base_of_v<AActorComponent, TActorComponent> && !std::is_base_of_v<ASceneComponent, TActorComponent>)
    FORCEINLINE TActorComponent& EmplaceComponent(TFunction2<void(TActorComponent& Comp)> Callback = {})
    {
        auto& Result{*StaticCastChecked<TActorComponent>(&*this->Components.emplace_back(SpawnObject(TWorldStaticInit<TActorComponent>{.Outer=this->GetWorld()})))};
        if (Callback)
        {
            Callback(Result);
        }
        if (this->bLives)
        {
            Result.OnAttach(*this);
        }
        return Result;
    }
    template<typename TActorComponent = AActorComponent> requires(std::is_base_of_v<AActorComponent, TActorComponent> && !std::is_base_of_v<ASceneComponent, TActorComponent>)
    FORCEINLINE TActorComponent& EmplaceComponent(TSubclassOf<TActorComponent> Class, TFunction2<void(TActorComponent& Comp)> Callback = {})
    {
        check(Class.GetClass())
        auto& Result{*StaticCastChecked<TActorComponent>(&*this->Components.emplace_back(SpawnObject(CastTo<AActorComponent>{}, {this->GetWorld(), *Class.GetClass()})))};
        if (Callback)
        {
            Callback(Result);
        }
        if (this->bLives)
        {
            Result.OnAttach(*this);
        }
        return Result;
    }

    //# Emplace a new scene component to this actor under the given parent. The parent obviously has to be already attached to this actor.
    template<typename TSceneComponent = ASceneComponent> requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceSceneComponentTo(ASceneComponent& Parent, TFunction2<void(TSceneComponent& Comp)> Callback = {})
    {
        auto& Result{*StaticCastChecked<TSceneComponent>(&*this->Components.emplace_back(SpawnObject(TWorldStaticInit<TSceneComponent>{.Outer=this->GetWorld()})))};
        check(algo::contains(this->Components, &Parent, algo::unique_raw))
        Parent.Children.emplace_back(&Result);
        if (Callback)
        {
            Callback(Result);
        }
        if (this->bLives)
        {
            Result.OnAttach(*this);
        }
        return Result;
    }
    template<typename TSceneComponent = ASceneComponent> requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceSceneComponentTo(ASceneComponent& Parent, TSubclassOf<ASceneComponent> Class, TFunction2<void(TSceneComponent& Comp)> Callback = {})
    {
        check(Class.GetClass())
        auto& Result{*StaticCastChecked<TSceneComponent>(&*this->Components.emplace_back(SpawnObject(CastTo<AActorComponent>{}, {this->GetWorld(), *Class.GetClass()})))};
        check(algo::contains(this->Components, &Parent, algo::unique_raw))
        Parent.Children.emplace_back(&Result);
        if (Callback)
        {
            Callback(Result);
        }
        if (this->bLives)
        {
            Result.OnAttach(*this);
        }
        return Result;
    }

    //# Emplace the root component for this actor. Once the root component has been set. It can no longer be removed until the actor is destroyed.
    template<typename TSceneComponent = ASceneComponent> requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceRootComponent(TFunction2<void(TSceneComponent& Comp)> Callback = {})
    {
        auto& Result{*StaticCastChecked<TSceneComponent>(&*this->Components.emplace_back(SpawnObject(TWorldStaticInit<TSceneComponent>{.Outer=this->GetWorld()})))};
        check(!this->RootComponent)
        this->RootComponent = &Result;
        if (Callback)
        {
            Callback(Result);
        }
        if (this->bLives)
        {
            Result.OnAttach(*this);
        }
        return Result;
    }
    template<typename TSceneComponent = ASceneComponent> requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceRootComponent(TSubclassOf<ASceneComponent> Class, TFunction2<void(TSceneComponent& Comp)> Callback = {})
    {
        check(Class.GetClass())
        auto& Result{*StaticCastChecked<TSceneComponent>(&*this->Components.emplace_back(SpawnObject(CastTo<AActorComponent>{}, {this->GetWorld(), *Class.GetClass()})))};
        check(!this->RootComponent)
        this->RootComponent = &Result;
        if (Callback)
        {
            Callback(Result);
        }
        if (this->bLives)
        {
            Result.OnAttach(*this);
        }
        return Result;
    }

    template<typename T> requires std::is_base_of_v<AActorComponent, T>
    NODISCARD FORCEINLINE T* GetComponent() const noexcept
    {
        for (auto const& Comp: this->Components)
        {
            if (Comp->IsA<T>())
            {
                return StaticCastChecked<T>(&*Comp);
            }
        }
        return {};
    }
    template<typename T> requires std::is_base_of_v<AActorComponent, T>
    NODISCARD FORCEINLINE T& GetComponentChecked() const noexcept
    {
        auto* Result{this->GetComponent<T>()};
        check(Result)
        return *Result;
    }
    template<typename T> requires std::is_base_of_v<AActorComponent, T>
    NODISCARD FORCEINLINE T& GetComponentAsserted() const noexcept
    {
        auto* Result{this->GetComponent<T>()};
        jassert(Result)
        return *Result;
    }

    //# Remove a component that was previously emplaced to this actor.
    FORCEINLINE void RemoveComponent(AActorComponent& Component) noexcept
    {
        auto It{algo::find(this->Components, &Component, algo::unique_raw)};
        check(It != this->Components.end())
        check(this->bLives && "Components cannot be removed during construction.")
        this->Components.erase(It);
    }

    NODISCARD FORCEINLINE constexpr bool HasRootComponent() const noexcept { return !!this->RootComponent; }
    NODISCARD FORCEINLINE constexpr ASceneComponent& GetRootComponent() const noexcept { check(this->RootComponent) return *this->RootComponent; }
    NODISCARD FORCEINLINE TArray<TJxxUnique<AActorComponent>> const& GetComponents() const noexcept { return this->Components; }

    NODISCARD FORCEINLINE bool CanEverTick() const noexcept { return this->bCanEverTick; }
    NODISCARD FORCEINLINE bool ShouldTick() const noexcept { return this->bShouldTick; }
    FORCEINLINE void SetShouldTick(bool bInShouldTick) noexcept { this->bShouldTick = bInShouldTick; }

#if JAFG_DO_CHECKS
    //# For checks only. Do not use elsewhere.
    FORCEINLINE constexpr bool _Lives() const noexcept { return this->bLives; }
#endif /* JAFG_DO_CHECKS */

protected:

    FORCEINLINE void SetEverTickConstructorOnlyFlag() noexcept { check(this->bLives == false) this->bCanEverTick = true; }
    FORCEINLINE void CancelEverTickConstructorOnlyFlag() noexcept { check(this->bLives == false) this->bCanEverTick = false; }

private:

    //# Make this virtual private to not confuse it with #ShouldTick.
    FORCEINLINE virtual bool ShouldTickableObjectTick() const override final { return this->ShouldTick(); }

    //# Whether this actor lives.
    bool bLives:1{};

    //#
    //# Whether this Actor should ever be able to tick or not.
    //# This bool flag can only be set in the constructor of the actor - new objects of this class
    //# will not be registered in the context tickable registry.
    //#
    bool bCanEverTick:1{};

    //#
    //# Whether this actor should tick now or not. This flag does nothing if #bCanEverTick is false.
    //#
    bool bShouldTick:1{true};

    //# The scene component that marks the translation of this actor. Everything is relative to this.
    ASceneComponent* RootComponent{};
    //# All owned components in no particular order.
    TArray<TJxxUnique<AActorComponent>> Components;
};

} /* ~Namespace Jafg */
