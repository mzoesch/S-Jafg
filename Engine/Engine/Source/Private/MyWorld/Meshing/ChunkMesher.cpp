// Copyright mzoesch. All rights reserved.

#include "MyWorld/Chunk/Chunk.h"
#include "MyWorld/Meshing/ChunkMesher.h"
#include "Engine/Engine.h"
#include "System/VoxelSubsystem.h"
#include "System/MaterialSubsystem.h"
#include "Stats/Stats.h"

Jafg::LChunkMesher::~LChunkMesher()
{
    this->ClearProceduralMesh();
}

void Jafg::LChunkMesher::ClearProceduralMesh()
{
    this->Vertices.Empty();
    this->Indices.Empty();
}

void Jafg::LChunkMesher::ApplyProceduralMesh()
{
    STAT_CYCLE_FUNCTION()

    checkSlow( this->Owner->IsRendererComponentValid() )

    if (Tasks::IsOnMasterThread())
    {
        this->Owner->GetChunkRendererComponent()->GetShaderInstance()->LoadMeshToGraphicsMemory(this->Vertices, this->Indices);
    }
    else
    {
        Tasks::Make(ENamedThreads::Master, ETaskTime::Whenever, [this](void) -> void
        {
            if (this->Owner->GetWorld()->GetWorldState() > EWorldState::Running)
            {
                return;
            }

            if (this->Owner->GetCurrentChunkStateDangerous() != EChunkState::Active)
            {
                LOG_WARNING(LogChunkGeneration, "Chunk [{}] is not active.", this->Owner->GetChunkKey().ToString())
                return;
            }

            this->Owner->GetChunkRendererComponent()->GetShaderInstance()->LoadMeshToGraphicsMemory(this->Vertices, this->Indices);

            return;
        });
    }


    return;
}

void Jafg::LChunkMesher::RegenerateProceduralMesh()
{
    check( GEngine )
    this->RegenerateProceduralMesh
    (
        GEngine->GetSubsystem<JVoxelSubsystem>(),
        GEngine->GetSubsystem<JMaterialSubsystem>()
    );

    return;
}
