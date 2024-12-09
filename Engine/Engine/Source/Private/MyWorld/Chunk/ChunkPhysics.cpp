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

bool Jafg::LChunkPhysicsComponent::Overlaps(const LVector& Point) const
{
    if (this->IsInTheoreticalMaxBounds(Point) == false)
    {
        return false;
    }

    return this->Owner->GetRawVoxelData(LVoxelKey::CreateFromWorldLocation(Point)) > ECompileTimeVoxels::Air;
}

bool Jafg::LChunkPhysicsComponent::Sweep(const LVector& Start, const LVector& End, LHitResult& OutHit) const
{
    LVector Cursor   = Start;
    float   Distance = (End - Start).Magnitude();
    const LVector Normal = (End - Start).GetUnsafeNormalized();

    if (Owner->ChunkKey == LChunkKey(1, 0 ,1))
    {
        LOG_WARNING(LogTemporal, "{} :: {} {} ({})", Start.ToString(), Owner->GetTranslation().ToString(),
            Owner->ChunkKey.ToString(), Normal.ToString())
    }

    while (Distance > 0)
    {
        if (this->Overlaps(Cursor))
        {
            OutHit.Actor               = this->Owner;
            OutHit.GlobalWorldLocation = Cursor;
            return true;
        }

        Cursor += Normal * LChunkPhysicsComponent::TraceStep;
        Distance -= LChunkPhysicsComponent::TraceStep;
    }

    return false;
}
