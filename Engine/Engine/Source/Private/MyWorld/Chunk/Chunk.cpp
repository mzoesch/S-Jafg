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
#include "Stats/Stats.h"
#include "System/VoxelSubsystem.h"
#include "System/MaterialSubsystem.h"

bool Jafg::LChunkRendererComponent::Cull(const std::span<LVector>& Corners) const
{
    return false;
}

void Jafg::LChunkRendererComponent::Draw(const LViewport& Context, const LEye& Eye)
{
    check( Tasks::IsOnRendererThread() )

    check( this->Owner.IsMesherValid() )
    checkSlow( this->Owner.GetSharedArgs() && this->Owner.GetSharedArgs()->ChunkShader.IsValid() )

    const LChunkShader& Shader = this->Owner.GetSharedArgs()->ChunkShader;

    Shader.Use();

    glBindTexture(GL_TEXTURE_2D, Shader.GetBlendOpaqueTextureLocation());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Shader.GetBlendersTextureLocation());

    glBindVertexArray(this->Instance.GetVertexArrayObject());
    glBindBuffer(GL_ARRAY_BUFFER, this->Instance.GetVertexBufferObject());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->Instance.GetElementBufferObject());

    LMatrix Model; Model.InlineTranslate(this->Owner.GetTranslation());
    Shader.GetProgram().SetMatrixUniform("Model", Model);

    check( this->Owner.IsMesherValid() )
    glDrawElements(GL_TRIANGLES, this->Owner.GetMesher()->GetNumTriangles(), GL_UNSIGNED_INT, nullptr);

    glActiveTexture(GL_TEXTURE0);

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

    checkSlow( this->RawVoxelData == nullptr )
    checkSlow( this->NNorth       == nullptr )
    checkSlow( this->NEast        == nullptr )
    checkSlow( this->NSouth       == nullptr )
    checkSlow( this->NWest        == nullptr )
    checkSlow( this->NUp          == nullptr )
    checkSlow( this->NDown        == nullptr )

    this->SetTranslation(this->ChunkKey.ToWorldSpace());

    this->SetState(EChunkState::Spawned);

    return;
}

void Jafg::AChunk::EndLife()
{
    Super::EndLife();

    this->SharedArgs = nullptr;
    this->Mesher.reset();
    this->RawVoxelData.reset();

    return;
}

void Jafg::AChunk::SetState
(
    const EChunkState::Type NewChunkState,
    const bool bKeepHunt, /* = false */
    const f32 WaitTime, /* = 0.5f */
    const std::atomic_bool* bAbort, /* = nullptr */
    bool* bOutChanged /* = nullptr */
)
{
    STAT_CYCLE_FUNCTION()

    if (bAbort)
    {
        bool bLocked { false };
        while (bAbort->load() == false)
        {
            if (this->ChunkStateMutex.try_lock_for(std::chrono::nanoseconds(static_cast<i64>(WaitTime * LAL_S2NS_F))))
            {
                if (bOutChanged)
                {
                    *bOutChanged = true;
                }

                bLocked = true;
                break;
            }

            continue;
        }

        if (bLocked == false)
        {
            check( bAbort->load() )

            if (bOutChanged)
            {
                *bOutChanged = false;
            }
            else
            {
                LOG_VERBOSE
                (
                    LogChunkMisc,
                    "Failed to update chunk state [{}] to [{}] of chunk [{}].",
                    LexToString(this->GetCurrentStateDangerous()), LexToString(NewChunkState), this->GetChunkKey().ToString()
                )
            }

            return;
        }
    }
    else
    {
        this->ChunkStateMutex.lock();
    }

    if (this->State >= NewChunkState)
    {
        this->ChunkStateMutex.unlock();
        return;
    }

    check( this->HuntedState == EChunkState::Invalid )

    checkCode
    (
        if (this->IsStateChangeValid(NewChunkState) == false)
        {
            panicMsgf
            (
                "Encountered invalid state change from {} to {}",
                LexToString(this->State),
                LexToString(NewChunkState)
            )
            this->ChunkStateMutex.unlock();
            return;
        }
    )

    this->HuntedState = NewChunkState;

    switch (NewChunkState)
    {
    case EChunkState::Spawned:         { this->Spawn(); break; }
    case EChunkState::Shaped:          { this->Shape(); break; }
    case EChunkState::SurfaceReplaced: { this->ReplaceSurface(); break; }
    case EChunkState::Active:          { this->Activate(); break; }
    default: { break; }
    }

    if (bKeepHunt == false)
    {
        this->State       = NewChunkState;
        this->HuntedState = EChunkState::Invalid;

        this->ChunkStateMutex.unlock();
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
    case EChunkState::Spawned:
    {
        return this->State == EChunkState::Invalid;
    }
    case EChunkState::Shaped:
    {
        return this->State == EChunkState::Spawned;
    }
    case EChunkState::SurfaceReplaced:
    {
        return this->State == EChunkState::Shaped;
    }
    case EChunkState::Active:
    {
        return this->State >= EChunkState::Spawned && this->State <= EChunkState::SurfaceReplaced;
    }
    default:
    {
        checkNoEntry()
        return false;
    }
    }
}

void Jafg::AChunk::Spawn()
{
    STAT_CYCLE_FUNCTION()

    check( Tasks::IsOnMasterThread() )

    check( this->HuntedState == EChunkState::Spawned )

    const JChunkGenerationSubsystem* Subsystem = this->SharedArgs->ChunkGenerationSubsystem;

    this->NNorth = Subsystem->FindChunk(this->ChunkKey.GetNorthKey());
    this->NEast  = Subsystem->FindChunk(this->ChunkKey.GetEastKey());
    this->NSouth = Subsystem->FindChunk(this->ChunkKey.GetSouthKey());
    this->NWest  = Subsystem->FindChunk(this->ChunkKey.GetWestKey());
    this->NUp    = Subsystem->FindChunk(this->ChunkKey.GetUpKey());
    this->NDown  = Subsystem->FindChunk(this->ChunkKey.GetDownKey());

    if (this->NNorth)
    {
        check( this->NNorth->NSouth == nullptr )
        this->NNorth->NSouth = this;
    }
    if (this->NEast)
    {
        check( this->NEast->NWest == nullptr )
        this->NEast->NWest = this;
    }
    if (this->NSouth)
    {
        check( this->NSouth->NNorth == nullptr )
        this->NSouth->NNorth = this;
    }
    if (this->NWest)
    {
        check( this->NWest->NEast == nullptr )
        this->NWest->NEast = this;
    }
    if (this->NUp)
    {
        check( this->NUp->NDown == nullptr )
        this->NUp->NDown = this;
    }
    if (this->NDown)
    {
        check( this->NDown->NUp == nullptr )
        this->NDown->NUp = this;
    }

    return;
}

void Jafg::AChunk::Shape()
{
    STAT_CYCLE_FUNCTION()

    check( Tasks::IsOnMasterThread() == false )

    check( this->HuntedState == EChunkState::Shaped )

    check( this->RawVoxelData.get() == nullptr )
    this->RawVoxelData = TUnique<voxel_t[]>{ new voxel_t[MwStatics::ChunkSizeCubed] };

    ChunkGenerator::ShapeChunk(this->SharedArgs, this->ChunkKey, this->RawVoxelData.get());

    return;
}

void Jafg::AChunk::ReplaceSurface()
{
    STAT_CYCLE_FUNCTION()

    check( Tasks::IsOnMasterThread() == false )

    check( this->HuntedState == EChunkState::SurfaceReplaced )
    ChunkGenerator::ReplaceSurface(this->SharedArgs, this->ChunkKey, this, this->RawVoxelData.get());
    return;
}

void Jafg::AChunk::Activate()
{
    STAT_CYCLE_FUNCTION()

    check( Tasks::IsOnMasterThread() == false )

    check( this->HuntedState == EChunkState::Active )

    check( this->Mesher == nullptr )
    check( this->IsRendererComponentValid() == false )
    this->SetPhysicsComponent(new LChunkPhysicsComponent(this));

    this->Mesher = this->SharedArgs->GetNewMesher(*this);
    checkSlow( this->Mesher )

    {
        std::unique_lock Lock(this->Mesher->GetMutex());

        this->Mesher->ClearProceduralMesh();
        this->Mesher->GenerateProceduralMesh(GEngine->GetCheckedSubsystem<JVoxelSubsystem>(), GEngine->GetCheckedSubsystem<JMaterialSubsystem>());

        this->SetRendererComponent(new LChunkRendererComponent(*this));
        checkSlow( this->IsRendererComponentValid() )

        this->Mesher->ApplyProceduralMesh();
    }

    this->RegenerateNeighboringMeshes();

    return;
}

void Jafg::AChunk::RegenerateNeighboringMeshes() const
{
    const JVoxelSubsystem* Vs { GEngine->GetCheckedSubsystem<JVoxelSubsystem>() };
    const JMaterialSubsystem* Ms { GEngine->GetCheckedSubsystem<JMaterialSubsystem>() };

    if (this->NNorth && this->NNorth->GetCurrentStateDangerous() == EChunkState::Active)
    {
        check( this->NNorth->Mesher.get() != nullptr )
        this->NNorth->Mesher->RegenerateProceduralMesh(Vs, Ms);
    }

    if (this->NEast && this->NEast->GetCurrentStateDangerous() == EChunkState::Active)
    {
        check( this->NEast->Mesher.get() != nullptr )
        this->NEast->Mesher->RegenerateProceduralMesh(Vs, Ms);
    }

    if (this->NSouth && this->NSouth->GetCurrentStateDangerous() == EChunkState::Active)
    {
        check( this->NSouth->Mesher.get() != nullptr )
        this->NSouth->Mesher->RegenerateProceduralMesh(Vs, Ms);
    }

    if (this->NWest && this->NWest->GetCurrentStateDangerous() == EChunkState::Active)
    {
        check( this->NWest->Mesher.get() != nullptr )
        this->NWest->Mesher->RegenerateProceduralMesh(Vs, Ms);
    }

    if (this->NUp && this->NUp->GetCurrentStateDangerous() == EChunkState::Active)
    {
        check( this->NUp->Mesher.get() != nullptr )
        this->NUp->Mesher->RegenerateProceduralMesh(Vs, Ms);
    }

    if (this->NDown && this->NDown->GetCurrentStateDangerous() == EChunkState::Active)
    {
        check( this->NDown->Mesher.get() != nullptr )
        this->NDown->Mesher->RegenerateProceduralMesh(Vs, Ms);
    }

    return;
}

void Jafg::AChunk::ModifySingleLocalVoxel(const LVoxelKey InKey, const voxel_t NewVoxel)
{
    check( Tasks::IsOnMasterThread() )

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
