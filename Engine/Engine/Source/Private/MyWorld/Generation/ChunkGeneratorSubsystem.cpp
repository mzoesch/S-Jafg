// Copyright mzoesch. All rights reserved.

#include "MyWorld/Generation/ChunkGeneratorSubsystem.h"
#include "Core/Application.h"
#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "MyWorld/Validation/ChunkValidationSubsystem.h"
#include "Stats/Stats.h"

//#
//# Whether to use slow but verbose output for chunk generation while stating.
//#
#ifndef STAT_CHUNK_VERBOSE_OUT
    #define STAT_CHUNK_VERBOSE_OUT      IN_DEBUG
#endif /* STAT_CHUNK_VERBOSE_OUT */

#if WITH_STATS
    #if STAT_CHUNK_VERBOSE_OUT
        #define STAT_QUICK_CYCLE_START_KEY(InChunkKey)                                   \
            std::string_view _sv = JAFG_PRETTY_FUNCTION;                                 \
            ::Jafg::LString _s = LString::SprintF("{}::{}", _sv, InChunkKey.ToString()); \
            STAT_QUICK_CYCLE_START(std::string_view(_s.GetBegin(), _s.GetEnd()))
    #else /* STAT_CHUNK_VERBOSE_OUT */
        #define STAT_QUICK_CYCLE_START_KEY(InChunkKey)  STAT_CYCLE_FUNCTION()
    #endif /* !STAT_CHUNK_VERBOSE_OUT */
#else /* WITH_STATS */
    #define STAT_QUICK_CYCLE_START_KEY(InChunkKey)
#endif  /* !WITH_STATS */

Jafg::LChunkGeneratorWorker::LChunkGeneratorWorker(const LString&& InHumanReadableName, JChunkGenerationSubsystem* InChunkGenerationSubsystem)
{
    this->HumanReadableName = std::move(InHumanReadableName);
    this->ChunkGenerationSubsystem = InChunkGenerationSubsystem;

    check( this->ChunkGenerationSubsystem )

    return;
}

Jafg::ETaskExit::Type Jafg::LChunkGeneratorWorker::Run()
{
    STAT_CYCLE_FUNCTION()

    checkSlow( Tasks::IsOnMasterThread() == false )

    constexpr i32 MaxChunkCount { 5 };

    AChunk* Chunk;
    while (this->IsStopped() == false)
    {
        Chunk = nullptr;

        i32 Dequeued = 0;
        while (Dequeued < MaxChunkCount && this->ChunkGenerationSubsystem->OutActiveChunks.Dequeue(&Chunk))
        {
            ++Dequeued;

            check( Chunk )

            if (this->IsStopped())
            {
                break;
            }

            const bool bRetState = this->TryToBringChunkToState(Chunk, true, EChunkState::Active);

            this->ChunkGenerationSubsystem->MutateRequestedPreSpawnedChunks([this](TArray<LChunkKey>& RequestedPreSpawnedChunks)
            {
                for (const LChunkKey& ChunkKey : this->Missing)
                {
                    RequestedPreSpawnedChunks.Emplace(ChunkKey);
                    continue;
                }

                return;
            });
            this->Missing.clear();

            if (bRetState == false)
            {
                this->ChunkGenerationSubsystem->InFailedActiveChunks.Enqueue(Chunk);
            }

            continue;
        }

        if (this->IsStopped())
        {
            break;
        }

        if (Dequeued < MaxChunkCount)
        {
            PlatformHal::Sleep(this->YieldTime);
        }

        this->Visited.clear();

        continue;
    }

    return ETaskExit::Success;
}

bool Jafg::LChunkGeneratorWorker::TryToBringChunkToState(AChunk* Target, const bool bPersistent, const EChunkState::Type TargetState)
{
#define GOTO_STATE(STATE)                                                               \
    if (TargetState < EChunkState::STATE)                                               \
    {                                                                                   \
        /* check( Target->GetChunkState() == static_cast<u8>(EChunkState::STATE) - 1 )*/\
        return true;                                                                    \
    }                                                                                   \
    if (Target->SetHuntedState(EChunkState::STATE))                                     \
    {                                                                                   \
        if                                                                              \
        (                                                                               \
            this->PRIVATE_JAFG_CORE_JOIN_OUTER_TWO(PrepareWorldForChunkTransit_, STATE) \
            (Key) == false                                                              \
        )                                                                               \
        {                                                                               \
            Target->InvalidateHuntedState();                                            \
            return false;                                                               \
        }                                                                               \
        Target->SetState(EChunkState::STATE);                                           \
    }

    STAT_QUICK_CYCLE_START_KEY(Target->GetChunkKey())

    /* We can only generate chunks between those states. The other are special. */
    check( EChunkState::Freed < TargetState && TargetState < EChunkState::Special )

    const LChunkKey& Key = Target->GetChunkKey();

    if (this->Visited.contains(Key) == false)
    {
        this->Visited.insert(Key);
    }

    if (Target->GetCurrentChunkStateDangerous() >= TargetState)
    {
        return true;
    }

    if (bPersistent)
    {
        Target->SetChunkPersistency(EChunkPersistency::Persistent);
    }

    GOTO_STATE(Spawned)
    GOTO_STATE(Shaped)
    GOTO_STATE(SurfaceReplaced)
    GOTO_STATE(Active)

    return true;

#undef GOTO_STATE
}

bool Jafg::LChunkGeneratorWorker::PrepareWorldForChunkTransit_Spawned(const LChunkKey& InChunkKey)
{
    STAT_QUICK_CYCLE_START_KEY(InChunkKey)

    bool bRet = true;

    for (const LChunkKey& Neighbor : InChunkKey.GetNeighboringChunkKeys())
    {
        if (this->ChunkGenerationSubsystem->FindChunk(Neighbor) == nullptr)
        {
            this->Missing.insert(Neighbor);
            bRet = false;
        }

        continue;
    }

    return bRet;
}

bool Jafg::LChunkGeneratorWorker::PrepareWorldForChunkTransit_SurfaceReplaced(const LChunkKey& InChunkKey)
{
    STAT_QUICK_CYCLE_START_KEY(InChunkKey)

    bool bRet = true;
    for (const LChunkKey& NeighborKey : InChunkKey.GetNeighboringChunkKeys())
    {
        if (AChunk* Neighbor = this->ChunkGenerationSubsystem->FindChunk(NeighborKey); Neighbor)
        {
            checkSlow( Neighbor->GetChunkKey() == NeighborKey )
            if (this->TryToBringChunkToState(Neighbor, false, EChunkState::Shaped) == false)
            {
                bRet = false;
            }
        }
        else
        {
            this->Missing.insert(NeighborKey);
            bRet = false;
        }

        continue;
    }

    return bRet;
}

Jafg::JChunkGeneratorSubsystem::JChunkGeneratorSubsystem(const LObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
    this->SetPriorityTearDown(true);
    return;
}

void Jafg::JChunkGeneratorSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

#if PLATFORM_SUPPORTS_SIMD
    FastNoise::SmartNode<FastNoise::Perlin> Root = FastNoise::New<FastNoise::Perlin>();
    this->FnGenerator = std::move(Root);
    LOG_INFO(LogChunkGeneration, "Using SIMD: [{}].", static_cast<FastSIMD::Level_BitFlags>(this->FnGenerator->GetSIMDLevel()));
#endif /* PLATFORM_SUPPORTS_SIMD */

    this->ChunkGenerationSubsystem = Collection.GetCheckedSubsystem<JChunkGenerationSubsystem>();

    for (i32 i = 0; i < 10; ++i)
    {
        ETaskExit::Type Exit;
        const ENamedThreads::Type WorkerName = Tasks::LaunchNamedThread<LChunkGeneratorWorker>(&Exit,
            LString::SprintF("WkrCg_{}", i),
            this->ChunkGenerationSubsystem
        );

        if (Exit != ETaskExit::Success)
        {
            LOG_ERROR(LogChunkGeneration, "Failed to launch chunk generator worker [{}].", i)
            break;
        }

        this->Workers.Emplace(WorkerName);
    }

    return;
}

void Jafg::JChunkGeneratorSubsystem::TearDown()
{
    STAT_CYCLE_FUNCTION()

    Super::TearDown();

    for (const ENamedThreads::Type Worker : this->Workers)
    {
        Tasks::StopThread(Worker);
    }

    for (const ENamedThreads::Type Worker : this->Workers)
    {
        Tasks::JoinThread(Worker);
    }

    this->Workers.Empty();

    return;
}
