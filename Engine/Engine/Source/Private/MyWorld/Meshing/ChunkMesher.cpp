// Copyright mzoesch. All rights reserved.

#include "MyWorld/Chunk/Chunk.h"
#include "MyWorld/Meshing/ChunkMesher.h"
#include "Engine/Engine.h"
#include "Stats/Stats.h"

void Jafg::LChunkMesher::ClearProceduralMesh()
{
    check( this->Mutex.try_lock() == false )

    algo::orphan(&this->Vertices);
    algo::orphan(&this->Indices);

    return;
}

void Jafg::LChunkMesher::ApplyProceduralMesh()
{
    check( this->Mutex.try_lock() == false )

    STAT_CYCLE_FUNCTION()

    if (Tasks::IsOnRendererThread())
    {
        check( this->Owner->IsRendererComponentValid() )
        this->Owner->GetChunkRendererComponent()->GetShaderInstance()->LoadMeshToGraphicsMemory(this->Vertices, this->Indices);
        this->NumTriangles = this->Indices.size();
    }
    else
    {
        Tasks::Make(ENamedThreads::Renderer, ETaskTime::Whenever, [this](void) -> void
        {
            if (this->Owner->GetWorld()->GetWorldState() > EWorldState::Running)
            {
                return;
            }

            check
            (
                   this->Owner->GetCurrentStateDangerous()       == EChunkState::Active
                || this->Owner->GetCurrentHuntedStateDangerous() == EChunkState::Active
            )

            check( this->Owner->IsRendererComponentValid() )
            this->Owner->GetChunkRendererComponent()->GetShaderInstance()->LoadMeshToGraphicsMemory(this->Vertices, this->Indices);
            this->NumTriangles = this->Indices.size();

            if (this->Owner->GetCurrentHuntedStateDangerous() == EChunkState::Active)
            {
                this->Owner->FinishHunt(EChunkState::Active);
            }

            check( this->Owner->GetCurrentHuntedStateDangerous() == EChunkState::Invalid )

            return;
        });
    }

    return;
}
