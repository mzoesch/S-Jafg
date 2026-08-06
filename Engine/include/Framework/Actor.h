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
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), ASceneComponent&>);
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
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(Callback)>, std::monostate> || std::is_invocable_v<decltype(Callback), ASceneComponent&>);
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
    NODISCARD FORCEINLINE TArray<TJxxUnique<AActorComponent>> const& GetComponents() const noexcept { return this->Components; }

    NODISCARD FORCEINLINE bool CanEverTick() const noexcept { return this->bCanEverTick; }
    NODISCARD FORCEINLINE bool ShouldTick() const noexcept { return this->bShouldTick; }
    FORCEINLINE void SetShouldTick(bool bInShouldTick) noexcept { this->bShouldTick = bInShouldTick; }

#if JAFG_WITH_EDITOR
    NODISCARD FORCEINLINE constexpr bool IsEditorHitTestable() const noexcept { return this->bEditorHitTestable; }
    FORCEINLINE constexpr void SetEditorHitTestable(bool b) noexcept { this->bEditorHitTestable = b; }
#endif /* JAFG_WITH_EDITOR */

#if JAFG_DO_CHECKS
    //# For checks only. Do not use elsewhere.
    FORCEINLINE constexpr bool _Lives() const noexcept { return this->bLives; }
#endif /* JAFG_DO_CHECKS */

protected:

    FORCEINLINE void SetEverTickConstructorOnlyFlag() noexcept { check(this->bLives == false) this->bCanEverTick = true; }
    FORCEINLINE void CancelEverTickConstructorOnlyFlag() noexcept { check(this->bLives == false) this->bCanEverTick = false; }

    virtual AWorldObject& CloneImpl(AWorldObject* Object) const noexcept override;

private:

#if JAFG_DO_CHECKS
    NODISCARD bool _check_IsComponentAttachedTransitive(AActorComponent const& Who) const noexcept
    {
        bool bSceneComp{Who.IsA<ASceneComponent>()};
        for (auto& Comp: this->Components)
        {
            check(Comp.get())
            check(&Comp->GetOwningActor() == this)

            if (&*Comp == &Who)
            {
                return true;
            }

            if (bSceneComp)
            {
                if (auto* SceneComp{Comp->As<ASceneComponent>()})
                {
                    auto IsAttachedInChild{[this](this auto&& Self, ASceneComponent const& Comp, ASceneComponent const& Who) noexcept -> bool
                    {
                        check(&Comp.GetOwningActor() == this)
                        check(&Comp != &Who)
                        for (auto& Child: Comp.GetChildren())
                        {
                            check(Child.get())
                            check(&Child->GetOwningActor() == this)
                            if (&*Child == &Who)
                            {
                                return true;
                            }
                            if (Self(*Child, Who))
                            {
                                return true;
                            }
                        }

                        return false;
                    }};
                    if (IsAttachedInChild(*SceneComp, Who.AsStatic<ASceneComponent>()))
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }
#endif /* JAFG_DO_CHECKS */

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
