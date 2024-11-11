// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "MyWorld/Chunk/Chunk.h"
#include "MyWorld/Meshing/ChunkMesher.h"

Jafg::LChunkMesher::~LChunkMesher()
{
    this->ClearProceduralMesh();
}

void Jafg::LChunkMesher::ClearProceduralMesh()
{
    this->Vertices.Empty();
    this->Indices.Empty();

    return;
}

void Jafg::LChunkMesher::ApplyProceduralMesh() const
{
    checkSlow( this->Owner->IsRendererComponentValid() )

    this->Owner->GetChunkRendererComponent()->GetShaderInstance()->LoadMeshToGraphicsMemory(
        this->Vertices, this->Indices
    );

    return;
}
