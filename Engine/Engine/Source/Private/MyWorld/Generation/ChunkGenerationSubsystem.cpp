// Copyright mzoesch. All rights reserved.

#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "Framework/Pawn.h"
#include "MyWorld/Chunk/ChunkStates.h"
#include "MyWorld/Generation/ChunkGeneratorSubsystem.h"
#include "MyWorld/Meshing/NaiveMesher.h"
#include "System/VoxelSubsystem.h"
#include "System/MaterialSubsystem.h"
#include "System/TextureSubsystem.h"
#include "User/UserPreferences.h"
#include "Engine/Engine.h"
#include "Physics/PhysicCompontent.h"
#include "Rhi/ChunkShader.h"
#include "Stats/Stats.h"

void Jafg::JChunkGenerationSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    this->SetTickInterval(0.0f);

    this->LoadedChunks = std::unordered_map<LChunkKey, AChunk*>(0x7FF);

    this->SharedChunkArgs.ChunkGenerationSubsystem = this;
    this->SharedChunkArgs.ChunkGeneratorSubsystem  = Collection.GetCheckedSubsystem<JChunkGeneratorSubsystem>();
    this->SharedChunkArgs.VoxelSubsystem  = this->GetEngine()->GetCheckedSubsystem<JVoxelSubsystem>();
    this->SharedChunkArgs.MaterialSubsystem  = this->GetEngine()->GetCheckedSubsystem<JMaterialSubsystem>();
    this->SharedChunkArgs.TextureSubsystem  = this->GetEngine()->GetCheckedSubsystem<JTextureSubsystem>();
    this->SharedChunkArgs.ChunkShaderHandle = this->SharedChunkArgs.ChunkShader.Make();
    this->SharedChunkArgs.GetNewMesher = [] (AChunk& Owner) -> LChunkMesher* { return new LNaiveMesher(Owner); };

    const JUserPreferences* Preferences = GetDefault<JUserPreferences>();
    this->RenderDistance = &Preferences->ChunkRenderDistance;
    this->RenderHeight   = &Preferences->ChunkRenderHeight;

    this->GetWorld()->OnStaticLineTrace.BindMember(this, &JChunkGenerationSubsystem::LineTraceByChannel);
    this->GetWorld()->OnStaticDraw.BindMember(this, &JChunkGenerationSubsystem::OnStaticDraw);

    return;
}

void Jafg::JChunkGenerationSubsystem::FixedTick(const f32 EngineDeltaTime, const f32 FixedDeltaTime)
{
    STAT_CYCLE_FUNCTION()

    Super::FixedTick(EngineDeltaTime, FixedDeltaTime);

    {
        std::unique_lock Lock(this->RequestedPreSpawnedChunksMutex);
        for (const LChunkKey& ChunkKey : this->RequestedPreSpawnedChunks)
        {
            if (this->LoadedChunks->contains(ChunkKey) == false)
            {
                this->SafeLoadPersistentPreSpawnedChunk(ChunkKey);
            }
        }
        this->RequestedPreSpawnedChunks.Empty();
    }

    {
        AChunk* Chunk = nullptr;
        while (this->InFailedActiveChunks.Dequeue(&Chunk))
        {
            check( Chunk )
            /* Just retry. */
            this->OutActiveChunks.Enqueue(Chunk);
        }
    }

    {
        i32 GeneratedChunks = 0;
        while (this->RequestedChunks.IsEmpty() == false && GeneratedChunks < 20)
        {
            const LChunkKey& Key = *this->RequestedChunks.GetLast();

            AChunk* Chunk = this->FindChunk(Key);
            if (Chunk)
            {
                Chunk->SetChunkPersistency(EChunkPersistency::Persistent);
            }
            else
            {
                Chunk = this->SafeLoadPersistentPreSpawnedChunk(Key);
                ++GeneratedChunks;
            }
            check( Chunk )

            if (Chunk->GetCurrentChunkStateDangerous() != EChunkState::Active)
            {
                this->OutActiveChunks.Enqueue(Chunk);
            }

            this->RequestedChunks.Pop();

            continue;
        }
    }

    return;
}

void Jafg::JChunkGenerationSubsystem::TearDown()
{
    STAT_CYCLE_FUNCTION()

    Super::TearDown();

    check( Tasks::IsOnMasterThread() )

    this->LoadedChunksMutex.lock();
    check( static_cast<bool>(this->LoadedChunks) )

    for (const std::pair<const LChunkKey&, AChunk*> Pair : this->LoadedChunks.value())
    {
        Pair.second->KillYourSelfNow();
    }

    this->LoadedChunks.reset();
    GEngine->RemoveShader(this->SharedChunkArgs.ChunkShaderHandle, false);

    return;
}

bool Jafg::JChunkGenerationSubsystem::LineTraceByChannel(
    TArray<LHitResult>& OutHits,
    const LVector& Start,
    const LVector& End,
    const LCollisionQueryParams& Params
) const
{
    STAT_CYCLE_FUNCTION()

    constexpr f32 Step { 0.5f };

    LChunkKey Key{Start};

    LVector Cursor   = Start;
    f32 Distance = (End - Start).Magnitude();
    const LVector Normal = (End - Start).GetUnsafeNormalized();
    const LVector NormalStep = Normal * Step;

    LChunkKey Last;

    std::shared_lock Lock(this->LoadedChunksMutex);
    LHitResult Dummy;
    while (Distance > 0)
    {
        const LChunkKey Current{Key};
        if (Current == Last)
        {
            Distance -= Step;
            Cursor += NormalStep;
            continue;
        }
        Last = Current;

        if
        (
            const std::unordered_map<LChunkKey, AChunk*>::const_iterator Element = this->LoadedChunks->find(Current);
            Element != this->LoadedChunks->end()
        )
        {
            if (Element->second->GetPhysicsComponent()->Sweep(Start, End, Dummy))
            {
                OutHits.Add(Dummy);
                Dummy.Reset();

                if (Params.bSingleHit)
                {
                    break;
                }
            }
        }

        Distance -= Step;
        Cursor += NormalStep;

        continue;
    }

    return OutHits.IsEmpty() == false;
}

void Jafg::JChunkGenerationSubsystem::OnStaticDraw(const LViewport& Viewport, const LEye& Eye, const std::span<LVector>& Corners) const
{
    for (const std::pair<const LChunkKey&, AChunk*> Pair : this->LoadedChunks.value())
    {
        check( Pair.second->IsGarbage() == false )

        if
        (
               Pair.second->IsRendererComponentValid()
            && Pair.second->GetRendererComponent()->Cull(Corners) == false
        )
        {
            Pair.second->GetRendererComponent()->Draw(Viewport, Eye);
        }
    }

    return;
}

Jafg::AChunk* Jafg::JChunkGenerationSubsystem::SpawnWeakChunk(const LChunkKey& InChunkKey)
{
    AChunk* Chunk = CheckedStaticCast<AChunk>(Private::LWorldMiscellaneousAccessor::SpawnActorWeak(this->GetWorld(), AChunk::StaticClass()));
    Chunk->SetSharedArgs(&this->SharedChunkArgs);
    MakeDeferredActorFinal(Chunk);
    Chunk->OnAlloc(InChunkKey);
    return Chunk;
}

Jafg::AChunk* Jafg::JChunkGenerationSubsystem::SafeLoadPersistentPreSpawnedChunk(const LChunkKey& ChunkKey)
{
    STAT_CYCLE_FUNCTION()

    AChunk* Chunk = this->SpawnWeakChunk(ChunkKey);

    std::unique_lock Lock(this->LoadedChunksMutex);
    check( this->LoadedChunks->contains(ChunkKey) == false )
    this->LoadedChunks->emplace(ChunkKey, Chunk);

    return Chunk;
}
