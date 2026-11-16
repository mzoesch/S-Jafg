// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SceneComponent.h"
#include "Rhi/Light.h"
#include "LightComponent.generated.h"

namespace Jafg
{

template<> ENGINE_API Detail::LNodeFactoryBase GetEditorNode<LLight>(TEditorNodeCreateInfo<LLight> const& Info) noexcept;

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
#if JAFG_WITH_EDITOR
    virtual void SelectedTick(WEditorWorldViewer& Viewer, f32 Dt) override;
#endif /* JAFG_WITH_EDITOR */
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

    FORCEINLINE constexpr void SetCanEverAffectWorld(bool bAffects) noexcept { check(!this->_check_HasExecutedOnAttach()) this->Light.bCanEverAffectWorld = bAffects; }
    NODISCARD FORCEINLINE constexpr bool CanEverAffectWorld() const noexcept { return this->Light.bCanEverAffectWorld; }
    FORCEINLINE constexpr void SetEnabled(bool bEnabled) noexcept { this->Light.bEnabled = bEnabled; }
    NODISCARD FORCEINLINE constexpr bool IsEnabled() const noexcept { return this->Light.bEnabled; }

    FORCEINLINE constexpr void SetCastShadows(bool bCast) noexcept { this->Light.bCastShadows = bCast; }
    NODISCARD FORCEINLINE constexpr bool CastsShadows() const noexcept { return this->Light.bCastShadows; }

    //#
    //# A light points along the forward axis of this component. So this rotates the component in a way that it
    //# points along the given world space direction (which does not have to be normalized) while keeping the light
    //# upright.
    //# @see LLight::GetDirection
    //#
    void SetDirection(LVec3F const& Direction) noexcept;
    NODISCARD FORCEINLINE LVec3F GetDirection() const noexcept { return this->Light.GetDirection(); }

    FORCEINLINE constexpr void SetTint(LLinearColor3 Tint) noexcept { this->Light.Tint = Tint; }
    NODISCARD FORCEINLINE constexpr LLinearColor3 const& GetTint() const noexcept { return this->Light.Tint; }

    //# In the unit of the type of this light, see #GetUnits.
    FORCEINLINE constexpr void SetIntensity(f32 Intensity) noexcept { check(Intensity >= 0.0f) this->Light.Intensity = Intensity; }
    NODISCARD FORCEINLINE constexpr f32 GetIntensity() const noexcept { return this->Light.Intensity; }
    NODISCARD FORCEINLINE constexpr ELightUnits GetUnits() const noexcept { return this->Light.GetUnits(); }

    FORCEINLINE constexpr void SetSpecularScale(f32 Scale) noexcept { check(Scale >= 0.0f) this->Light.SpecularScale = Scale; }
    NODISCARD FORCEINLINE constexpr f32 GetSpecularScale() const noexcept { return this->Light.SpecularScale; }

    //# Point, spot and rect lights only. In meters with zero being unlimited.
    FORCEINLINE constexpr void SetRange(f32 Range) noexcept { check(Range >= 0.0f) this->Light.Range = Range; }
    NODISCARD FORCEINLINE constexpr f32 GetRange() const noexcept { return this->Light.Range; }

    //# Point and spotlights only. In meters.
    FORCEINLINE constexpr void SetSourceRadius(f32 Radius) noexcept { check(Radius >= 0.0f) this->Light.SourceRadius = Radius; }
    NODISCARD FORCEINLINE constexpr f32 GetSourceRadius() const noexcept { return this->Light.SourceRadius; }

    //# Directional lights only. The angular diameter in radians.
    FORCEINLINE constexpr void SetSourceAngle(f32 Radians) noexcept { check(Radians >= 0.0f) this->Light.SourceAngle = Radians; }
    NODISCARD FORCEINLINE constexpr f32 GetSourceAngle() const noexcept { return this->Light.SourceAngle; }

    //# Spot lights only. Both angles are measured in radians from the axis of the cone.
    FORCEINLINE constexpr void SetConeAngles(f32 InnerRadians, f32 OuterRadians) noexcept
    {
        check(0.0f <= InnerRadians && InnerRadians <= OuterRadians)
        this->Light.InnerConeAngle = InnerRadians;
        this->Light.OuterConeAngle = OuterRadians;
    }
    FORCEINLINE constexpr void SetInnerConeAngle(f32 Radians) noexcept { check(Radians >= 0.0f) this->Light.InnerConeAngle = Radians; }
    NODISCARD FORCEINLINE constexpr f32 GetInnerConeAngle() const noexcept { return this->Light.InnerConeAngle; }
    FORCEINLINE constexpr void SetOuterConeAngle(f32 Radians) noexcept { check(Radians >= 0.0f) this->Light.OuterConeAngle = Radians; }
    NODISCARD FORCEINLINE constexpr f32 GetOuterConeAngle() const noexcept { return this->Light.OuterConeAngle; }

    //# Rect lights only. In meters with x being the width and y the height.
    FORCEINLINE constexpr void SetSize(LVec2F Size) noexcept
    {
        check(Size.x >= 0.0f && Size.y >= 0.0f)
        this->Light.Width = Size.x;
        this->Light.Height = Size.y;
    }
    NODISCARD FORCEINLINE constexpr LVec2F GetSize() const noexcept { return {this->Light.Width, this->Light.Height}; }
    FORCEINLINE constexpr void SetWidth(f32 Width) noexcept { check(Width >= 0.0f) this->Light.Width = Width; }
    NODISCARD FORCEINLINE constexpr f32 GetWidth() const noexcept { return this->Light.Width; }
    FORCEINLINE constexpr void SetHeight(f32 Height) noexcept { check(Height >= 0.0f) this->Light.Height = Height; }
    NODISCARD FORCEINLINE constexpr f32 GetHeight() const noexcept { return this->Light.Height; }

    //# Private public method. Do not use.
    void _detail_OnTypeChanged() { this->OnTypeChanged(); }

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

    CLASS_FIELD(EditorEditable)
    LLight Light;
};

} /* ~Namespace Jafg */
