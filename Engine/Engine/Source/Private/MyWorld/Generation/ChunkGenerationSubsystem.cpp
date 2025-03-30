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
    this->GetWorld()->OnStaticDraw.BindMember(this, &JChunkGenerationSubsystem::LOnStaticDraw);

    return;
}

void Jafg::JChunkGenerationSubsystem::FixedTick(const float EngineDeltaTime, const float SubsystemDeltaTime)
{
    STAT_CYCLE_FUNCTION()

    Super::FixedTick(EngineDeltaTime, SubsystemDeltaTime);

    this->DequeueVipChunks();

    i32 GeneratedChunks = 0;
    while (this->OptimalVerticalChunkQueue.IsEmpty() == false && GeneratedChunks < 5)
    {
        if (this->DequeueNextOptimalVerticalChunk())
        {
            ++GeneratedChunks;
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
    this->VipChunksToLoadMutex.lock();
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
    TdhArray<LHitResult>& OutHits,
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

void Jafg::JChunkGenerationSubsystem::LOnStaticDraw(const LViewport& Viewport, const LEye& Eye, const std::span<LVector>& Corners) const
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

Jafg::AChunk* Jafg::JChunkGenerationSubsystem::SpawnChunk(const LChunkKey& InChunkKey)
{
    AChunk* Chunk = CheckedStaticCast<AChunk>(Private::LWorldMiscellaneousAccessor::SpawnActorWeak(this->GetWorld(), AChunk::StaticClass()));
    Chunk->SetSharedArgs(&this->SharedChunkArgs);
    MakeDeferredActorFinal(Chunk);
    Chunk->OnAlloc(InChunkKey);
    return Chunk;
}

void Jafg::JChunkGenerationSubsystem::DequeueVipChunks()
{
    this->VipChunksToLoadMutex.lock();
    LChunkKey VipChunkKey;
    while (this->VipChunksToLoad.Dequeue(VipChunkKey))
    {
        this->SafeLoadPersistentChunkPreSpawnedChunk(VipChunkKey);
    }
    this->VipChunksToLoadMutex.unlock();

    return;
}

bool Jafg::JChunkGenerationSubsystem::DequeueNextOptimalVerticalChunk()
{
    LChunkKey2 OptimalVerticalChunk;
    if (this->OptimalVerticalChunkQueue.Dequeue(OptimalVerticalChunk) == false)
    {
        checkNoEntry()
        return false;
    }

    bool Ret = false;
    for (i32 Z = 0; Z <= this->GetRenderHeight(); ++Z)
    {
        AChunk* Chunk;
        if (Chunk = this->FindLoadedChunkOrNull(LChunkKey(OptimalVerticalChunk, Z)); Chunk == nullptr)
        {
            this->SafeLoadPersistentChunkPreSpawnedChunk(LChunkKey(OptimalVerticalChunk, Z));
            Ret = true;
            continue;
        }

        if (Chunk->GetHuntedChunkState() != EChunkState::Active)
        {
            this->SafeLoadPersistentChunkPreSpawnedChunk(LChunkKey(OptimalVerticalChunk, Z));
            Ret = true;
            continue;
        }

        continue;
    }

    return Ret;
}

void Jafg::JChunkGenerationSubsystem::SafeLoadPersistentChunkPreSpawnedChunk(const LChunkKey& ChunkKey)
{
    std::unique_lock Lock(this->LoadedChunksMutex); // Move this one statement down?
    AChunk* Chunk = this->SpawnChunk(ChunkKey);
    this->LoadedChunks->emplace(ChunkKey, Chunk);

    return;
}
