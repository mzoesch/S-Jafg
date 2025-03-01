// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "MyWorld/Chunk/Chunk.h"
#include "MyWorld/Meshing/ChunkMesher.h"
#include "Engine/Engine.h"
#include "System/VoxelSubsystem.h"
#include "System/MaterialSubsystem.h"

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
    check( GEngine )
    this->RegenerateProceduralMesh(
        GEngine->GetSubsystem<JVoxelSubsystem>(),
        GEngine->GetSubsystem<JMaterialSubsystem>()
    );
}
