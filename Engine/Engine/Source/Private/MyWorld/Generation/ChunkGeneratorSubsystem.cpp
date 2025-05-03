// Copyright mzoesch. All rights reserved.

#include "MyWorld/Generation/ChunkGeneratorSubsystem.h"
#include "Core/Application.h"
#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "MyWorld/Validation/ChunkValidationSubsystem.h"
#include "Stats/Stats.h"

static constexpr f32 DeltaTime = 0.2f;

void Jafg::JChunkGeneratorSubsystem::OnInitialize(LSubsystemCollection& Collection)
{
    Super::OnInitialize(Collection);
    this->SetTickInterval(::DeltaTime);

    this->HumanReadableName = "JChunkGeneratorSubsystem";

#if PLATFORM_SUPPORTS_SIMD
    FastNoise::SmartNode<FastNoise::Perlin> Root = FastNoise::New<FastNoise::Perlin>();
    this->FnGenerator = std::move(Root);
    LOG_INFO(LogChunkGeneration, "Using SIMD: [{}].", static_cast<FastSIMD::Level_BitFlags>(this->FnGenerator->GetSIMDLevel()));
#endif /* PLATFORM_SUPPORTS_SIMD */

    this->ChunkValidationSubsystem = Collection.GetCheckedSubsystem<JChunkValidationSubsystem>();
    this->ChunkGenerationSubsystem = Collection.GetCheckedSubsystem<JChunkGenerationSubsystem>();

    return;
}

void Jafg::JChunkGeneratorSubsystem::FixedTick(const float RunnableDeltaTime)
{
    Super::FixedTick(RunnableDeltaTime);
    checkSlow( Tasks::IsOnMasterThread() == false )

    Application::LHrcTimePoint Start = Application::GetHighestNow();

    // Chunks that have been visited this tick.
    std::set<LChunkKey> Visited;

    // Chunks that are missing and are needed to be loaded by the master thread.
    std::set<LChunkKey> Missing;

    // The chunks that have been requested by the validation subsystem to be loaded AND activated.
    TArray<LChunkKey> Requested = this->ChunkGenerationSubsystem->GetRequestedChunksSnapshot();

    for (i32 i = Requested.GetSize() - 1; i >= 0; --i)
    {
        if (this->ShouldTickRunnable() == false)
        {
            LOG_VERBOSE(LogRunnable, "Interrupted.")
            break;
        }

        const LChunkKey& ChunkKey = Requested[i];

        if (AChunk* Chunk = this->ChunkGenerationSubsystem->FindChunk(ChunkKey))
        {
            this->TryToBringChunkToState(ChunkKey, Chunk, true, EChunkState::Active, &Missing, &Visited);
        }

        if (Application::GetTimeDiff(Start, Application::GetHighestNow()) > ::DeltaTime * 2.0f)
        {
            LOG_VERBOSE(LogRunnable, "Chunk generation took too long. Stopping.")
            break;
        }

        continue;
    }

    this->ChunkGenerationSubsystem->MutateRequestedPreSpawnedChunks([&Missing](TArray<LChunkKey>& RequestedPreSpawnedChunks)
    {
        for (const LChunkKey& ChunkKey : Missing)
        {
            RequestedPreSpawnedChunks.Emplace(ChunkKey);
            continue;
        }

        return;
    });

    return;
}

bool Jafg::JChunkGeneratorSubsystem::TryToBringChunkToState
(
    const LChunkKey&        Key,
    AChunk*                 Target,
    const bool              bPersistent,
    const EChunkState::Type TargetState,
    std::set<LChunkKey>*    Missing,
    std::set<LChunkKey>*    Visited
)
{
#define GOTO_STATE(STATE)                                                               \
    if (TargetState < EChunkState::STATE)                                               \
    {                                                                                   \
        check( Target->GetChunkState() == static_cast<u8>(EChunkState::STATE) - 1 )     \
        return true;                                                                    \
    }                                                                                   \
    if (Target->GetChunkState() < EChunkState::STATE)                                   \
    {                                                                                   \
        if                                                                              \
        (                                                                               \
            this->PRIVATE_JAFG_CORE_JOIN_OUTER_TWO(PrepareWorldForChunkTransit_, STATE) \
            (Key, Missing, Visited) == false                                            \
        )                                                                               \
        {                                                                               \
            return false;                                                               \
        }                                                                               \
        Target->SetChunkState(EChunkState::STATE);                                      \
    }

    STAT_CYCLE_FUNCTION()

    check( Missing && Visited )

    /* We can only generate chunks between those states. The other are special. */
    check( EChunkState::Freed < TargetState && TargetState < EChunkState::Special )

    if (Visited->contains(Key) == false)
    {
        Visited->insert(Key);
    }

    if (Target->GetChunkState() >= TargetState)
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

    return false;

#undef GOTO_STATE
}

bool Jafg::JChunkGeneratorSubsystem::PrepareWorldForChunkTransit_Spawned(const LChunkKey& InChunkKey, std::set<LChunkKey>* Missing, std::set<LChunkKey>* Visited)
{
    STAT_CYCLE_FUNCTION()

    bool bRet = true;

    for (const LChunkKey& Neighbor : InChunkKey.GetNeighboringChunkKeys())
    {
        if (this->ChunkGenerationSubsystem->FindChunk(Neighbor) == nullptr)
        {
            Missing->insert(Neighbor);
            bRet = false;
        }

        continue;
    }

    return bRet;
}

bool Jafg::JChunkGeneratorSubsystem::PrepareWorldForChunkTransit_SurfaceReplaced(const LChunkKey& InChunkKey, std::set<LChunkKey>* Missing, std::set<LChunkKey>* Visited)
{
    STAT_CYCLE_FUNCTION()

    bool bRet = true;
    for (const LChunkKey& NeighborKey : InChunkKey.GetNeighboringChunkKeys())
    {
        if (AChunk* Neighbor = this->ChunkGenerationSubsystem->FindChunk(NeighborKey); Neighbor)
        {
            checkSlow( Neighbor->GetChunkKey() == NeighborKey )
            if (this->TryToBringChunkToState(NeighborKey, Neighbor, false, EChunkState::Shaped, Missing, Visited) == false)
            {
                bRet = false;
            }
        }
        else
        {
            Missing->insert(NeighborKey);
            bRet = false;
        }

        continue;
    }

    return bRet;
}
