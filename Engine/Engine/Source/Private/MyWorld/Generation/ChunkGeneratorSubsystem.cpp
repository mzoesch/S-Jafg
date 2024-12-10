// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "ChunkGeneratorSubsystem.h"
#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "MyWorld/Validation/ChunkValidationSubsystem.h"

void Jafg::JChunkGeneratorSubsystem::OnInitialize(LSubsystemCollection& Collection)
{
    Super::OnInitialize(Collection);
    this->SetTickInterval(0.1f);

#if PLATFORM_SUPPORTS_SIMD
    this->FnGenerator = FastNoise::NewFromEncodedNodeTree("DQAFAAAAAAAAQAgAAAAAAD8AAAAAAA==");
#endif /* PLATFORM_SUPPORTS_SIMD */

    this->ChunkValidationSubsystem = Collection.GetCheckedSubsystem<JChunkValidationSubsystem>();
    this->ChunkGenerationSubsystem = Collection.GetCheckedSubsystem<JChunkGenerationSubsystem>();

    return;
}

void Jafg::JChunkGeneratorSubsystem::FixedTick(const float RunnableDeltaTime)
{
    Super::FixedTick(RunnableDeltaTime);
    checkSlow( Tasks::IsOnMasterThread() == false )

    this->VipChunksToLoad.Empty();

    std::set<LChunkKey> Visited;
    TdhArray<LChunkKey2> OptimalVerticalChunks = this->ChunkValidationSubsystem->CopyVerticalChunksInQuestion();
    for (const LChunkKey2& ChunkKey : OptimalVerticalChunks)
    {
        this->TryToActivateVerticalChunk(ChunkKey, &Visited);

        if (this->ShouldTickRunnable() == false)
        {
            LOG_VERBOSE(LogRunnable, "Interrupted.")
            break;
        }
    }

    TQueue<LChunkKey>& VipQueue = this->ChunkGenerationSubsystem->AcquireVipChunksToLoad();
    VipQueue.Empty();
    for (const LChunkKey& ChunkKey : this->VipChunksToLoad) { VipQueue.Enqueue(ChunkKey); }
    this->ChunkGenerationSubsystem->ReleaseVipChunksToLoad();

    return;
}

void Jafg::JChunkGeneratorSubsystem::TryToActivateVerticalChunk(const LChunkKey2& ChunkKey, std::set<LChunkKey>* Visited)
{
    for (int32 Z = 0; Z < this->ChunkGenerationSubsystem->GetRenderHeight(); ++Z)
    {
        AChunk* Target;
        if (Target = this->ChunkGenerationSubsystem->FindLoadedChunkOrNull(LChunkKey(ChunkKey, Z)); Target == nullptr)
        {
            continue;
        }

        check( Target->GetChunkKey() == LChunkKey(ChunkKey, Z) )

        if (Target->GetChunkState() == EChunkState::Active || Target->GetHuntedChunkState() == EChunkState::Active)
        {
            continue;
        }

        this->TryToBringChunkToState(Target, EChunkState::Active, Visited);

        continue;
    }

    return;
}

bool Jafg::JChunkGeneratorSubsystem::TryToBringChunkToState(AChunk* Target, const EChunkState::Type TargetState, std::set<LChunkKey>* Visited)
{
    /* We can only generate chunks between those states. The other are special. */
    check( EChunkState::Freed < TargetState && TargetState < EChunkState::Special )

    const LChunkKey ChunkKey = Target->GetChunkKey();

    if (Visited->contains(ChunkKey))
    {
        return Target->GetChunkState() >= TargetState;
    }
    Visited->insert(ChunkKey);

    if (Target->GetChunkState() >= TargetState)
    {
        return true;
    }

    Target->SetChunkPersistency(EChunkPersistency::Persistent);

    if (TargetState < EChunkState::Spawned) { check( EChunkState::PreSpawned) return true; }
    if (Target->GetChunkState() < EChunkState::Spawned)
    {
        if (this->PrepareWorldForChunkTransit_Spawned(ChunkKey) == false)
        {
            return false;
        }
        Target->SetChunkState(EChunkState::Spawned);
    }

    if (TargetState < EChunkState::Shaped) { check( EChunkState::Spawned) return true; }
    if (Target->GetChunkState() < EChunkState::Shaped)
    {
        if (this->PrepareWorldForChunkTransit_Shaped(ChunkKey) == false)
        {
            return false;
        }
        Target->SetChunkState(EChunkState::Shaped);
    }

    if (TargetState < EChunkState::SurfaceReplaced) { check( EChunkState::Shaped) return true; }
    if (Target->GetChunkState() < EChunkState::SurfaceReplaced)
    {
        if (this->PrepareWorldForChunkTransit_SurfaceReplaced(ChunkKey, Visited) == false)
        {
            return false;
        }
        Target->SetChunkState(EChunkState::SurfaceReplaced);
    }

    if (TargetState < EChunkState::Active) { check( EChunkState::SurfaceReplaced) return true; }
    if (Target->GetHuntedChunkState() == EChunkState::Active)
    {
        return true;
    }
    Target->SetHuntedChunkState(TargetState);
    checkSlow( Target->GetHuntedChunkState() == EChunkState::Active )

    // Regenerate mesh here and then just apply on master thread.

    check( Target->GetChunkState() != EChunkState::Active )
    Tasks::Make(ENamedThreads::Master, ETaskTime::Whenever, [Target] (void)
    {
        if (Target->GetWorld()->GetWorldState() > EWorldState::Running)
        {
            return;
        }

        checkSlow( Target->GetHuntedChunkState() == EChunkState::Active )
        if (Target->GetChunkState() != EChunkState::Active)
        {
            Target->SetChunkState(EChunkState::Active);
        }
        else
        {
            LOG_WARNING(LogRunnable, "Chunk {} already active.", Target->GetChunkKey().ToString())
        }

        return;
    });

    return false;
}

bool Jafg::JChunkGeneratorSubsystem::PrepareWorldForChunkTransit_Spawned(const LChunkKey& InChunkKey)
{
    bool bRet = true;
    for (const LChunkKey& Neighbor : InChunkKey.GetNeighboringChunkKeys())
    {
        if (this->ChunkGenerationSubsystem->FindLoadedChunkOrNull(Neighbor) == nullptr)
        {
            this->VipChunksToLoad.Add(Neighbor);
            bRet = false;
        }
    }

    return bRet;
}

bool Jafg::JChunkGeneratorSubsystem::PrepareWorldForChunkTransit_Shaped(const LChunkKey& InChunkKey)
{
    /* Shaping currently does not require any additional states more than spawned. */
    return true;
}

bool Jafg::JChunkGeneratorSubsystem::PrepareWorldForChunkTransit_SurfaceReplaced(const LChunkKey& InChunkKey, std::set<LChunkKey>* Visited)
{
    bool bRet = true;
    for (const LChunkKey& NeighborKey : InChunkKey.GetNeighboringChunkKeys())
    {
        AChunk* Neighbor;
        if (Neighbor = this->ChunkGenerationSubsystem->FindLoadedChunkOrNull(NeighborKey); Neighbor == nullptr)
        {
            this->VipChunksToLoad.Add(NeighborKey);
            bRet = false;
            continue;
        }

        if (Neighbor->GetChunkState() < EChunkState::Shaped)
        {
            if (this->TryToBringChunkToState(Neighbor, EChunkState::Shaped, Visited) == false)
            {
                bRet = false;
            }
        }

        continue;
    }

    return bRet;
}
