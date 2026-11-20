// Copyright mzoesch. All rights reserved.

#include "Framework/RigidComponent.h"
#include "Framework/Actor.h"
#include "PhysicsForeignCore.h"
#include "Rhi/StaticMesh.h"
#include "Framework/StaticMeshComponent.h"
#include "Nodes/Node.h"
#include "Widgets/EditorFactory.h"
#include "Widgets/Input_Boolean.h"
#include "Widgets/Input_Vector1.h"
#include "Widgets/Input_Vector3.h"

static_assert(std::to_underlying(Jafg::EBodyType::Rigid) == std::to_underlying(JPH::EBodyType::RigidBody));
static_assert(std::to_underlying(Jafg::EBodyType::Soft) == std::to_underlying(JPH::EBodyType::SoftBody));
static_assert(std::to_underlying(Jafg::EBodyType::Count) == JPH::cBodyTypeCount);
static_assert(std::is_same_v<std::underlying_type_t<Jafg::EBodyType>, std::underlying_type_t<JPH::EBodyType>>);

static_assert(Jafg::LPhysicsBodyId::cInvalidBodyID == JPH::BodyID::cInvalidBodyID);
static_assert(Jafg::LPhysicsBodyId::cBroadPhaseBit == JPH::BodyID::cBroadPhaseBit);
static_assert(Jafg::LPhysicsBodyId::cMaxBodyIndex == JPH::BodyID::cMaxBodyIndex);
static_assert(Jafg::LPhysicsBodyId::cMaxSequenceNumber == JPH::BodyID::cMaxSequenceNumber);
static_assert(Jafg::LPhysicsBodyId::cSequenceNumberShift == JPH::BodyID::cSequenceNumberShift);

static_assert(std::is_same_v<decltype(Jafg::LPhysicsBodyId::cInvalidBodyID), decltype(JPH::BodyID::cInvalidBodyID)>);
static_assert(std::is_same_v<decltype(Jafg::LPhysicsBodyId::cBroadPhaseBit), decltype(JPH::BodyID::cBroadPhaseBit)>);
static_assert(std::is_same_v<decltype(Jafg::LPhysicsBodyId::cMaxBodyIndex), decltype(JPH::BodyID::cMaxBodyIndex)>);
static_assert(std::is_same_v<decltype(Jafg::LPhysicsBodyId::cMaxSequenceNumber), decltype(JPH::BodyID::cMaxSequenceNumber)>);
static_assert(std::is_same_v<decltype(Jafg::LPhysicsBodyId::cSequenceNumberShift), decltype(JPH::BodyID::cSequenceNumberShift)>);

static_assert(std::to_underlying(Jafg::ECanSleep::Cannot) == std::to_underlying(JPH::ECanSleep::CannotSleep));
static_assert(std::to_underlying(Jafg::ECanSleep::Can) == std::to_underlying(JPH::ECanSleep::CanSleep));
static_assert(std::is_same_v<std::underlying_type_t<Jafg::ECanSleep>, std::underlying_type_t<JPH::ECanSleep>>);

static_assert(std::is_same_v<decltype(Jafg::LPhysicsBody::cInactiveIndex), decltype(JPH::MotionProperties::cInactiveIndex)>);
static_assert(Jafg::LPhysicsBody::cInactiveIndex == JPH::MotionProperties::cInactiveIndex);

Jafg::LPhysicsBodyId Jafg::LPhysicsBody::GetID() const
{
    return LPhysicsBodyId{this->GetBody().GetID().GetIndexAndSequenceNumber()};
}

Jafg::EBodyType Jafg::LPhysicsBody::GetBodyType() const
{
    return static_cast<EBodyType>(this->GetBody().GetBodyType());
}

bool Jafg::LPhysicsBody::IsActive() const
{
    return this->GetBody().IsActive();
}

bool Jafg::LPhysicsBody::IsStatic() const
{
    return this->GetBody().IsStatic();
}

bool Jafg::LPhysicsBody::IsKinematic() const
{
    return this->GetBody().IsKinematic();
}

bool Jafg::LPhysicsBody::IsDynamic() const
{
    return this->GetBody().IsDynamic();
}

bool Jafg::LPhysicsBody::CanBeKinematicOrDynamic() const
{
    return this->GetBody().CanBeKinematicOrDynamic();
}

void Jafg::LPhysicsBody::SetIsSensor(bool bSensor)
{
    this->GetBody().SetIsSensor(bSensor);
}

bool Jafg::LPhysicsBody::IsSensor() const
{
    return this->GetBody().IsSensor();
}

void Jafg::LPhysicsBody::SetCollideKinematicVsNonDynamic(bool bCollide)
{
    this->GetBody().SetCollideKinematicVsNonDynamic(bCollide);
}

bool Jafg::LPhysicsBody::GetCollideKinematicVsNonDynamic() const
{
    return this->GetBody().GetCollideKinematicVsNonDynamic();
}

void Jafg::LPhysicsBody::SetUseManifoldReduction(bool bUseReduction)
{
    this->GetBody().SetUseManifoldReduction(bUseReduction);
}

bool Jafg::LPhysicsBody::GetUseManifoldReduction() const
{
    return this->GetBody().GetUseManifoldReduction();
}

bool Jafg::LPhysicsBody::GetUseManifoldReductionWithBody(LPhysicsBody const& Body2) const
{
    return this->GetBody().GetUseManifoldReductionWithBody(Body2.GetBody());
}

void Jafg::LPhysicsBody::SetApplyGyroscopicForce(bool bApply)
{
    this->GetBody().SetApplyGyroscopicForce(bApply);
}

bool Jafg::LPhysicsBody::GetApplyGyroscopicForce() const
{
    return this->GetBody().GetApplyGyroscopicForce();
}

void Jafg::LPhysicsBody::SetEnhancedInternalEdgeRemoval(bool bApply)
{
    this->GetBody().SetEnhancedInternalEdgeRemoval(bApply);
}

bool Jafg::LPhysicsBody::GetEnhancedInternalEdgeRemoval() const
{
    return this->GetBody().GetEnhancedInternalEdgeRemoval();
}

bool Jafg::LPhysicsBody::GetEnhancedInternalEdgeRemovalWithBody(LPhysicsBody const& Body2) const
{
    return this->GetBody().GetEnhancedInternalEdgeRemovalWithBody(Body2.GetBody());
}

Jafg::EPhysicsMotion Jafg::LPhysicsBody::GetMotionType() const
{
    return static_cast<EPhysicsMotion>(this->GetBody().GetMotionType());
}

void Jafg::LPhysicsBody::SetMotionType(EPhysicsMotion Motion)
{
    this->GetBody().SetMotionType(static_cast<JPH::EMotionType>(Motion));
}

Jafg::EPhysicsBroadPhase Jafg::LPhysicsBody::GetBroadPhaseLayer() const
{
    return static_cast<EPhysicsBroadPhase>(this->GetBody().GetBroadPhaseLayer().GetValue());
}

Jafg::EPhysicsLayer Jafg::LPhysicsBody::GetObjectLayer() const
{
    return static_cast<EPhysicsLayer>(this->GetBody().GetObjectLayer());
}

bool Jafg::LPhysicsBody::GetAllowSleeping() const
{
    auto& Body{this->GetBody()};
    return Body.CanBeKinematicOrDynamic() && Body.GetAllowSleeping();
}

void Jafg::LPhysicsBody::SetAllowSleeping(bool bAllow)
{
    check(this->GetBody().CanBeKinematicOrDynamic())
    this->GetBody().SetAllowSleeping(bAllow);
}

void Jafg::LPhysicsBody::ResetSleepTimer()
{
    this->GetBody().ResetSleepTimer();
}

f32 Jafg::LPhysicsBody::GetFriction() const
{
    return this->GetBody().GetFriction();
}

void Jafg::LPhysicsBody::SetFriction(f32 Friction)
{
    this->GetBody().SetFriction(Friction);
}

f32 Jafg::LPhysicsBody::GetRestitution() const
{
    return this->GetBody().GetRestitution();
}

void Jafg::LPhysicsBody::SetRestitution(f32 Restitution)
{
    this->GetBody().SetRestitution(Restitution);
}

LVec3F Jafg::LPhysicsBody::GetLinearVelocity() const
{
    JPH::Vec3 Result{this->GetBody().GetLinearVelocity()};
    return LVec3F{Result.GetX(), Result.GetY(), Result.GetZ()};
}

void Jafg::LPhysicsBody::SetLinearVelocity(LVec3F const& Velocity)
{
    this->GetBody().SetLinearVelocity(JPH::Vec3{Velocity.x, Velocity.y, Velocity.z});
}

void Jafg::LPhysicsBody::SetLinearVelocityClamped(LVec3F const& Velocity)
{
    this->GetBody().SetLinearVelocityClamped(JPH::Vec3{Velocity.x, Velocity.y, Velocity.z});
}

LVec3F Jafg::LPhysicsBody::GetAngularVelocity() const
{
    JPH::Vec3 Result{this->GetBody().GetAngularVelocity()};
    return LVec3F{Result.GetX(), Result.GetY(), Result.GetZ()};
}

void Jafg::LPhysicsBody::SetAngularVelocity(LVec3F const& Velocity)
{
    this->GetBody().SetAngularVelocity(JPH::Vec3{Velocity.x, Velocity.y, Velocity.z});
}

void Jafg::LPhysicsBody::SetAngularVelocityClamped(LVec3F const& Velocity)
{
    this->GetBody().SetAngularVelocityClamped(JPH::Vec3{Velocity.x, Velocity.y, Velocity.z});
}

LVec3F Jafg::LPhysicsBody::GetPointVelocityCOM(LVec3F const& PointRelativeToCOM) const
{
    JPH::Vec3 Result{this->GetBody().GetPointVelocityCOM(JPH::Vec3{PointRelativeToCOM.x, PointRelativeToCOM.y, PointRelativeToCOM.z})};
    return LVec3F{Result.GetX(), Result.GetY(), Result.GetZ()};
}

LWorldVec3 Jafg::LPhysicsBody::GetPointVelocity(LWorldVec3 const& Point) const
{
    JPH::Vec3 Result{this->GetBody().GetPointVelocity(JPH::RVec3{Point.x, Point.y, Point.z})};
    return LWorldVec3{Result.GetX(), Result.GetY(), Result.GetZ()};
}

void Jafg::LPhysicsBody::AddForce(LVec3F const& Force)
{
    this->GetBody().AddForce(JPH::Vec3{Force.x, Force.y, Force.z});
}

void Jafg::LPhysicsBody::AddForce(LVec3F const& Force, LWorldVec3 const& Position)
{
    this->GetBody().AddForce(JPH::Vec3{Force.x, Force.y, Force.z}, JPH::RVec3{Position.x, Position.y, Position.z});
}

LWorldVec3 Jafg::LPhysicsBody::GetAccumulatedForce() const
{
    JPH::Vec3 Result{this->GetBody().GetAccumulatedForce()};
    return LWorldVec3{Result.GetX(), Result.GetY(), Result.GetZ()};
}

void Jafg::LPhysicsBody::ResetForce()
{
    this->GetBody().ResetForce();
}

void Jafg::LPhysicsBody::AddTorque(LWorldVec3 const& Torque)
{
    this->GetBody().AddTorque(JPH::Vec3{Torque.x, Torque.y, Torque.z});
}

LWorldVec3 Jafg::LPhysicsBody::GetAccumulatedTorque() const
{
    JPH::Vec3 Result{this->GetBody().GetAccumulatedTorque()};
    return LWorldVec3{Result.GetX(), Result.GetY(), Result.GetZ()};
}

void Jafg::LPhysicsBody::ResetTorque()
{
    this->GetBody().ResetTorque();
}

void Jafg::LPhysicsBody::ResetMotion()
{
    this->GetBody().ResetMotion();
}

LMat4F Jafg::LPhysicsBody::GetInverseInertia() const
{
    static_assert(sizeof(f32) == 4);
    static_assert(sizeof(LMat4F) == sizeof(JPH::Mat44));
    //# TODO: This has to be evaluated if it is correct. Especially if we use something like AVX.
    return std::bit_cast<LMat4F>(this->GetBody().GetInverseInertia());
}

void Jafg::LPhysicsBody::AddImpulse(LWorldVec3 const& Impulse)
{
    this->GetBody().AddImpulse(JPH::Vec3{Impulse.x, Impulse.y, Impulse.z});
}

void Jafg::LPhysicsBody::AddImpulse(LWorldVec3 const& Impulse, LWorldVec3 const& Position)
{
    this->GetBody().AddImpulse(JPH::Vec3{Impulse.x, Impulse.y, Impulse.z}, JPH::RVec3{Position.x, Position.y, Position.z});
}

void Jafg::LPhysicsBody::AddAngularImpulse(LWorldVec3 const& AngularImpulse)
{
    this->GetBody().AddAngularImpulse(JPH::Vec3{AngularImpulse.x, AngularImpulse.y, AngularImpulse.z});
}

void Jafg::LPhysicsBody::MoveKinematic(LWorldVec3 const& TargetPosition, LQuatF TargetRotation, float DeltaTime)
{
    this->GetBody().MoveKinematic(
          JPH::RVec3{TargetPosition.x, TargetPosition.y, TargetPosition.z}
        , JPH::Quat{TargetRotation.x, TargetRotation.y, TargetRotation.z, TargetRotation.w}
        , DeltaTime
        );
}

void Jafg::LPhysicsBody::GetSubmergedVolume(
      LWorldVec3 const& SurfacePosition
    , LWorldVec3 const& SurfaceNormal
    , f32& OutTotalVolume
    , f32& OutSubmergedVolume
    , LWorldVec3& OutRelativeCenterOfBuoyancy
    ) const
{
    JPH::Vec3 RelativeCenterOfBuoyancy;
    this->GetBody().GetSubmergedVolume(
          JPH::RVec3{SurfacePosition.x, SurfacePosition.y, SurfacePosition.z}
        , JPH::Vec3{SurfaceNormal.x, SurfaceNormal.y, SurfaceNormal.z}
        , OutTotalVolume
        , OutSubmergedVolume
        , RelativeCenterOfBuoyancy
        );
    OutRelativeCenterOfBuoyancy = LWorldVec3{RelativeCenterOfBuoyancy.GetX(), RelativeCenterOfBuoyancy.GetY(), RelativeCenterOfBuoyancy.GetZ()};
}

bool Jafg::LPhysicsBody::ApplyBuoyancyImpulse(
      LWorldVec3 const& SurfacePosition
    , LWorldVec3 const& SurfaceNormal
    , f32 Buoyancy
    , f32 LinearDrag
    , f32 AngularDrag
    , LWorldVec3 const& FluidVelocity
    , LWorldVec3 const& Gravity
    , f32 DeltaTime
    )
{
    return this->GetBody().ApplyBuoyancyImpulse(
          JPH::Vec3{SurfacePosition.x, SurfacePosition.y, SurfacePosition.z}
        , JPH::Vec3{SurfaceNormal.x, SurfaceNormal.y, SurfaceNormal.z}
        , Buoyancy
        , LinearDrag
        , AngularDrag
        , JPH::Vec3{FluidVelocity.x, FluidVelocity.y, FluidVelocity.z}
        , JPH::Vec3{Gravity.x, Gravity.y, Gravity.z}
        , DeltaTime
        );
}

bool Jafg::LPhysicsBody::ApplyBuoyancyImpulse(
      f32 TotalVolume
    , f32 SubmergedVolume
    , LWorldVec3 const& RelativeCenterOfBuoyancy
    , f32 Buoyancy
    , f32 LinearDrag
    , f32 AngularDrag
    , LWorldVec3 const& FluidVelocity
    , LWorldVec3 const& Gravity
    , f32 DeltaTime
    )
{
    return this->GetBody().ApplyBuoyancyImpulse(
          TotalVolume
        , SubmergedVolume
        , JPH::Vec3{RelativeCenterOfBuoyancy.x, RelativeCenterOfBuoyancy.y, RelativeCenterOfBuoyancy.z}
        , Buoyancy
        , LinearDrag
        , AngularDrag
        , JPH::Vec3{FluidVelocity.x, FluidVelocity.y, FluidVelocity.z}
        , JPH::Vec3{Gravity.x, Gravity.y, Gravity.z}
        , DeltaTime
        );
}

bool Jafg::LPhysicsBody::IsInBroadPhase() const
{
    return this->GetBody().IsInBroadPhase();
}

bool Jafg::LPhysicsBody::IsCollisionCacheInvalid() const
{
    return this->GetBody().IsCollisionCacheInvalid();
}

LWorldVec3 Jafg::LPhysicsBody::GetPosition() const
{
    JPH::Vec3 Position{this->GetBody().GetPosition()};
    return LWorldVec3{Position.GetX(), Position.GetY(), Position.GetZ()};
}

LWorldQuat Jafg::LPhysicsBody::GetRotation() const
{
    JPH::Quat Rotation{this->GetBody().GetRotation()};
    return LWorldQuat{Rotation.GetX(), Rotation.GetY(), Rotation.GetZ(), Rotation.GetW()};
}

LWorldVec3 Jafg::LPhysicsBody::GetCenterOfMassPosition() const
{
    JPH::Vec3 Position{this->GetBody().GetPosition()};
    return LWorldVec3{Position.GetX(), Position.GetY(), Position.GetZ()};
}

#if JAFG_DO_CHECKS
void Jafg::LPhysicsBody::ValidateCachedBounds() const
{
#ifdef JPH_ENABLE_ASSERTS
    this->GetBody().ValidateCachedBounds();
#endif /* JPH_ENABLE_ASSERTS */
}

void Jafg::LPhysicsBody::ValidateMotion() const
{
#ifdef JPH_ENABLE_ASSERTS
    this->GetBody().ValidateMotion();
#endif /* JPH_ENABLE_ASSERTS */
}
#endif /* JAFG_DO_CHECKS */

JPH::MotionProperties const& Jafg::LPhysicsBody::GetMotionProperties() const
{
    return *this->GetBody().GetMotionProperties();
}

JPH::MotionProperties& Jafg::LPhysicsBody::GetMotionProperties()
{
    return *this->GetBody().GetMotionProperties();
}

JPH::MotionProperties const* Jafg::LPhysicsBody::GetMotionPropertiesUnchecked() const
{
    return this->GetBody().GetMotionPropertiesUnchecked();
}

JPH::MotionProperties* Jafg::LPhysicsBody::GetMotionPropertiesUnchecked()
{
    return this->GetBody().GetMotionPropertiesUnchecked();
}

u64 Jafg::LPhysicsBody::GetUserData() const
{
    static_assert(sizeof(u64) == sizeof(decltype(this->GetBody().GetUserData())));
    return this->GetBody().GetUserData();
}

void Jafg::LPhysicsBody::SetUserData(u64 UserData)
{
    this->GetBody().SetUserData(UserData);
}

namespace
{

//#
//# @return Whether the rigid object was removed from the simulation.
//#
bool PrepareRecreation(JPH::BodyInterface& Interface, Jafg::ARigidComponent& Comp)
{
    bool bResult{};

    if (Comp.GetRigidObject().IsValid())
    {
        if (Comp.IsAddedToSimulation())
        {
            Interface.RemoveBody(Comp.GetRigidObject()->GetID());
        }
        Comp._DestroyRigidObjectImpl();
        bResult = true;
    }
    check(!Comp.GetRigidObject().IsValid())

    return bResult;
}

//# @see Jafg::Detail::CharacterBodyUserDataTag
NODISCARD u64 MakeBodyUserData(Jafg::ARigidComponent& Comp) noexcept
{
    u64 const Result{reinterpret_cast<u64>(&Comp)};
    check((Result & Jafg::Detail::CharacterBodyUserDataTag) == 0)
    return Result;
}

} /* ~Namespace <Anonymous> */

//# COM declarations from Windows headers define INTERFACE...
#ifdef INTERFACE
    #undef INTERFACE
#endif /* INTERFACE */
#define INTERFACE this->GetParentPhysicsSystem()->GetBodyInterfaceNoLock()

namespace Jafg
{

namespace
{

//#
//# The body is not a plain data struct. Everything goes through getters and setters on the native body, and the
//# simulation changes values (velocities, sleeping) behind our back. So every row reads the body directly and
//# registers a refresher that pulls the current value into its widget each inspector tick.
//#
struct LPhysicsBodyEditorState final
{
    ARigidComponent* Comp{};
    //# The field itself, not a copy. The component may recreate the native body while the inspector is open.
    LPhysicsBody* Body{};
    TArray<TFunction2<void()>> Refreshers;
};
typedef std::shared_ptr<LPhysicsBodyEditorState> LPhysicsBodyEditorStatePtr;

struct LPhysicsBodyBoolRow final: LEditorRowStateExt
{
    WInput_Boolean* Input{};
};
struct LPhysicsBodyScalarRow final: LEditorRowStateExt
{
    WInput_Vector1* Input{};
};
struct LPhysicsBodyVectorRow final: LEditorRowStateExt
{
    WInput_Vector3* Input{};
};

NODISCARD EJxxFieldFlags MakeRowFlags(EJxxFieldFlags Flags, bool bEditable) noexcept
{
    if (bEditable && JxxFieldBits::IsEditorEditable(Flags))
    {
        return Flags;
    }
    return EJxxFieldFlags{EJxxFieldBits::EditorVisible};
}

//# Read-only row that displays a string.
template<typename TGetter>
NODISCARD Detail::LNodeFactoryBase MakeTextRow(TEditorNodeCreateInfo<LPhysicsBody> const& Info
    , LPhysicsBodyEditorStatePtr const& State
    , LString Name
    , LStringView Tooltip
    , TGetter Getter
    ) noexcept
{
    EJxxFieldFlags const Flags{MakeRowFlags(Info.Flags, false)};
    auto SharedState{std::make_shared<LEditorRowState>()};
    auto Text{std::make_shared<WText*>()};

    auto Evaluate{[Body=State->Body, Getter]() -> LString
    {
        return Body->IsValid() ? LString{Getter(std::as_const(*Body))} : LString{"-"};
    }};
    State->Refreshers.emplace_back([Text, Evaluate]
    {
        check(*Text)
        if (LString Value{Evaluate()}; Value != (*Text)->GetContent())
        {
            (*Text)->SetContent(std::move(Value));
        }
    });

    return NewNode(Info.Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
        .Tooltip(LString{Tooltip})
    [
        EditorComponentLabel(Flags, Info.Viewport, std::move(Name), SharedState)
        + EditorComponentContentWrapper(Flags, Info.Viewport, SharedState)
        [
            NewNode(Info.Viewport).Class<WText>().SaveTo(Text.get())
                .Anchor(EAnchor::HFill)
                .TextTint(Colors::Gray)
                .Content(Evaluate())
        ]
        + EditorResetButton(Flags, Info.Viewport, false, [](WButton& Self) {}, LEditorRowState::Lambda<WButton>(SharedState))
    ];
}

template<typename TGetter, typename TSetter>
NODISCARD Detail::LNodeFactoryBase MakeBoolRow(TEditorNodeCreateInfo<LPhysicsBody> const& Info
    , LPhysicsBodyEditorStatePtr const& State
    , LString Name
    , LStringView Tooltip
    , bool bEditable
    , std::optional<bool> Default
    , TGetter Getter
    , TSetter Setter
    ) noexcept
{
    EJxxFieldFlags const Flags{MakeRowFlags(Info.Flags, bEditable)};
    bool const bResettable{JxxFieldBits::IsEditorEditable(Flags) && Default.has_value()};
    auto Row{std::make_shared<LPhysicsBodyBoolRow>()};
    LPhysicsBody* Body{State->Body};
    bool const bInitial{Body->IsValid() && Getter(std::as_const(*Body))};

    State->Refreshers.emplace_back([Row, Body, Getter, Default, bResettable]
    {
        check(Row->Input && Row->ResetButton)
        if (!Body->IsValid())
        {
            return;
        }
        if (bool const bValue{Getter(std::as_const(*Body))}; bValue != Row->Input->GetValue())
        {
            Row->Input->SetValue(bValue, false);
            Row->ResetButton->SetEnabled(bResettable && bValue != *Default);
        }
    });

    return NewNode(Info.Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
        .Tooltip(LString{Tooltip})
    [
        EditorComponentLabel(Flags, Info.Viewport, std::move(Name), Row)
        + EditorComponentContentWrapper(Flags, Info.Viewport, Row)
        [
            NewNode(Info.Viewport).Class<WInput_Boolean>().SaveTo(&Row->Input)
                .Anchor(EAnchor::HFill)
                .Value(bInitial)
                .Enabled(JxxFieldBits::IsEditorEditable(Flags))
                .OnValueChanged([Row, Body, Setter, Default, bResettable](WInput_Boolean& Self)
                {
                    if (Body->IsValid())
                    {
                        Setter(*Body, Self.GetValue());
                    }
                    check(Row->ResetButton)
                    Row->ResetButton->SetEnabled(bResettable && Self.GetValue() != *Default);
                })
        ]
        + EditorResetButton(Flags, Info.Viewport, bResettable && Body->IsValid() && bInitial != *Default, [Row, Default](WButton& Self)
        {
            check(Row->Input)
            check(Default)
            Row->Input->SetValue(*Default);
        }, LEditorRowState::Lambda<WButton>(Row)).SaveTo(&Row->ResetButton)
    ];
}

template<typename TGetter, typename TSetter>
NODISCARD Detail::LNodeFactoryBase MakeScalarRow(TEditorNodeCreateInfo<LPhysicsBody> const& Info
    , LPhysicsBodyEditorStatePtr const& State
    , LString Name
    , LStringView Tooltip
    , std::optional<f32> Default
    , TGetter Getter
    , TSetter Setter
    ) noexcept
{
    EJxxFieldFlags const Flags{MakeRowFlags(Info.Flags, true)};
    bool const bResettable{JxxFieldBits::IsEditorEditable(Flags) && Default.has_value()};
    auto Row{std::make_shared<LPhysicsBodyScalarRow>()};
    LPhysicsBody* Body{State->Body};
    f32 const Initial{Body->IsValid() ? Getter(std::as_const(*Body)) : 0.0f};

    State->Refreshers.emplace_back([Row, Body, Getter, Default, bResettable]
    {
        check(Row->Input && Row->ResetButton)
        if (!Body->IsValid())
        {
            return;
        }
        if (f32 const Value{Getter(std::as_const(*Body))}; Value != static_cast<f32>(Row->Input->Get<f64>()))
        {
            Row->Input->Set(static_cast<f64>(Value), false);
            Row->ResetButton->SetEnabled(bResettable && Value != *Default);
        }
    });

    return NewNode(Info.Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
        .Tooltip(LString{Tooltip})
    [
        EditorComponentLabel(Flags, Info.Viewport, std::move(Name), Row)
        + EditorComponentContentWrapper(Flags, Info.Viewport, Row)
        [
            NewNode(Info.Viewport).Class<WInput_Vector1>(static_cast<f64>(Initial)).SaveTo(&Row->Input)
                .Anchor(EAnchor::HFill)
                .Enabled(JxxFieldBits::IsEditorEditable(Flags))
                .OnVectorChanged([Row, Body, Setter, Default, bResettable](WInput_Vector1& Self)
                {
                    f32 const Value{static_cast<f32>(Self.Get<f64>())};
                    if (Body->IsValid())
                    {
                        Setter(*Body, Value);
                    }
                    check(Row->ResetButton)
                    Row->ResetButton->SetEnabled(bResettable && Value != *Default);
                })
        ]
        + EditorResetButton(Flags, Info.Viewport, bResettable && Body->IsValid() && Initial != *Default, [Row, Default](WButton& Self)
        {
            check(Row->Input)
            check(Default)
            Row->Input->Set(static_cast<f64>(*Default));
        }, LEditorRowState::Lambda<WButton>(Row)).SaveTo(&Row->ResetButton)
    ];
}

//# Editable row for a vector that the simulation changes every tick. Resetting sets the vector to zero.
template<typename TGetter, typename TSetter>
NODISCARD Detail::LNodeFactoryBase MakeVectorRow(TEditorNodeCreateInfo<LPhysicsBody> const& Info
    , LPhysicsBodyEditorStatePtr const& State
    , LString Name
    , LStringView Tooltip
    , bool bEditable
    , TGetter Getter
    , TSetter Setter
    ) noexcept
{
    EJxxFieldFlags const Flags{MakeRowFlags(Info.Flags, bEditable)};
    bool const bResettable{JxxFieldBits::IsEditorEditable(Flags)};
    auto Row{std::make_shared<LPhysicsBodyVectorRow>()};
    LPhysicsBody* Body{State->Body};
    LVec3D const Initial{Body->IsValid() ? LVec3D{Getter(std::as_const(*Body))} : maths::zero_vector<LVec3D>};

    State->Refreshers.emplace_back([Row, Body, Getter, bResettable]
    {
        check(Row->Input && Row->ResetButton)
        if (!Body->IsValid())
        {
            return;
        }
        if (LVec3D const Value{Getter(std::as_const(*Body))}; Value != Row->Input->Get<f64>())
        {
            Row->Input->Set(Value, false);
            Row->ResetButton->SetEnabled(bResettable && Value != maths::zero_vector<LVec3D>);
        }
    });

    return NewNode(Info.Viewport).Class<WHParent>()
        .Anchor(EAnchor::HFill)
        .Space(1_spt)
        .Tooltip(LString{Tooltip})
    [
        EditorComponentLabel(Flags, Info.Viewport, std::move(Name), Row)
        + EditorComponentContentWrapper(Flags, Info.Viewport, Row)
        [
            NewNode(Info.Viewport).Class<WInput_Vector3>(Initial).SaveTo(&Row->Input)
                .Anchor(EAnchor::HFill)
                .Enabled(JxxFieldBits::IsEditorEditable(Flags))
                .OnVectorChanged([Row, Body, Setter, bResettable](WInput_Vector3& Self)
                {
                    LVec3D const Value{Self.Get<f64>()};
                    if (Body->IsValid())
                    {
                        Setter(*Body, LWorldVec3{Value});
                    }
                    check(Row->ResetButton)
                    Row->ResetButton->SetEnabled(bResettable && Value != maths::zero_vector<LVec3D>);
                })
        ]
        + EditorResetButton(Flags, Info.Viewport, bResettable && Initial != maths::zero_vector<LVec3D>, [Row](WButton& Self)
        {
            check(Row->Input)
            Row->Input->Set(maths::zero_vector<LVec3D>);
        }, LEditorRowState::Lambda<WButton>(Row)).SaveTo(&Row->ResetButton)
    ];
}

} /* ~Namespace <Anonymous> */

} /* ~Namespace Jafg */

template<>
Jafg::Detail::LNodeFactoryBase Jafg::GetEditorNode<Jafg::LPhysicsBody>(TEditorNodeCreateInfo<LPhysicsBody> const& Info) noexcept
{
    check(Info.Owner)
    ARigidComponent& Comp{Info.Owner->AsStatic<ARigidComponent>()};

    auto State{std::make_shared<LPhysicsBodyEditorState>(LPhysicsBodyEditorState{.Comp = &Comp, .Body = &Info.Field})};

    //# Velocities cannot be set on static bodies. Bodies in the simulation go through the body interface, so that
    //# they are woken up if they are sleeping.
    bool const bMovable{Info.Field.IsValid() && !Info.Field.IsStatic()};

    /* Defaults are the ones of JPH::BodyCreationSettings. */
    auto Factory{
          MakeTextRow(Info, State, "Body", "Index and sequence number of the native body.", [](LPhysicsBody const& Body)
            {
                LPhysicsBodyId const Id{Body.GetID()};
                return algo::sprintf("{} (seq {})", Id.GetIndex(), Id.GetSequenceNumber());
            })
        + MakeTextRow(Info, State, "Body Type", "Whether this is a rigid or a soft body.", [](LPhysicsBody const& Body) -> LString
            {
                return Body.IsRigidBody() ? "Rigid" : "Soft";
            })
        + MakeTextRow(Info, State, "Motion Type", "Motion type of the native body. Change the component's motion to recreate the body with another one.", [](LPhysicsBody const& Body)
            {
                return serde::to_string(Body.GetMotionType());
            })
        + MakeTextRow(Info, State, "Object Layer", "Determines which other objects this body collides with.", [](LPhysicsBody const& Body)
            {
                return serde::to_string(Body.GetObjectLayer());
            })
        + MakeTextRow(Info, State, "Broad Phase Layer", "Determines in which broad phase sub-tree this body is placed.", [](LPhysicsBody const& Body)
            {
                return serde::to_string(Body.GetBroadPhaseLayer());
            })
        + MakeBoolRow(Info, State, "In Simulation", "Whether this body has been added to the physics system.", false, std::nullopt, [Comp=&Comp](LPhysicsBody const& Body)
            {
                return Comp->IsAddedToSimulation();
            }, [](LPhysicsBody& Body, bool bValue) {})
        + MakeBoolRow(Info, State, "Active", "Whether this body is currently simulating (true) or sleeping (false).", false, std::nullopt, [](LPhysicsBody const& Body)
            {
                return Body.IsActive();
            }, [](LPhysicsBody& Body, bool bValue) {})
        + MakeVectorRow(Info, State, "Linear Velocity", "World space linear velocity of the center of mass (m/s).", bMovable, [](LPhysicsBody const& Body)
            {
                return Body.GetLinearVelocity();
            }, [Comp=&Comp](LPhysicsBody& Body, LWorldVec3 const& Velocity)
            {
                if (Body.IsStatic())
                {
                    return;
                }
                if (Comp->IsAddedToSimulation())
                {
                    Comp->SetLinearVelocity(Velocity);
                }
                else
                {
                    Body.SetLinearVelocity(Velocity);
                }
            })
        + MakeVectorRow(Info, State, "Angular Velocity", "World space angular velocity of the center of mass (rad/s).", bMovable, [](LPhysicsBody const& Body)
            {
                return Body.GetAngularVelocity();
            }, [Comp=&Comp](LPhysicsBody& Body, LWorldVec3 const& Velocity)
            {
                if (Body.IsStatic())
                {
                    return;
                }
                if (Comp->IsAddedToSimulation())
                {
                    Comp->GetParentPhysicsSystem()->GetBodyInterfaceNoLock().SetAngularVelocity(Body->GetID()
                        , JPH::Vec3{static_cast<float>(Velocity.x), static_cast<float>(Velocity.y), static_cast<float>(Velocity.z)});
                }
                else
                {
                    Body.SetAngularVelocity(Velocity);
                }
            })
        + MakeScalarRow(Info, State, "Friction", "Usually between 0 and 1. 0 is no friction, 1 is a friction force equal to the force that presses the two bodies together.", 0.2f, [](LPhysicsBody const& Body)
            {
                return Body.GetFriction();
            }, [](LPhysicsBody& Body, f32 Value)
            {
                Body.SetFriction(Value);
            })
        + MakeScalarRow(Info, State, "Restitution", "Usually between 0 and 1. 0 is a completely inelastic, 1 a completely elastic collision response.", 0.0f, [](LPhysicsBody const& Body)
            {
                return Body.GetRestitution();
            }, [](LPhysicsBody& Body, f32 Value)
            {
                Body.SetRestitution(Value);
            })
        + MakeBoolRow(Info, State, "Allow Sleeping", "Whether this body can go to sleep. Disabling it does not wake up a sleeping body.", true, true, [](LPhysicsBody const& Body)
            {
                return Body.GetAllowSleeping();
            }, [](LPhysicsBody& Body, bool bValue)
            {
                Body.SetAllowSleeping(bValue);
            })
        + MakeBoolRow(Info, State, "Sensor", "A sensor receives collision callbacks but causes no collision response.", true, false, [](LPhysicsBody const& Body)
            {
                return Body.IsSensor();
            }, [](LPhysicsBody& Body, bool bValue)
            {
                Body.SetIsSensor(bValue);
            })
        + MakeBoolRow(Info, State, "Collide Kinematic vs Non-Dynamic", "Whether kinematic bodies generate contacts against other kinematic or static bodies.", true, false, [](LPhysicsBody const& Body)
            {
                return Body.GetCollideKinematicVsNonDynamic();
            }, [](LPhysicsBody& Body, bool bValue)
            {
                Body.SetCollideKinematicVsNonDynamic(bValue);
            })
        + MakeBoolRow(Info, State, "Manifold Reduction", "Whether contacts with similar normals from different sub shapes are combined.", true, true, [](LPhysicsBody const& Body)
            {
                return Body.GetUseManifoldReduction();
            }, [](LPhysicsBody& Body, bool bValue)
            {
                Body.SetUseManifoldReduction(bValue);
            })
        + MakeBoolRow(Info, State, "Gyroscopic Force", "Whether the gyroscopic force is applied to this body (Dzhanibekov effect).", true, false, [](LPhysicsBody const& Body)
            {
                return Body.GetApplyGyroscopicForce();
            }, [](LPhysicsBody& Body, bool bValue)
            {
                Body.SetApplyGyroscopicForce(bValue);
            })
        + MakeBoolRow(Info, State, "Enhanced Edge Removal", "Extra effort to remove ghost contacts with internal edges of a mesh. More expensive.", true, false, [](LPhysicsBody const& Body)
            {
                return Body.GetEnhancedInternalEdgeRemoval();
            }, [](LPhysicsBody& Body, bool bValue)
            {
                Body.SetEnhancedInternalEdgeRemoval(bValue);
            })
        };

    if (Info.UpdateValue)
    {
        check(!*Info.UpdateValue)
        *Info.UpdateValue = [State]
        {
            for (auto& Refresh: State->Refreshers)
            {
                check(!!Refresh)
                Refresh();
            }
        };
    }

    return Factory;
}

#if JAFG_DO_CHECKS
void Jafg::ARigidComponent::OnAttach(AActor& InOwner)
{
    Super::OnAttach(InOwner);
    check(&this->GetOwningActor().GetRootComponent() == this && "Currently this is a limitation that we want to maybe resolve later.")
}
#endif /* JAFG_DO_CHECKS */

void Jafg::ARigidComponent::OnGarbage(EJxxRecordTearDownReason Reason)
{
    Super::OnGarbage(Reason);

    if (this->RigidObject.IsValid())
    {
        auto& Interface{INTERFACE};
        if (Interface.IsAdded(this->RigidObject->GetID()))
        {
            Interface.RemoveBody(this->RigidObject->GetID());
        }
        Interface.DestroyBody(this->RigidObject->GetID());
        this->RigidObject = nullptr;
        this->RemoveFromWorldRigidComponents();
    }
}

void Jafg::ARigidComponent::SetPhysicsLayer(EPhysicsLayer Layer)
{
    check(!this->RigidObject.IsValid() && "Not implemented yet.")
    this->Layer = Layer;
}

void Jafg::ARigidComponent::SetPhysicsMotion(EPhysicsMotion Motion)
{
    check(!this->RigidObject.IsValid() && "Not implemented yet.")
    this->Motion = Motion;
}

void Jafg::ARigidComponent::MakeSphere(f32 Radius)
{
    check(Tasks::IsOnMasterThread())

    auto& Interface{INTERFACE};
    bool bWasRemoved{::PrepareRecreation(Interface, *this)};

    LWorldTrans WorldTransform{this->GetWorldTransformSlow()};
    check(WorldTransform.s.x == WorldTransform.s.y && WorldTransform.s.x == WorldTransform.s.z)
    JPH::BodyCreationSettings CreateSettings{
        new JPH::SphereShape{Radius * WorldTransform.s.x},
        JPH::RVec3{WorldTransform.t.x, WorldTransform.t.y, WorldTransform.t.z},
        {WorldTransform.r.x, WorldTransform.r.y, WorldTransform.r.z, WorldTransform.r.w},
        JPH::EMotionType{std::to_underlying(this->Motion)},
        std::to_underlying(this->Layer),
        };
    CreateSettings.mUserData = ::MakeBodyUserData(*this);
    this->RigidObject = LPhysicsBody::FromNative(Interface.CreateBody(CreateSettings));
    check(this->RigidObject.IsValid())

    this->UpdateComponentAabb();
    if (!bWasRemoved)
    {
        this->AddToWorldRigidComponents();
    }
}

void Jafg::ARigidComponent::MakeBox(LWorldVec3 HalfExtent, f32 ConvexRadius /* = Physx::DefaultConvexRadius */)
{
    check(Tasks::IsOnMasterThread())

    auto& Interface{INTERFACE};
    bool bWasRemoved{::PrepareRecreation(Interface, *this)};

    LWorldTrans WorldTransform{this->GetWorldTransformSlow()};
    check(WorldTransform.s.x == WorldTransform.s.y && WorldTransform.s.x == WorldTransform.s.z)
    JPH::BodyCreationSettings CreateSettings{
        new JPH::BoxShape{{HalfExtent.x * WorldTransform.s.x, HalfExtent.y * WorldTransform.s.y, HalfExtent.z * WorldTransform.s.z}, ConvexRadius},
        JPH::RVec3{WorldTransform.t.x, WorldTransform.t.y, WorldTransform.t.z},
        {WorldTransform.r.x, WorldTransform.r.y, WorldTransform.r.z, WorldTransform.r.w},
        JPH::EMotionType{std::to_underlying(this->Motion)},
        std::to_underlying(this->Layer),
        };
    CreateSettings.mUserData = ::MakeBodyUserData(*this);
    this->RigidObject = LPhysicsBody::FromNative(Interface.CreateBody(CreateSettings));
    check(this->RigidObject.IsValid())

    this->UpdateComponentAabb();
    if (!bWasRemoved)
    {
        this->AddToWorldRigidComponents();
    }
}

void Jafg::ARigidComponent::MakeConvex(LStaticMesh& Mesh, f32 ConvexRadius /* = Physx::DefaultConvexRadius */)
{
    check(Tasks::IsOnMasterThread())

    if (!Mesh.HasConvex())
    {
        LOG_WARNING(LogPhysics, "[{}]: Mesh has no convex hull. Consider loading the mesh with [EStaticMeshStateBits::Convex]."
            , Mesh.GetPath()
            )
        Mesh.ReloadConvex();
        check(Mesh.HasConvex())
    }

    LWorldVec3 const WorldScale{this->GetWorldScaleSlow()};
    check(WorldScale.x == WorldScale.y && WorldScale.x == WorldScale.z)

    TArray<LVec3F> const& Hull{Mesh.GetConvexHull()};
    TArray<LVec3F> Points;
    Points.reserve(Hull.size());
    for (LVec3F const& Point: Hull)
    {
        Points.emplace_back(LVec3F{Point * LVec3F{WorldScale}});
    }

    this->MakeConvexFromPoints(Points, ConvexRadius);
}

void Jafg::ARigidComponent::MakeConvex(ERigidComponentConvexWalker Walker, f32 ConvexRadius /* = Physx::DefaultConvexRadius */)
{
    check(Tasks::IsOnMasterThread())

    TArray<AStaticMeshComponent*> Meshes;

    //# Only static mesh components are walked into. @return Whether the walk is done.
    auto Walk{[&](this auto const& Self, ASceneComponent const& Parent) -> bool
    {
        for (auto const& Child: Parent.GetChildren())
        {
            auto* Comp{DynamicCast<AStaticMeshComponent>(&*Child)};
            if (!Comp)
            {
                continue;
            }

            Meshes.push_back(Comp);

            if (Walker == ERigidComponentConvexWalker::First && Comp->IsMeshValid())
            {
                return true;
            }
            if (Walker != ERigidComponentConvexWalker::Direct && Self(*Comp))
            {
                return true;
            }
        }
        return false;
    }};
    Walk(*this);

    if (Meshes.empty())
    {
        LOG_WARNING(LogPhysics, "Found no static mesh component while walking [{}]. Keeping the current rigid object."
            , serde::to_string(Walker)
            )
        return;
    }

    this->MakeConvex(Meshes, ConvexRadius);
}

void Jafg::ARigidComponent::MakeConvex(std::span<AStaticMeshComponent* const> Meshes, f32 ConvexRadius /* = Physx::DefaultConvexRadius */)
{
    check(Tasks::IsOnMasterThread())

    LWorldTrans const WorldTransform{this->GetWorldTransformSlow()};
    check(WorldTransform.s.x == WorldTransform.s.y && WorldTransform.s.x == WorldTransform.s.z)
    LWorldQuat const InverseRotation{maths::inverse(WorldTransform.r)};

    TArray<LVec3F> Points;
    for (AStaticMeshComponent* Comp: Meshes)
    {
        check(Comp)
        if (!Comp->IsMeshValid())
        {
            LOG_WARNING(LogPhysics, "Static mesh component has no mesh.")
            continue;
        }

        LStaticMesh& Mesh{Comp->GetMutableMesh()};
        if (!Mesh.HasConvex())
        {
            LOG_WARNING(LogPhysics, "[{}]: Mesh has no convex hull. Consider loading the mesh with [EStaticMeshStateBits::Convex]."
                , Mesh.GetSource()
                )
            Mesh.ReloadConvex();
            check(Mesh.HasConvex())
        }

        /* Mesh space -> world space -> unrotated and untranslated space of this component (but still scaled). */
        LWorldTrans const MeshTransform{Comp->GetWorldTransformSlow()};
        TArray<LVec3F> const& Hull{Mesh.GetConvexHull()};
        Points.reserve(Points.size() + Hull.size());
        for (LVec3F const& Point: Hull)
        {
            LWorldVec3 const World{MeshTransform.r * (MeshTransform.s * LWorldVec3{Point}) + MeshTransform.t};
            Points.emplace_back(LVec3F{InverseRotation * (World - WorldTransform.t)});
        }
    }

    if (Points.empty())
    {
        LOG_WARNING(LogPhysics, "Found no static mesh with a convex hull among [{}] components. Keeping the current rigid object."
            , Meshes.size()
            )
        return;
    }

    this->MakeConvexFromPoints(Points, ConvexRadius);
}

void Jafg::ARigidComponent::MakeConvexFromPoints(TArray<LVec3F> const& Hull, f32 ConvexRadius)
{
    check(Tasks::IsOnMasterThread())

    LWorldTrans const WorldTransform{this->GetWorldTransformSlow()};

    JPH::Array<JPH::Vec3> Points;
    Points.reserve(Hull.size());
    for (LVec3F const& Point: Hull)
    {
        Points.emplace_back(Point.x, Point.y, Point.z);
    }

    JPH::ConvexHullShapeSettings const HullSettings{Points, ConvexRadius};
    JPH::ShapeSettings::ShapeResult const HullResult{HullSettings.Create()};
    if (HullResult.HasError())
    {
        LOG_FATAL(LogPhysics, "Failed to create convex hull from [{}] points: {}."
            , Points.size(), HullResult.GetError().c_str()
            )
    }

    auto& Interface{INTERFACE};
    bool bWasRemoved{::PrepareRecreation(Interface, *this)};

    JPH::BodyCreationSettings CreateSettings{
        HullResult.Get(),
        JPH::RVec3{WorldTransform.t.x, WorldTransform.t.y, WorldTransform.t.z},
        {WorldTransform.r.x, WorldTransform.r.y, WorldTransform.r.z, WorldTransform.r.w},
        JPH::EMotionType{std::to_underlying(this->Motion)},
        std::to_underlying(this->Layer),
        };
    CreateSettings.mUserData = ::MakeBodyUserData(*this);
    this->RigidObject = LPhysicsBody::FromNative(Interface.CreateBody(CreateSettings));
    check(this->RigidObject.IsValid())

    this->UpdateComponentAabb();
    if (!bWasRemoved)
    {
        this->AddToWorldRigidComponents();
    }
}

bool Jafg::ARigidComponent::IsAddedToSimulation() const noexcept
{
    return this->RigidObject.IsValid() && INTERFACE.IsAdded(this->RigidObject->GetID());
}

void Jafg::ARigidComponent::AddToSimulation(ESimulationAddingBehavior Behavior)
{
    check(!this->IsAddedToSimulation())
    check(!this->HasAttachParent() && "Simulated bodies cannot follow an attach parent.")
    INTERFACE.AddBody(this->RigidObject->GetID(), JPH::EActivation{std::to_underlying(Behavior)});
}

void Jafg::ARigidComponent::RemoveFromSimulation()
{
    check(this->IsAddedToSimulation())
    INTERFACE.RemoveBody(this->RigidObject->GetID());
}

void Jafg::ARigidComponent::SetLinearVelocity(LWorldVec3 const& Velocity)
{
    check(this->IsAddedToSimulation())
    INTERFACE.SetLinearVelocity(this->RigidObject->GetID(), JPH::Vec3{Velocity.x, Velocity.y, Velocity.z});
}

std::optional<LWorldReal> Jafg::ARigidComponent::TraceNonPhysical(LWorldMagRay3 const& Ray) const
{
    auto const HasStaticMesh{[](this auto const& Self, ASceneComponent const& Parent) -> bool
    {
        for (auto const& Child: Parent.GetChildren())
        {
            if (auto const* Mesh{DynamicCast<AStaticMeshComponent>(&*Child)}; Mesh && Mesh->IsMeshValid())
            {
                return true;
            }
            if (Self(*Child))
            {
                return true;
            }
        }
        return false;
    }};
    if (HasStaticMesh(*this) || !this->RigidObject.IsValid())
    {
        return {};
    }

    /* Cheap rejection first. */
    if (!Super::TraceNonPhysical(Ray))
    {
        return {};
    }

    /*
     * The body was created at the world translation and rotation of this component with the scale baked into the
     * shape (@see #MakeConvexFromPoints). Not from the body itself, it is only moved while being simulated.
     */
    LWorldTrans const World{this->GetWorldTransformSlow()};
    LWorldQuat const InverseRotation{maths::inverse(World.r)};
    LWorldVec3 const Origin{InverseRotation * (Ray.origin - World.t)};
    LWorldVec3 const Direction{InverseRotation * (Ray.direction * Ray.magnitude)};

    JPH::Shape const* Shape{this->RigidObject->GetShape()};
    check(Shape)
    /* Shapes are relative to their center of mass. */
    JPH::RayCast const Cast{
        JPH::Vec3{static_cast<f32>(Origin.x), static_cast<f32>(Origin.y), static_cast<f32>(Origin.z)} - Shape->GetCenterOfMass(),
        JPH::Vec3{static_cast<f32>(Direction.x), static_cast<f32>(Direction.y), static_cast<f32>(Direction.z)},
        };
    if (JPH::RayCastResult Hit; Shape->CastRay(Cast, JPH::SubShapeIDCreator{}, Hit))
    {
        return static_cast<LWorldReal>(Hit.mFraction) * Ray.magnitude;
    }
    return {};
}

void Jafg::ARigidComponent::OnTransformChanged(ETransformChangeFlags Change)
{
    Super::OnTransformChanged(Change);

    if (this->IsOwningActorValid())
    {
        check(&this->GetOwningActor().GetRootComponent() == this)

        if (!(Change & ETransformChangeBits::PhysicsPoll) && this->RigidObject.IsValid() && this->IsAddedToSimulation())
        {
            auto& Interface{INTERFACE};
            if ((Change & ETransformChangeBits::Translation) && (Change & ETransformChangeBits::Rotator))
            {
                Interface.SetPositionAndRotation(this->RigidObject->GetID()
                    , JPH::RVec3{this->GetLocalTranslation().x, this->GetLocalTranslation().y, this->GetLocalTranslation().z}
                    , JPH::Quat{this->GetLocalRotator().x, this->GetLocalRotator().y, this->GetLocalRotator().z, this->GetLocalRotator().w}
                    , JPH::EActivation::DontActivate
                    );
            }
            else if (Change & ETransformChangeBits::Translation)
            {
                Interface.SetPosition(this->RigidObject->GetID()
                    , JPH::RVec3{this->GetLocalTranslation().x, this->GetLocalTranslation().y, this->GetLocalTranslation().z}
                    , JPH::EActivation::DontActivate
                    );
            }
            else if (Change & ETransformChangeBits::Rotator)
            {
                Interface.SetRotation(this->RigidObject->GetID()
                    , JPH::Quat{this->GetLocalRotator().x, this->GetLocalRotator().y, this->GetLocalRotator().z, this->GetLocalRotator().w}
                    , JPH::EActivation::DontActivate
                    );
            }

            if (Change & ETransformChangeBits::Scale)
            {
                checkNoEntry()
                // we should cache the current shape (either outselves or with the jph::sahep) then apply the scale. Maybe only in editor builds?
                // how much does it need to be extra stroage to save?
                // this->GetParentPhysicsSystem()->AddConstraint()
                // this->RigidObject->GetBodyCreationSettings()
                // this->UpdateComponentAabb();
            }
        }
    }
}

void Jafg::ARigidComponent::UpdateComponentAabb() noexcept
{
    if (this->RigidObject.IsValid())
    {
        auto* Shape{this->RigidObject->GetShape()};
        check(Shape)
        auto Box{Shape->GetLocalBounds()};
        this->Aabb = LWorldAabb3{
            .min = LWorldVec3{
                this->GetLocalScale().x == 0.0f ? 0.0f : Box.mMin.GetX() / this->GetLocalScale().x,
                this->GetLocalScale().y == 0.0f ? 0.0f : Box.mMin.GetY() / this->GetLocalScale().y,
                this->GetLocalScale().z == 0.0f ? 0.0f : Box.mMin.GetZ() / this->GetLocalScale().z,
                },
            .max = LWorldVec3{
                this->GetLocalScale().x == 0.0f ? 0.0f : Box.mMax.GetX() / this->GetLocalScale().x,
                this->GetLocalScale().y == 0.0f ? 0.0f : Box.mMax.GetY() / this->GetLocalScale().y,
                this->GetLocalScale().z == 0.0f ? 0.0f : Box.mMax.GetZ() / this->GetLocalScale().z,
                },
            };
    }
    else
    {
        this->Aabb = maths::identity<LWorldAabb3>;
    }
}

void Jafg::ARigidComponent::AddToWorldRigidComponents() noexcept
{
    check(this->RigidObject.IsValid())
    check(!algo::contains(this->GetWorld().RigidComponents, this))
    this->GetWorld().RigidComponents.emplace_back(this);
}

void Jafg::ARigidComponent::RemoveFromWorldRigidComponents() noexcept
{
    check(!this->RigidObject.IsValid())
    algo::erase_exactly_once_checked(&this->GetWorld().RigidComponents, this);
}

void Jafg::ARigidComponent::DestroyRigidObject() noexcept
{
    this->_DestroyRigidObjectImpl();
    this->RemoveFromWorldRigidComponents();
}

void Jafg::ARigidComponent::_DestroyRigidObjectImpl() noexcept
{
    check(this->RigidObject.IsValid())
    INTERFACE.DestroyBody(this->RigidObject->GetID());
    this->RigidObject = nullptr;
}

#undef INTERFACE
