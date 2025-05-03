// Copyright mzoesch. All rights reserved.

#include "MyWorld/Chunk/Chunk.h"
#include "MyWorld/Generation/ChunkGenerator.h"
#include "Engine/Engine.h"
#include "MyWorld/MyWorldStatics.h"
#include "Framework/Pawn.h"
#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "MyWorld/Meshing/ChunkMesher.h"
#include "MyWorld/Chunk/ChunkPhysics.h"
#include "Rhi/RhiVendorInclude.h"

bool Jafg::LChunkRendererComponent::Cull(const std::span<LVector>& Corners) const
{
    return false;
}

void Jafg::LChunkRendererComponent::Draw(const LViewport& Context, const LEye& Eye)
{
    checkSlow( this->Owner->GetSharedArgs() )

    LChunkShader& Shader = this->Owner->GetSharedArgs()->ChunkShader;

    Shader.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Shader.GetBlendOpaqueTextureLocation());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Shader.GetBlendersTextureLocation());

    glBindVertexArray(this->Instance.GetVertexArrayObject());

    LMatrix Model; Model.InlineTranslate(this->Owner->GetTranslation());
    Shader.GetProgram().SetMatrixUniform("Model", Model);

    glDrawElements(GL_TRIANGLES, this->Owner->GetMesher()->GetNumTriangles(), GL_UNSIGNED_INT, nullptr);

    return;
}

Jafg::AChunk::AChunk(const LObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
    this->DisableStrongActorContext();
    return;
}

void Jafg::AChunk::BeginLife()
{
    Super::BeginLife();

    check( this->SharedArgs )

    this->ChunkState = EChunkState::Freed;

    checkSlow( this->RawVoxelData == nullptr )
    checkSlow( this->Mesher == nullptr )
    checkSlow( this->NNorth == nullptr )
    checkSlow( this->NEast  == nullptr )
    checkSlow( this->NSouth == nullptr )
    checkSlow( this->NWest  == nullptr )
    checkSlow( this->NUp    == nullptr )
    checkSlow( this->NDown  == nullptr )

    return;
}

void Jafg::AChunk::EndLife()
{
    Super::EndLife();

    this->Mesher.Reset();
    this->RawVoxelData.Reset();

    return;
}

void Jafg::AChunk::SetChunkState(const EChunkState::Type NewChunkState)
{
    if (this->IsStateChangeValid(NewChunkState) == false)
    {
        panicMsgf
        (
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
    this->SetTranslation(this->ChunkKey.ToWorldSpace());

    this->SetChunkState(EChunkState::PreSpawned);

    return;
}

void Jafg::AChunk::SetChunkPersistency(const EChunkPersistency::Type NewPersistency, const f32 TimeToLive /* = 10.0f */) noexcept
{
    this->ChunkPersistency = NewPersistency;

    if (this->ChunkPersistency == EChunkPersistency::Persistent)
    {
        return;
    }

    if
    (
        const f32 EndOfLive = this->GetWorld()->GetRealTimeSecondsSinceWorldLaunch() + TimeToLive;
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

    const JChunkGenerationSubsystem* Subsystem = this->SharedArgs->ChunkGenerationSubsystem;

    this->NNorth = Subsystem->FindChunkChecked(this->ChunkKey.GetNorthKey());
    this->NEast  = Subsystem->FindChunkChecked(this->ChunkKey.GetEastKey());
    this->NSouth = Subsystem->FindChunkChecked(this->ChunkKey.GetSouthKey());
    this->NWest  = Subsystem->FindChunkChecked(this->ChunkKey.GetWestKey());
    this->NUp    = Subsystem->FindChunkChecked(this->ChunkKey.GetUpKey());
    this->NDown  = Subsystem->FindChunkChecked(this->ChunkKey.GetDownKey());

    return;
}

void Jafg::AChunk::Shape()
{
    check( this->ChunkState == EChunkState::Shaped )

    struct HelperMalloc
    {
        u32 Data[MwStatics::ChunkSizeCubed];
    };
    check( this->RawVoxelData == nullptr )
    this->RawVoxelData = Smart::MakeUnique(reinterpret_cast<u32*>(new HelperMalloc));

    ChunkGenerator::ShapeChunk(this->SharedArgs, this->ChunkKey, this->RawVoxelData.GetValuePtrChecked());

    return;
}

void Jafg::AChunk::ReplaceSurface()
{
    check( this->ChunkState == EChunkState::SurfaceReplaced )
    ChunkGenerator::ReplaceSurface(this->SharedArgs, this->ChunkKey, this, this->RawVoxelData);
    return;
}

void Jafg::AChunk::OnActive()
{
    check( this->ChunkState == EChunkState::Active )

    check( this->Mesher == nullptr )
    check( this->IsRendererComponentValid() == false )

    this->SetRendererComponent(new LChunkRendererComponent(*this));
    checkSlow( this->IsRendererComponentValid() )
    this->SetPhysicsComponent(new LChunkPhysicsComponent(this));

    this->Mesher = Smart::MakeUnique(this->SharedArgs->GetNewMesher(*this));
    checkSlow( this->Mesher )

    this->Mesher->RegenerateProceduralMesh();

    return;
}

void Jafg::AChunk::ModifySingleLocalVoxel(const LVoxelKey InKey, const voxel_t NewVoxel)
{
    check( this->HasRawVoxelData() )
    check( InKey.IsLocal() )

    if (this->GetRawVoxelData(InKey) == NewVoxel)
    {
        LOG_WARNING(LogChunkManipulation, "Attempted to modify voxel to the same value [{}]", NewVoxel)
        return;
    }

    this->OverrideRawVoxelData(InKey, NewVoxel);

    check( this->IsMesherValid() )
    this->GetMesher()->RegenerateProceduralMesh(this->SharedArgs->VoxelSubsystem, this->SharedArgs->MaterialSubsystem);

    return;
}

void Jafg::AChunk::ModifySingleVoxelByNonZeroOrigin(LVoxelKey InKey, const voxel_t NewVoxel)
{
    AChunk* Target = this->GetNeighboringChunk(&InKey);
    Target->ModifySingleLocalVoxel(InKey, NewVoxel);
    return;
}
