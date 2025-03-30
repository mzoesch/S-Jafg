// Copyright mzoesch. All rights reserved.

#include "MyWorld/CommonTypes.h"
#include "MyWorld/Chunk/ChunkPhysics.h"

bool Jafg::LChunkPhysicsComponent::IsInTheoreticalMaxBounds(const LVector& Point) const
{
    checkSlow( this->Owner )
    return
           this->Owner->GetTranslation().X <= Point.X && Point.X <= this->Owner->GetTranslation().X + MwStatics::ChunkSize
        && this->Owner->GetTranslation().Y <= Point.Y && Point.Y <= this->Owner->GetTranslation().Y + MwStatics::ChunkSize
        && this->Owner->GetTranslation().Z <= Point.Z && Point.Z <= this->Owner->GetTranslation().Z + MwStatics::ChunkSize;
}

Jafg::TOptional<Jafg::LVector> Jafg::LChunkPhysicsComponent::GetNormalAtLocation(const LVector& InLocation, const LVector& InTraceNormal) const
{
    LVector P1, P2, P3; // Clockwise

    LVector Origin = InLocation.GetFloor();
    LVector P1_Hit = InLocation - Origin;

    P1 = Origin + LVector(
        Maths::IsNearlyEqual(P1_Hit.X, 0.0f, 0.001f) ? Maths::Floor(P1_Hit.X) : Maths::Ceil(P1_Hit.X),
        Maths::IsNearlyEqual(P1_Hit.Y, 0.0f, 0.001f) ? Maths::Floor(P1_Hit.Y) : Maths::Ceil(P1_Hit.Y),
        Maths::IsNearlyEqual(P1_Hit.Z, 0.0f, 0.001f) ? Maths::Floor(P1_Hit.Z) : Maths::Ceil(P1_Hit.Z)
    );

         if (Maths::IsNearlyEqual(P1_Hit.X, 0.0f, 0.001f)) { P2 = P1 + LVector::Left();     P3 = P1 + LVector::Down();     }
    else if (Maths::IsNearlyEqual(P1_Hit.X, 1.0f, 0.001f)) { P2 = P1 + LVector::Left();     P3 = P1 + LVector::Down();     }
    else if (Maths::IsNearlyEqual(P1_Hit.Y, 0.0f, 0.001f)) { P2 = P1 + LVector::Down();     P3 = P1 + LVector::Backward(); }
    else if (Maths::IsNearlyEqual(P1_Hit.Y, 1.0f, 0.001f)) { P2 = P1 + LVector::Down();     P3 = P1 + LVector::Backward(); }
    else if (Maths::IsNearlyEqual(P1_Hit.Z, 0.0f, 0.001f)) { P2 = P1 + LVector::Backward(); P3 = P1 + LVector::Left();     }
    else if (Maths::IsNearlyEqual(P1_Hit.Z, 1.0f, 0.001f)) { P2 = P1 + LVector::Backward(); P3 = P1 + LVector::Left();     }
    else
    {
        return { };
    }

    LVector OutNormal = (P2 - P1).Cross(P3 - P1).GetUnsafeNormalized();
    if ((OutNormal | InTraceNormal.GetNormalized()) > 0.0f)
    {
        OutNormal = -OutNormal;
    }

    return OutNormal;
}

bool Jafg::LChunkPhysicsComponent::Overlaps(const LVector& Point) const
{
    if (this->IsInTheoreticalMaxBounds(Point) == false)
    {
        return false;
    }

    return this->Owner->GetRawVoxelData(LVoxelKey::FromWorldSpace(Point)) > ECompileTimeVoxels::Air;
}

bool Jafg::LChunkPhysicsComponent::Sweep(const LVector& Start, const LVector& End, LHitResult& OutHit) const
{
    LVector Cursor   = Start;
    float   Distance = (End - Start).Magnitude();
    const LVector Normal = (End - Start).GetUnsafeNormalized();
    const LVector NormalStep = Normal * LChunkPhysicsComponent::TraceStep;

    while (Distance > 0)
    {
        if (this->Overlaps(Cursor))
        {
            OutHit.Actor               = this->Owner;
            OutHit.GlobalWorldLocation = Cursor;
            OutHit.SurfaceNormal       = this->GetNormalAtLocation(Cursor, Normal);
            return true;
        }

        Cursor += NormalStep;
        Distance -= LChunkPhysicsComponent::TraceStep;

        continue;
    }

    return false;
}
