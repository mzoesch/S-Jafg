// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/Framework/Pawn.h"
#include "Debug/DebugTraceLine.h"
#include "Debug/DebugTraceSphere.h"
#include "MyWorld/Chunk/Chunk.h"
#include "User/Input/InputActionValue.h"

Jafg::APawn::APawn(const LObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
    this->SetEverTickConstructorOnlyFlag();
    return;
}

void Jafg::APawn::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);

    this->CurrentGenericTraceResults.Empty();
    const LVector TraceStart = this->GetTranslation();
    const LVector TraceEnd   = this->GetTranslation() + this->GetRotator().ToVector() * 5.0f;
    this->GetWorld()->LineTraceByChannel(
        this->CurrentGenericTraceResults, TraceStart, TraceEnd,
        ECollisionChannel::Static, LCollisionQueryParams({.bSingleHit = true})
    );

    return;
}

void Jafg::APawn::DeclareNewPossessor(APersonaController* InNewController)
{
    this->OwningController = InNewController;

#if WITH_LOCAL_LAYER
    if (InNewController)
    {
        this->Eye = new LEye(this);
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

    this->GetMutableRotator().ConstrainAxis(ERotatorAxis::Pitch, 89.9f);
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
            const LVoxelKey Key = LVoxelKey::FromWorldLocation(Hit.GlobalWorldLocation);
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
            const LVoxelKey Key = HitChunk->CreateRelativeVoxelKey(Hit.GlobalWorldLocation + Hit.SurfaceNormal * 0.5f);
            HitChunk->ModifySingleVoxelByNonZeroOrigin(Key, ECompileTimeVoxels::Num);
            break;
        }

        continue;
    }

    return;
}

bool Jafg::APawn::TraceFromEyeByChannel(
    TdhArray<LHitResult>& OutHits,
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
