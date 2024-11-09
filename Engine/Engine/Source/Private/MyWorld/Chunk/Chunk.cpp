// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "MyWorld/Chunk/Chunk.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "MyWorld/MyWorldStatics.h"
#include "JustTemp.h"
#include "Engine/Framework/Camera.h"
#include "MyWorld/Blocks.h"
#include "MyWorld/WorldGen.h"
#include "MyWorld/WorldStatics.h"
#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "MyWorld/Meshing/ChunkMesher.h"
#include "RhiFramework/ChunkShaderContext.h"

// C4553

Jafg::LChunkRendererComponent::LChunkRendererComponent(AChunk& Owner)
{
    this->Owner = &Owner;
}

void Jafg::LChunkRendererComponent::Draw(const LViewport& Context)
{
    checkSlow( this->Owner->SharedArgs )

    LChunkShaderContext* ShaderContext = this->Owner->SharedArgs->ChunkGenerationSubsystem->GetChunkShaderContext();

    LChunkShaderDrawArgs Args;
    Args.DegYFov   = this->Owner->GetWorld()->MainCamera->Zoom;
    Args.ViewMatrix.CopyFrom(this->Owner->GetWorld()->MainCamera->GetViewMatrix());
    ShaderContext->Draw(Context, Args);

    const uint32 ModelLoc = JustTemp::D(ShaderContext->GetProgram());

    this->Owner->Render(ModelLoc);

    return;
}

void Jafg::AChunk::BeginLife()
{
    Super::BeginLife();

    check( this->SharedArgs )

    this->ChunkState = EChunkState::Freed;

    this->bReady        = false;
    this->bGenerated    = false;

    return;
}

void Jafg::AChunk::EndLife()
{
    Super::EndLife();

    JustTemp::E(&VertexArrayObject, &Vbo, &Vbo);

    if (this->Mesher)
    {
        delete this->Mesher;
        this->Mesher = nullptr;
    }

    if (this->RawVoxelData)
    {
        delete[] RawVoxelData;
        RawVoxelData = nullptr;
    }

    return;
}

void Jafg::AChunk::Render(unsigned int ModelLoc)
{
    if (!bReady)
    {
        if (bGenerated)
        {
            JustTemp::F(&VertexArrayObject, &Vbo, &Ebo, &this->Mesher->GetVertices(), &this->Mesher->GetIndices());
            // numTriangles = static_cast<unsigned int>( indices.size() );
            //
            // glGenVertexArrays(1, &vertexArrayObject);
            // glBindVertexArray(vertexArrayObject);
            //
            // glGenBuffers(1, &vbo);
            // glBindBuffer(GL_ARRAY_BUFFER, vbo);
            // glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
            //
            // glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, posX)));
            // glEnableVertexAttribArray(0);
            // glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texGridX)));
            // glEnableVertexAttribArray(1);
            //
            // glGenBuffers(1, &ebo);
            // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            // glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(),
            //              GL_STATIC_DRAW);
            bReady = true;
        }

        return;
    }

    //std::cout << "Rendering chunk " << chunkPos.x << ", " << chunkPos.y << ", " << chunkPos.z << '\n'
    //	<< "Chunk VAO: " << vertexArrayObject << '\n' << "Triangles: " << numTriangles << '\n';

    JustTemp::G(&VertexArrayObject, this->Mesher->GetNumTriangles(), &WorldLocation, &ModelLoc);

    // glBindVertexArray(vertexArrayObject);
    //
    // glm::mat4 model = glm::mat4(1.0f);
    // model = glm::
    // translate(model, worldPos);
    // glUniformMatrix4fv(static_cast<GLint>(modelLoc), 1, GL_FALSE, glm::value_ptr(model));
    //
    // glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(numTriangles), GL_UNSIGNED_INT, 0);

    return;
}

void Jafg::AChunk::SetChunkState(const EChunkState::Type NewChunkState)
{
    if (this->IsStateChangeValid(NewChunkState) == false)
    {
        LOG_FATAL(
            LogChunkValidation,
            "Encountered invalid state change from {} to {}",
            LexToString(this->ChunkState),
            LexToString(NewChunkState)
        )

        return;
    }

    this->ChunkState = NewChunkState;

    switch (this->ChunkState)
    {
    case EChunkState::Spawned: { this->OnSpawned(); break; }
    case EChunkState::Shaped: { this->Shape(); break; }
    case EChunkState::SurfaceReplaced: { this->ReplaceSurface(); break; }
    case EChunkState::Active: { this->OnActive(); break; }
    default: { break; }
    }

    return;
}

void Jafg::AChunk::OnAlloc(const LChunkKey& InChunkKey)
{
    check( this->GetChunkState() == EChunkState::Freed )

    this->ChunkKey = InChunkKey;
    this->ChunkPos = glm::vec3(this->ChunkKey.X, this->ChunkKey.Y, this->ChunkKey.Z);
    this->WorldLocation = this->ChunkKey.ToWorldSpaceVector();

    this->SetChunkState(EChunkState::PreSpawned);

    return;
}

void Jafg::AChunk::SetChunkPersistency(const EChunkPersistency::Type NewPersistency, const float TimeToLive /* = 10.0f */)
{
    this->ChunkPersistency = NewPersistency;

    if (this->ChunkPersistency == EChunkPersistency::Persistent)
    {
        return;
    }

    if (
        const float EndOfLive = this->GetWorld()->GetRealTimeSecondsSinceWorldLaunch() + TimeToLive;
        EndOfLive > this->RealTimeInSecondsWhenTransientChunkShouldBeKilled
    )
    {
        this->RealTimeInSecondsWhenTransientChunkShouldBeKilled = EndOfLive;
    }

    return;
}

bool Jafg::AChunk::IsStateChangeValid(const EChunkState::Type NewChunkState) const
{
    switch (NewChunkState)
    {
    case EChunkState::Invalid:
    {
        return false;
    }
    case EChunkState::Freed:
    {
        return true;
    }
    case EChunkState::PreSpawned:
    {
        return this->ChunkState == EChunkState::Freed;
    }
    case EChunkState::Spawned:
    {
        return this->ChunkState == EChunkState::PreSpawned;
    }
    case EChunkState::Shaped:
    {
        return this->ChunkState == EChunkState::Spawned;
    }
    case EChunkState::SurfaceReplaced:
    {
        return this->ChunkState == EChunkState::Shaped;
    }
    case EChunkState::Active:
    {
        return this->ChunkState == EChunkState::SurfaceReplaced;
    }
    default:
    {
        checkNoEntry()
        return false;
    }
    }
}

void Jafg::AChunk::OnSpawned()
{
    check( this->ChunkState == EChunkState::Spawned )

    JChunkGenerationSubsystem* Subsystem = this->SharedArgs->ChunkGenerationSubsystem;

    this->NNorth = Subsystem->GetPanickedChunk(this->ChunkKey.GetNorthKey());
    this->NEast  = Subsystem->GetPanickedChunk(this->ChunkKey.GetEastKey());
    this->NSouth = Subsystem->GetPanickedChunk(this->ChunkKey.GetSouthKey());
    this->NWest  = Subsystem->GetPanickedChunk(this->ChunkKey.GetWestKey());
    this->NUp    = Subsystem->GetPanickedChunk(this->ChunkKey.GetUpKey());
    this->NDown  = Subsystem->GetPanickedChunk(this->ChunkKey.GetDownKey());

    checkSlow( this->NNorth )
    checkSlow( this->NEast  )
    checkSlow( this->NSouth )
    checkSlow( this->NWest  )
    checkSlow( this->NUp    )
    checkSlow( this->NDown  )

    return;
}

void Jafg::AChunk::Shape()
{
    check( this->ChunkState == EChunkState::Shaped )

    this->RawVoxelData = new uint32[MwStatics::VoxelCount];
    ::memset(this->RawVoxelData, 0, MwStatics::VoxelCount * sizeof(uint32));

    WorldGen::GenerateChunkData(
        static_cast<int>(ChunkPos.x), static_cast<int>(ChunkPos.y), static_cast<int>(ChunkPos.z),
        WorldStatics::ChunkSize, RawVoxelData);

    return;
}

void Jafg::AChunk::ReplaceSurface()
{
    check( this->ChunkState == EChunkState::SurfaceReplaced )
}

void Jafg::AChunk::OnActive()
{
    check( this->ChunkState == EChunkState::Active )

    check( this->Mesher == nullptr )
    check( this->HasRendererComponent() == false )

    this->SetRendererComponent(new LChunkRendererComponent(*this));
    checkSlow( this->HasRendererComponent() )

    this->Mesher = this->SharedArgs->GetNewMesher(*this);
    checkSlow( this->Mesher )

    this->Mesher->RegenerateProceduralMesh();

    return;
}
