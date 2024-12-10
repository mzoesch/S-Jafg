// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "MyWorld/Chunk/Chunk.h"
#include "MyWorld/Generation/ChunkGenerator.h"
#include "Engine/Engine.h"
#include "MyWorld/MyWorldStatics.h"
#include "Engine/Framework/Pawn.h"
#include "Engine/Framework/PersonaController.h"
#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "MyWorld/Meshing/ChunkMesher.h"
#include "User/LocalEgo.h"
#include "Rhi/ChunkShaderContext.h"
#include "MyWorld/Chunk/ChunkPhysics.h"

Jafg::LChunkRendererComponent::LChunkRendererComponent(AChunk& Owner)
{
    this->Owner = &Owner;
}

void Jafg::LChunkRendererComponent::Draw(const LViewport& Context)
{
    checkSlow( this->Owner->GetSharedArgs() )

    const LChunkShaderContext* ShaderContext =
        this->Owner->GetSharedArgs()->ChunkGenerationSubsystem->GetChunkShaderContext();

    const LEye* Eye = GEngine->GetCheckedLocalEgo()->GetPossessed()->GetPossessed()->GetEye();

    LChunkShaderDrawArgs Args;
    Args.DegYFov = Eye->GetDegYFov();
    Args.ViewMatrix.CopyFrom(Eye->GetViewMatrix());
    Args.WorldLocation = this->Owner->GetTranslation();
    Args.NumTriangles = this->Owner->GetMesher()->GetNumTriangles();
    Args.Instance = &this->Instance;
    ShaderContext->Draw(Context, Args);

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
    this->SetTranslation(this->ChunkKey.ToWorldSpaceVector());

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
    // ::memset(this->RawVoxelData, 0, MwStatics::VoxelCount * sizeof(uint32));

    ChunkGenerator::ShapeChunk(this->SharedArgs, this->ChunkKey, this->RawVoxelData);

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

    this->Mesher = this->SharedArgs->GetNewMesher(*this);
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
