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
    #define STAT_CHUNK_VERBOSE_OUT          IN_DEBUG
#endif /* STAT_CHUNK_VERBOSE_OUT */

#if WITH_STATS
    #if STAT_CHUNK_VERBOSE_OUT
        #define STAT_QUICK_CYCLE_START_KEY(InChunkKey)                             \
            ::LStringView _sv { LAL_PRETTY_FUNCTION };                             \
            ::LString _s { ::Lal::SprintF("{}::{}", _sv, InChunkKey.ToString()) }; \
            STAT_QUICK_CYCLE_START(LStringView(_s.begin(), _s.end()))
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

    while (this->IsStopped() == false)
    {
        AChunk* Chunk { nullptr };

        while (this->IsStopped() == false && this->ChunkGenerationSubsystem->OutActiveChunks.Dequeue(&Chunk))
        {
            check( Chunk )
            this->MakeChunkActive(Chunk);
            continue;
        }

        if (this->IsStopped() == false)
        {
            Hal::Sleep(this->YieldTime);
        }

        continue;
    }

    return ETaskExit::Success;
}

void Jafg::LChunkGeneratorWorker::MakeChunkActive(AChunk* Target)
{
    STAT_QUICK_CYCLE_START_KEY(Target->GetChunkKey())

    if (Target->GetCurrentStateDangerous() < EChunkState::Shaped)
    {
        Target->SetState(EChunkState::Shaped, false, 0.5f, this->GetStoppedPointer(), nullptr);
    }

    if (Target->GetCurrentStateDangerous() < EChunkState::SurfaceReplaced)
    {
        Target->SetState(EChunkState::SurfaceReplaced, false, 0.5f, this->GetStoppedPointer(), nullptr);
    }

    if (Target->GetCurrentStateDangerous() < EChunkState::Active)
    {
        Target->SetState(EChunkState::Active, true, 0.5f, this->GetStoppedPointer(), nullptr);
    }

    return;
}

Jafg::JChunkGeneratorSubsystem::JChunkGeneratorSubsystem(LCxxObjectInitializer const& CxxObjectInitializer)
    : Super(CxxObjectInitializer)
{
    this->SetPriorityTearDown(true);
    return;
}

bool Jafg::JChunkGeneratorSubsystem::ShouldCreateSubsystem(LClassOuter const* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return Super::IsOuterWorld(Outer);
}

void Jafg::JChunkGeneratorSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

#if LAL_PLATFORM_SUPPORTS_SIMD
    FastNoise::SmartNode<FastNoise::Perlin> Root = FastNoise::New<FastNoise::Perlin>();
    this->FnGenerator = std::move(Root);
    LOG_INFO(LogChunkGeneration, "Using SIMD: [{}].", static_cast<FastSIMD::Level_BitFlags>(this->FnGenerator->GetSIMDLevel()));
#endif /* PLATFORM_SUPPORTS_SIMD */

    this->ChunkGenerationSubsystem = Collection.GetSubsystemChecked<JChunkGenerationSubsystem>();

    for (i32 i = 0; i < 1 /* 10 */; ++i)
    {
        ETaskExit::Type Exit;
        const ENamedThreads::Type WorkerName = Tasks::LaunchNamedThread<LChunkGeneratorWorker>
        (
            &Exit,
            Lal::SprintF("WkrCg_{}", i),
            this->ChunkGenerationSubsystem
        );

        if (Exit != ETaskExit::Success)
        {
            LOG_ERROR(LogChunkGeneration, "Failed to launch chunk generator worker [{}].", i)
            break;
        }

        this->Workers.emplace_back(WorkerName);
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

    algo::orphan(&this->Workers);

    return;
}
