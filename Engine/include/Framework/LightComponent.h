// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SceneComponent.h"
#include "Rhi/Light.h"
#include "LightComponent.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class ENGINE_API ALightComponent : public ASceneComponent
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_WORLD_CONSTRUCTORS_BODY(ALightComponent)
    {
#if JAFG_WITH_EDITOR
        this->SetShouldRender(true);
        this->CreateDefaultComponents();
#endif /* JAFG_WITH_EDITOR */
    }

public:

    virtual void OnAttach(AActor& Owner) override;
    virtual void OnGarbage(EJxxRecordTearDownReason Reason) override;

    ENGINE_API bool IsRegistered() const noexcept;

    void SetLight(LLight const& Light);
    NODISCARD FORCEINLINE constexpr LLight const& GetLight() const noexcept { return this->Light; }

    ENGINE_API void SetType(ELight Type) noexcept
    {
        this->Light.Type = Type;
        this->OnTypeChanged();
    }
    NODISCARD FORCEINLINE constexpr ELight GetType() const noexcept { return this->Light.Type; }

    FORCEINLINE constexpr void SetCanEverAffectWorld(bool bEnabled) noexcept { check(!this->_check_HasExecutedOnAttach()) this->Light.bEnabled = bEnabled; }
    NODISCARD FORCEINLINE constexpr bool CanEverAffectWorld() const noexcept { return this->Light.bEnabled; }
    FORCEINLINE constexpr void SetEnabled(bool bEnabled) noexcept { this->Light.bEnabled = bEnabled; }
    NODISCARD FORCEINLINE constexpr bool IsEnabled() const noexcept { return this->Light.bEnabled; }

    FORCEINLINE constexpr void SetCastShadows(bool bCast) noexcept { this->Light.bCastShadows = bCast; }
    NODISCARD FORCEINLINE constexpr bool CastsShadows() const noexcept { return this->Light.bCastShadows; }

    FORCEINLINE constexpr void SetDirection(LVec3F Direction) noexcept { this->Light.Direction = Direction; }
    NODISCARD FORCEINLINE constexpr LVec3F const& GetDirection() const noexcept { return this->Light.Direction; }

    FORCEINLINE constexpr void SetTint(LLinearColor Tint) noexcept { this->Light.Tint = Tint; }
    NODISCARD FORCEINLINE constexpr LLinearColor const& GetTint() const noexcept { return this->Light.Tint; }

protected:

    virtual void OnTransformChanged(ETransformChangeFlags Change) override
    {
        this->UpdateLightTransform();
    }

#if JAFG_WITH_EDITOR
    void CreateDefaultComponents();
    void OnTypeChanged();
#endif /* JAFG_WITH_EDITOR */

    void UpdateLightTransform()
    {
        auto Transform{this->GetWorldTransformSlow()};
        this->Light.Translation = Transform.t;
        this->Light.Rotation = Transform.r;
    }

private:

    LLight Light;
};

} /* ~Namespace Jafg */
