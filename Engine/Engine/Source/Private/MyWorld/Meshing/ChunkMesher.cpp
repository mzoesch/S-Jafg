// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "MyWorld/Chunk/Chunk.h"
#include "MyWorld/Meshing/ChunkMesher.h"
#include "System/VoxelSubsystem.h"
#include "System/MaterialSubsystem.h"
#include "Engine/Framework/ApplicationInstance.h"

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
    checkSlow( this->Owner->IsRendererComponentValid() )

    this->Owner->GetChunkRendererComponent()->GetShaderInstance()->LoadMeshToGraphicsMemory(
        this->Vertices, this->Indices
    );

    return;
}

void Jafg::LChunkMesher::RegenerateProceduralMesh()
{
    this->RegenerateProceduralMesh(
        this->GetOwner().GetApplicationInstanceSubsystem()->GetSubsystem<JVoxelSubsystem>(),
        this->GetOwner().GetApplicationInstanceSubsystem()->GetSubsystem<JMaterialSubsystem>()
    );
}
