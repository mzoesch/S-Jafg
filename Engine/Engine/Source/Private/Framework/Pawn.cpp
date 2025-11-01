// Copyright mzoesch. All rights reserved.

#include "Framework/Pawn.h"
#include "Debug/DebugTraceCube.h"
#include "Debug/DebugTraceLine.h"
#include "MyWorld/Chunk/Chunk.h"
#include "User/Input/InputActionValue.h"
#include "Framework/PersonaController.h"
#include "Engine/Engine.h"
#include "System/VoxelSubsystem.h"

Jafg::APawn::APawn(LCxxObjectInitializer const& CxxObjectInitializer)
    : Super(CxxObjectInitializer)
{
    this->SetEverTickConstructorOnlyFlag();
    return;
}

void Jafg::APawn::Tick(const f32 DeltaTime)
{
    Super::Tick(DeltaTime);

    algo::orphan(&this->CurrentGenericTraceResults);
    const LVector TraceStart = this->GetTranslation();
    const LVector TraceEnd   = this->GetTranslation() + this->GetRotator().ToVector() * 5.0f;
    this->GetWorld()->LineTraceByChannel
    (
        this->CurrentGenericTraceResults, TraceStart, TraceEnd,
        ECollisionChannel::Static, LCollisionQueryParams({.bSingleHit = true})
    );

    // Quick user feedback. Just temp.
    if (algo::is_valid_index(this->CurrentGenericTraceResults, 0) && this->CurrentGenericTraceResults[0].Actor->IsA<AChunk>())
    {
        const LVoxelKey VKey = LVoxelKey::FromWorldSpace(this->CurrentGenericTraceResults[0].GlobalWorldLocation);
        this->GetWorld()->AddTemporalObject(LDebugTraceCube
        (
            LTemporalWorldObject::DrawOnce,
            StaticCastChecked<AChunk>(this->CurrentGenericTraceResults[0].Actor)
                ->GetChunkKey().ToWorldSpace() + LVector(VKey.X, VKey.Y, VKey.Z) + LVector(-0.001f),
            LVector::One() + LVector(0.002f),
            LDebugTraceCubeVisualParams{Lal::LColor{0.1f}, 5}
        ));
    }

    return;
}

void Jafg::APawn::EndLife()
{
    Super::EndLife();

    if (this->IsPossessed())
    {
        this->OwningController->PossessPawn(nullptr, false);
    }

    return;
}

bool Jafg::APawn::IsPossessedLocally() const
{
    return this->OwningController && this->OwningController->IsSurfaceValid();
}

Jafg::LLocalEgo* Jafg::APawn::GetLocalEgoIfPossessed() const
{
    if (this->OwningController)
    {
        return &this->OwningController->GetLocalEgo();
    }

    return nullptr;
}

void Jafg::APawn::SetOwningController(APersonaController* InNew)
{
    this->OwningController = InNew;

#if WITH_LOCAL_LAYER
    if (InNew)
    {
        this->Eye.SetOwningPawn(this);
    }
#endif /* WITH_LOCAL_LAYER */

    return;
}

void Jafg::APawn::OnOngoingMovementInput(LInputActionValue& InValue)
{
    LVector TranslationDelta = LVector::Zero();
    TranslationDelta += this->RelativeFront * (InValue.Get<LVector3>().X * this->MovementSpeed);
    TranslationDelta += this->RelativeRight * (InValue.Get<LVector3>().Y * this->MovementSpeed);
    TranslationDelta += LVector::UpVector   * (InValue.Get<LVector3>().Z * this->MovementSpeed);

    this->AddTranslation(TranslationDelta);

    return;
}

void Jafg::APawn::OnOngoingRotationInput(LInputActionValue& InValue)
{
    this->AddRotator(LRotator(
        InValue.Get<LVector2>().X * this->MouseSensitivity,
        InValue.Get<LVector2>().Y * this->MouseSensitivity,
        0.0f
    ));

    this->GetMutableRotator().ConstrainAxis(Lal::ERotatorAxis::Pitch, 89.9f);
    this->GetMutableRotator().NormalizeRotation();
    check( this->GetRotator().Pitch >= -89.9f && this->GetRotator().Pitch <= 89.9f )
    check( this->GetRotator().Yaw >= -180.0f && this->GetRotator().Yaw <= 180.0f )

    this->UpdateRelativeVectors();

    return;
}

void Jafg::APawn::OnOngoingVelocityChange(LInputActionValue& InValue)
{
    this->MovementSpeed += InValue.Get<float>() * 3.0f;

    if (this->MovementSpeed < 0)
    {
        this->MovementSpeed = 0;
    }
    if (this->MovementSpeed > 200)
    {
        this->MovementSpeed = 200;
    }

    return;
}

void Jafg::APawn::OnOngoingPrimaryInput(LInputActionValue& InValue)
{
    check( this->GetWorld() )

    for (const LHitResult& Hit : this->CurrentGenericTraceResults)
    {
        if (AChunk* HitChunk = Hit.Actor->As<AChunk>(); HitChunk)
        {
            const LVoxelKey Key = LVoxelKey::FromWorldSpace(Hit.GlobalWorldLocation);
            HitChunk->ModifySingleLocalVoxel(Key, ECompileTimeVoxels::Air);
            break;
        }

        continue;
    }

    return;
}

void Jafg::APawn::OnOngoingSecondaryInput(LInputActionValue& InValue)
{
    check( this->GetWorld() )

    for (const LHitResult& Hit : this->CurrentGenericTraceResults)
    {
        if (AChunk* HitChunk = Hit.Actor->As<AChunk>(); HitChunk)
        {
            const JVoxelSubsystem* Vs = GEngine->GetSubsystemChecked<JVoxelSubsystem>();
            const LVoxelKey Key = HitChunk->CreateRelativeVoxelKey(Hit.GlobalWorldLocation + Hit.SurfaceNormal.value() * 0.5f);
            HitChunk->ModifySingleVoxelByNonZeroOrigin(Key, Vs->GetCheckedVoxelIndex("Stone"));
            break;
        }

        continue;
    }

    return;
}

bool Jafg::APawn::TraceFromEyeByChannel(
    TArray<LHitResult>& OutHits,
    const float DistanceInMeters,
    const ECollisionChannel::Type Channel,
    const LCollisionQueryParams& Params
) const
{
    return false;
}

void Jafg::APawn::UpdateRelativeVectors()
{
    this->RelativeFront.X =
        Maths::Cos(Maths::ToRadians(this->GetRotator().Yaw)) * Maths::Cos(Maths::ToRadians(this->GetRotator().Pitch));
    this->RelativeFront.Y =
        Maths::Sin(Maths::ToRadians(this->GetRotator().Yaw)) * Maths::Cos(Maths::ToRadians(this->GetRotator().Pitch));
    this->RelativeFront.Z =
        Maths::Sin(Maths::ToRadians(this->GetRotator().Pitch));
    this->RelativeFront.Normalize();

    this->RelativeRight = this->RelativeFront.Cross(LVector::UpVector).NormalizeRet().InvertRet();
    this->RelativeUp    = this->RelativeRight.Cross(this->RelativeFront).NormalizeRet().InvertRet();

    return;
}
