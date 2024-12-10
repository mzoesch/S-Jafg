// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
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

Jafg::LVector Jafg::LChunkPhysicsComponent::GetNormalAtLocation(const LVector& InLocation, const LVector& InTraceNormal) const
{
    LVector Out = LVector::Zero();

    LVector Dummy = InLocation.GetModF();

    if (InTraceNormal.X < 0 && Maths::IsNearlyEqual(Dummy.X, 1.0f, LChunkPhysicsComponent::TraceStep))
    {
        Dummy.X = Maths::Floor(Dummy.X);
    }
    if (InTraceNormal.Y < 0 && Maths::IsNearlyEqual(Dummy.Y, 1.0f, LChunkPhysicsComponent::TraceStep))
    {
        Dummy.Y = Maths::Floor(Dummy.Y);
    }
    if (InTraceNormal.Z < 0 && Maths::IsNearlyEqual(Dummy.Z, 1.0f, LChunkPhysicsComponent::TraceStep))
    {
        Dummy.Z = Maths::Floor(Dummy.Z);
    }

    const EVectorAxis::Type InferiorAxis = Dummy.GetMostInferiorAxis();

    if (InferiorAxis == EVectorAxis::X)
    {
        Out.X = InTraceNormal.X < 0 ? 1.0f : -1.0f;
    }
    else if (InferiorAxis == EVectorAxis::Y)
    {
        Out.Y = InTraceNormal.Y < 0 ? 1.0f : -1.0f;
    }
    else if (InferiorAxis == EVectorAxis::Z)
    {
        Out.Z = InTraceNormal.Z < 0 ? 1.0f : -1.0f;
    }
    else
    {
        panic( "Invalid most inferior axis." )
    }

    check( Out.IsNormalized() )

    return Out;
}

bool Jafg::LChunkPhysicsComponent::Overlaps(const LVector& Point) const
{
    if (this->IsInTheoreticalMaxBounds(Point) == false)
    {
        return false;
    }

    return this->Owner->GetRawVoxelData(LVoxelKey::FromWorldLocation(Point)) > ECompileTimeVoxels::Air;
}

bool Jafg::LChunkPhysicsComponent::Sweep(const LVector& Start, const LVector& End, LHitResult& OutHit) const
{
    LVector Cursor   = Start;
    float   Distance = (End - Start).Magnitude();
    const LVector Normal = (End - Start).GetUnsafeNormalized();

    while (Distance > 0)
    {
        if (this->Overlaps(Cursor))
        {
            OutHit.Actor               = this->Owner;
            OutHit.GlobalWorldLocation = Cursor;
            OutHit.SurfaceNormal       = this->GetNormalAtLocation(Cursor, Normal);
            return true;
        }

        Cursor += Normal * LChunkPhysicsComponent::TraceStep;
        Distance -= LChunkPhysicsComponent::TraceStep;
    }

    return false;
}
