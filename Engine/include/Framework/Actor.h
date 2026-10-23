// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/WorldObject.h"
#include "Framework/TickableObject.h"
#include "Framework/FixedTickableObject.h"
#include "Framework/ActorComponent.h"
#include "Framework/SceneComponent.h"
#include "Actor.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API AActor : public AWorldObject, private LTickableObject, private LFixedTickableObject
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS(AActor)

public:

    virtual void BeginLife() override;
    virtual void WorldTick(f32 Dt)
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
    virtual void PhysicsTick(f32 Dt, bool bFollowup)
    {
        check(this->bLives && this->_IsGarbage() == false)
        for (auto& Comp: this->Components)
        {
            if (Comp->bPhysicsTick)
            {
                Comp->ParentPhysicsTick(Dt, bFollowup);
            }
        }
    }
    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

    //# Emplace a new actor component to this actor that is not a scene component.
    template<typename TActorComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<AActorComponent, TActorComponent> && !std::is_base_of_v<ASceneComponent, TActorComponent>)
    FORCEINLINE TActorComponent& EmplaceDefaultComponent(TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), ASceneComponent&>);
        auto& Result{*StaticCastChecked<TActorComponent>(&*this->Components.emplace_back(SpawnObject(TWorldStaticInit<TActorComponent>{.Outer=this->GetWorld()})))};
        Result.Origin = EActorComponentOrigin::Ctor;
        check(!Result.template IsA<ASceneComponent>())
        if constexpr (requires { Callback(Result); })
        {
            Callback(Result);
        }
        check(!this->bLives)
        return Result;
    }
    template<typename TActorComponent = AActorComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<AActorComponent, TActorComponent> && !std::is_base_of_v<ASceneComponent, TActorComponent>)
    FORCEINLINE TActorComponent& EmplaceDefaultComponent(TSubclassOf<TActorComponent> Class, TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), ASceneComponent&>);
        check(Class.GetClass())
        auto& Result{*StaticCastChecked<TActorComponent>(&*this->Components.emplace_back(SpawnObject(CastTo<AActorComponent>{}, {this->GetWorld(), *Class.GetClass()})))};
        Result.Origin = EActorComponentOrigin::Ctor;
        check(!Result.template IsA<ASceneComponent>())
        if constexpr (requires { Callback(Result); })
        {
            Callback(Result);
        }
        check(!this->bLives)
        return Result;
    }

    //# Emplace a new scene component to this actor under the given parent. The parent obviously has to be already attached to this actor.
    template<typename TSceneComponent = ASceneComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceDefaultSceneComponentTo(ASceneComponent& Parent, TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), TSceneComponent&>);
        check(this->_check_IsComponentAttachedTransitive(Parent))
        auto& Result{*StaticCastChecked<TSceneComponent>(&*Parent.Children.emplace_back(SpawnObject(TWorldStaticInit<TSceneComponent>{.Outer=this->GetWorld()})))};
        Result.Parent = &Parent;
        Result.Origin = EActorComponentOrigin::Ctor;
        if constexpr (requires { Callback(Result); })
        {
            Callback(Result);
        }
        check(!this->bLives)
        return Result;
    }
    template<typename TSceneComponent = ASceneComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceDefaultSceneComponentTo(ASceneComponent& Parent, TSubclassOf<ASceneComponent> Class, TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), TSceneComponent&>);
        check(Class.GetClass())
        check(this->_check_IsComponentAttachedTransitive(Parent))
        auto& Result{*StaticCastChecked<TSceneComponent>(&*Parent.Children.emplace_back(SpawnObject(CastTo<ASceneComponent>{}, {this->GetWorld(), *Class.GetClass()})))};
        Result.Parent = &Parent;
        Result.Origin = EActorComponentOrigin::Ctor;
        if constexpr (requires { Callback(Result); })
        {
            Callback(Result);
        }
        check(!this->bLives)
        return Result;
    }

    //# Emplace the root component for this actor. Once the root component has been set. It can no longer be removed until the actor is destroyed.
    template<typename TSceneComponent = ASceneComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceDefaultRootComponent(TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), TSceneComponent&>);
        auto& Result{*StaticCastChecked<TSceneComponent>(&*this->Components.emplace_back(SpawnObject(TWorldStaticInit<TSceneComponent>{.Outer=this->GetWorld()})))};
        Result.Origin = EActorComponentOrigin::Ctor;
        check(!this->RootComponent)
        this->RootComponent = &Result;
        if constexpr (requires { Callback(Result); })
        {
            (void)Callback(Result);
        }
        check(!this->bLives)
        return Result;
    }
    template<typename TSceneComponent = ASceneComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceDefaultRootComponent(TSubclassOf<ASceneComponent> Class, TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), TSceneComponent&>);
        check(Class.GetClass())
        auto& Result{*StaticCastChecked<TSceneComponent>(&*this->Components.emplace_back(SpawnObject(CastTo<AActorComponent>{}, {this->GetWorld(), *Class.GetClass()})))};
        Result.Origin = EActorComponentOrigin::Ctor;
        check(!this->RootComponent)
        this->RootComponent = &Result;
        if constexpr (requires { Callback(Result); })
        {
            Callback(Result);
        }
        check(!this->bLives)
        return Result;
    }

    //# Emplace a new actor component to this actor that is not a scene component.
    template<typename TActorComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<AActorComponent, TActorComponent> && !std::is_base_of_v<ASceneComponent, TActorComponent>)
    FORCEINLINE TActorComponent& EmplaceComponent(TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), TActorComponent&>);
        auto& Result{*StaticCastChecked<TActorComponent>(&*this->Components.emplace_back(SpawnObject(TWorldStaticInit<TActorComponent>{.Outer=this->GetWorld()})))};
        Result.Origin = EActorComponentOrigin::Runtime;
        check(!Result.template IsA<ASceneComponent>())
        if constexpr (requires { Callback(Result); })
        {
            Callback(Result);
        }
        if (this->bLives)
        {
            Result.OnAttach(*this);
        }
        return Result;
    }
    template<typename TActorComponent = AActorComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<AActorComponent, TActorComponent> && !std::is_base_of_v<ASceneComponent, TActorComponent>)
    FORCEINLINE TActorComponent& EmplaceComponent(TSubclassOf<TActorComponent> Class, TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), TActorComponent&>);
        check(Class.GetClass())
        auto& Result{*StaticCastChecked<TActorComponent>(&*this->Components.emplace_back(SpawnObject(CastTo<AActorComponent>{}, {this->GetWorld(), *Class.GetClass()})))};
        Result.Origin = EActorComponentOrigin::Runtime;
        check(!Result.template IsA<ASceneComponent>())
        if constexpr (requires { Callback(Result); })
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
    template<typename TSceneComponent = ASceneComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceSceneComponentTo(ASceneComponent& Parent, TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), TSceneComponent&>);
        check(this->_check_IsComponentAttachedTransitive(Parent))
        auto& Result{*StaticCastChecked<TSceneComponent>(&*Parent.Children.emplace_back(SpawnObject(TWorldStaticInit<TSceneComponent>{.Outer=this->GetWorld()})))};
        Result.Parent = &Parent;
        Result.Origin = EActorComponentOrigin::Runtime;
        if constexpr (requires { Callback(Result); })
        {
            Callback(Result);
        }
        if (this->bLives)
        {
            Result.OnAttach(*this);
        }
        return Result;
    }
    template<typename TSceneComponent = ASceneComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceSceneComponentTo(ASceneComponent& Parent, TSubclassOf<ASceneComponent> Class, TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), TSceneComponent&>);
        check(Class.GetClass())
        check(this->_check_IsComponentAttachedTransitive(Parent))
        auto& Result{*StaticCastChecked<TSceneComponent>(&*Parent.Children.emplace_back(SpawnObject(CastTo<ASceneComponent>{}, {this->GetWorld(), *Class.GetClass()})))};
        Result.Parent = &Parent;
        Result.Origin = EActorComponentOrigin::Runtime;
        if constexpr (requires { Callback(Result); })
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
    template<typename TSceneComponent = ASceneComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceRootComponent(TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), TSceneComponent&>);
        auto& Result{*StaticCastChecked<TSceneComponent>(&*this->Components.emplace_back(SpawnObject(TWorldStaticInit<TSceneComponent>{.Outer=this->GetWorld()})))};
        Result.Origin = EActorComponentOrigin::Runtime;
        check(!this->RootComponent)
        this->RootComponent = &Result;
        if constexpr (requires { Callback(Result); })
        {
            (void)Callback(Result);
        }
        if (this->bLives)
        {
            Result.OnAttach(*this);
        }
        return Result;
    }
    template<typename TSceneComponent = ASceneComponent, typename TCallback = std::monostate>
        requires(std::is_base_of_v<ASceneComponent, TSceneComponent>)
    FORCEINLINE TSceneComponent& EmplaceRootComponent(TSubclassOf<ASceneComponent> Class, TCallback&& Callback = {})
    {
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), TSceneComponent&>);
        check(Class.GetClass())
        auto& Result{*StaticCastChecked<TSceneComponent>(&*this->Components.emplace_back(SpawnObject(CastTo<AActorComponent>{}, {this->GetWorld(), *Class.GetClass()})))};
        Result.Origin = EActorComponentOrigin::Runtime;
        check(!this->RootComponent)
        this->RootComponent = &Result;
        if constexpr (requires { Callback(Result); })
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

    template<typename T = AActorComponent> requires std::is_base_of_v<AActorComponent, T>
    NODISCARD FORCEINLINE T* GetComponent(TSubclassOf<T> Class) const noexcept
    {
        for (auto const& Comp: this->Components)
        {
            if (Comp->IsA(Class))
            {
                return StaticCastChecked<T>(&*Comp);
            }
        }
        return {};
    }
    template<typename T> requires std::is_base_of_v<AActorComponent, T>
    NODISCARD FORCEINLINE T& GetComponentChecked(TSubclassOf<T> Class) const noexcept
    {
        auto* Result{this->GetComponent<T>(Class)};
        check(Result)
        return *Result;
    }
    template<typename T> requires std::is_base_of_v<AActorComponent, T>
    NODISCARD FORCEINLINE T& GetComponentAsserted(TSubclassOf<T> Class) const noexcept
    {
        auto* Result{this->GetComponent<T>(Class)};
        jassert(Result)
        return *Result;
    }

    //# Remove a component that was previously emplaced to this actor.
    FORCEINLINE void RemoveComponent(AActorComponent& Component) noexcept
    {
        auto It{algo::find(this->Components, &Component, algo::unique_raw)};
        check(It != this->Components.end())
        check(this->bLives && "Components cannot be removed during construction.")
        check(&Component != this->RootComponent && "The root component cannot be changed.")
        check(Component.Origin == EActorComponentOrigin::Runtime && "Only components that were emplaced at runtime can be removed.")
        this->Components.erase(It);
    }

    NODISCARD FORCEINLINE constexpr bool HasRootComponent() const noexcept { return !!this->RootComponent; }
    NODISCARD FORCEINLINE constexpr ASceneComponent& GetRootComponent() const noexcept { check(this->RootComponent) return *this->RootComponent; }
    template<typename T> requires std::is_base_of_v<ASceneComponent, T>
    NODISCARD FORCEINLINE constexpr T& GetRootComponent() const noexcept { return *StaticCastChecked<T>(this->RootComponent); }
    NODISCARD FORCEINLINE TArray<TJxxUnique<AActorComponent>> const& GetComponents() const noexcept { return this->Components; }

    NODISCARD FORCEINLINE constexpr bool CanEverTick() const noexcept { return this->bCanEverTick; }
    //# Ctor only.
    FORCEINLINE constexpr void SetCanEverTick(bool bCanEverTick) noexcept { check(this->bLives == false) this->bCanEverTick = bCanEverTick; }
    NODISCARD FORCEINLINE constexpr bool CanEverTickInDormantTimes() const noexcept { return this->bCanEverTickInDesistedTime; }
    //# Ctor only.
    FORCEINLINE constexpr void SetCanEverTickInDormantTimes(bool bCanEverTick) noexcept { check(this->bLives == false) this->bCanEverTickInDesistedTime = bCanEverTick; }
    NODISCARD FORCEINLINE bool ShouldTick() const noexcept { return this->bShouldTick; }
    FORCEINLINE void SetShouldTick(bool bInShouldTick) noexcept { this->bShouldTick = bInShouldTick; }

    NODISCARD FORCEINLINE constexpr bool CanEverPhysicsTick() const noexcept { return this->bCanEverPhysicsTick; }
    //# Ctor only.
    FORCEINLINE constexpr void SetCanEverPhysicsTick(bool bTick) noexcept { check(!this->bLives) this->bCanEverPhysicsTick = bTick; }
    NODISCARD FORCEINLINE bool ShouldPhysicsTick() const noexcept { return this->bShouldPhysicsTick; }
    FORCEINLINE void SetShouldPhysicsTick(bool bTick) noexcept { this->bShouldPhysicsTick = bTick; }

    //# Whether to allow that components of this actor render. An actor itself does not have rendering capabilities.
    NODISCARD FORCEINLINE constexpr bool AllowsRendering() const noexcept { return this->bAllowRender; }
    FORCEINLINE constexpr void SetAllowRendering(bool b) noexcept { this->bAllowRender = b; }

#if JAFG_WITH_EDITOR
    NODISCARD FORCEINLINE constexpr bool IsEditorHitTestable() const noexcept { return this->bEditorHitTestable; }
    FORCEINLINE constexpr void SetEditorHitTestable(bool b) noexcept { this->bEditorHitTestable = b; }
#endif /* JAFG_WITH_EDITOR */

    NODISCARD LWorldAabb3 GetTransformedActorAabb() const noexcept
    {
        LWorldAabb3 Result;
        if (this->HasRootComponent())
        {
            Result = this->GetRootComponent().GetTransitiveComponentAabb();
        }
        else
        {
            Result = maths::identity<LWorldAabb3>;
        }
        return Result;
    }

#if JAFG_DO_CHECKS
    //# For checks only. Do not use elsewhere.
    FORCEINLINE constexpr bool _Lives() const noexcept { return this->bLives; }
#endif /* JAFG_DO_CHECKS */

protected:

    virtual AWorldObject& CloneImpl(AWorldObject* Object) const noexcept override;

private:

    virtual void Tick(f32 Dt) final override;
    virtual void FixedTick(f32 Dt, bool bFollowup) final override;

#if JAFG_DO_CHECKS
    NODISCARD bool _check_IsComponentAttachedTransitive(AActorComponent const& Who) const noexcept;
#endif /* JAFG_DO_CHECKS */

    //# Make this virtual private to not confuse it with #ShouldTick.
    FORCEINLINE virtual bool ShouldTickableObjectTick() const override final { return this->ShouldTick(); }
    FORCEINLINE virtual bool ShouldFixedTickableObjectTick() const override final { return this->ShouldPhysicsTick(); }

    //# Whether this actor lives.
    bool bLives:1{};

    //#
    //# Whether this Actor should ever be able to tick or not.
    //# This bool flag can only be set in the constructor of the actor - new objects of this class
    //# will not be registered in the context tickable registry.
    //#
    bool bCanEverTick:1{};

    //#
    //# Whether this actor should ever be able to tick or not in a time desisted or dormant world.
    //# This bool flag can only be set in the constructor of the actor - new objects of this class
    //# will not be registered in the context tickable registry.
    //#
    bool bCanEverTickInDesistedTime:1{};

    //#
    //# Whether this actor should tick now or not. This flag does nothing if #bCanEverTick is false.
    //# Or in desisted worlds if #bCanEverTickInDesistedTime is false.
    //#
    bool bShouldTick:1{true};

    //#
    //# Whether this Actor should ever be able to tick before the fixed tick of the physics system.
    //# This is called every time the physics system ticks. This might be called multiple times per frame.
    //#
    //# Not that is flag does not prohibit the physics tick of subcomponents which register themselves
    //# directly to the physics system. This is only for manual callbacks. Rarely useful.
    //#
    //# It is prohibited to delete any Jxx class during fixed updates.
    //#
    //# This bool flag can only be set in the constructor of the actor - new objects of this class
    //# will not be registered in the context tickable registry.
    //#
    bool bCanEverPhysicsTick:1{};

    //#
    //# Whether this actor should physics tick now or not. This flag does nothing if #bCanEverTickFixed is false.
    //#
    bool bShouldPhysicsTick:1{true};

    //# Whether to allow that components render.
    bool bAllowRender:1{true};

#if JAFG_WITH_EDITOR
    //# Whether this actor is hit testable by the editor world viewer.
    bool bEditorHitTestable:1{true};
#endif /* JAFG_WITH_EDITOR */

    //# The scene component that marks the translation of this actor. Everything is relative to this.
    ASceneComponent* RootComponent{};
    //# All owned components in no particular order.
    TArray<TJxxUnique<AActorComponent>> Components;
};

} /* ~Namespace Jafg */
