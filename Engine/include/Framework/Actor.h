// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/WorldObject.h"
#include "Engine/TickableObject.h"
#include "Components/ActorComponent.h"
#include "Actor.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API AActor : public AWorldObject, private LTickableObject
{
    GENERATED_CLASS_BODY()

protected:

    explicit AActor(LWorldDynamicInit const& Init) noexcept : Super{Init}
        , DisplayName{Init.Class.GetFullyQualifiedName()}
    {
    }
    template<typename TCxxClass>
    explicit AActor(TWorldStaticInit<TCxxClass> const& Init) noexcept : Super{Init}
        , DisplayName{TCxxClass::StaticClass().GetFullyQualifiedName()}
    {
    }

public:

    virtual void BeginLife() override;
    virtual void Tick(f32 Dt) override { check(this->bLives && this->_IsGarbage() == false) }
    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

#if JAFG_WITH_LOCAL_LAYER
    //#
    //# DisplayName is for editor purposes only. Do not query for it, try to find objects with the name.
    //# A display name can change at any time and might be localized ect.
    //#
    LString DisplayName;
#endif /* JAFG_WITH_LOCAL_LAYER */

    template<typename TActorComponent> requires std::is_base_of_v<AActorComponent, TActorComponent>
    FORCEINLINE TActorComponent* EmplaceComponent(TFunction<void(TActorComponent& Comp)> const& Callback = {})
    {
        return this->EmplaceComponent<TActorComponent>(TActorComponent::StaticClass(), Callback);
    }
    template<typename TActorComponent = AActorComponent> requires std::is_base_of_v<AActorComponent, TActorComponent>
    FORCEINLINE TActorComponent* EmplaceComponent(LJxxClass const& Class, TFunction<void(TActorComponent& Comp)> const& Callback = {})
    {
        auto* Result{StaticCastChecked<TActorComponent>(&*this->Components.emplace_back(NewObject(CastTo<AActorComponent>{}, {this->GetOuter(), Class})))};
        if (Callback.IsValid())
        {
            Callback(*Result);
        }
        /* If called before #BeginLife the #AActorComponent::OnAttach will be called then there. */
        if (this->bLives)
        {
            Result->OnAttach(*this);
        }
        return Result;
    }

    template<typename T> requires std::is_base_of_v<AActorComponent, T>
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
    template<typename T> requires std::is_base_of_v<AActorComponent, T>
    FORCEINLINE T* GetComponentChecked() const noexcept
    {
        auto* Result{this->GetComponent<T>()};
        check(Result)
        return Result;
    }
    template<typename T> requires std::is_base_of_v<AActorComponent, T>
    FORCEINLINE T* GetComponentAsserted() const noexcept
    {
        auto* Result{this->GetComponent<T>()};
        jassert(Result)
        return Result;
    }

    FORCEINLINE auto const& GetComponents() const noexcept { return this->Components; }

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

    TArray<TJxxUnique<AActorComponent>> Components;
};

} /* ~Namespace Jafg */
