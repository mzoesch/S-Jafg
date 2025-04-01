// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "MyWorld/Validation/ChunkValidationSubsystem.h"
#include "Engine/World.h"
#include "Framework/Pawn.h"
#include "MyWorld/Validation/ChunkValidationUtility.h"
#include "MyWorld/Generation/ChunkGenerationSubsystem.h"

void Jafg::JChunkValidationSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Collection.InitializeDependency<JChunkGenerationSubsystem>();
    Super::Initialize(Collection);
    this->SetTickInterval(0.5f);

    this->ChunkGenerationSubsystem = Collection.GetCheckedSubsystem<JChunkGenerationSubsystem>();

    return;
}

void Jafg::JChunkValidationSubsystem::FixedTick(const float EngineDeltaTime, const float SubsystemDeltaTime)
{
    Super::FixedTick(EngineDeltaTime, SubsystemDeltaTime);

    const LVector   Translation = this->GetWorld()->GetLocalPawn()->GetTranslation();
    const LChunkKey CurrentKey  = LChunkKey(Translation);

    if (CurrentKey.Equals(this->LastChunkKey, LChunkKey::XYx))
    {
        return;
    }

    LOG_TRACE(LogChunkValidation, "Local pawn moved to chunk: {}.", CurrentKey.ToString())
    this->LastChunkKey = CurrentKey;

    TArray<LChunkKey2> NowVerticalChunksInQuestion;
    Validation::GetAllChunksFromCenterAsBox(CurrentKey.XY(), this->ChunkGenerationSubsystem->GetRenderDistance(), NowVerticalChunksInQuestion);

    TQueue<LChunkKey2>& OptimalQueue = this->ChunkGenerationSubsystem->GetOptimalVerticalChunkQueue();
    OptimalQueue.Empty();
    for (const LChunkKey2& Chunk : NowVerticalChunksInQuestion)
    {
        OptimalQueue.Enqueue(Chunk);
    }

    std::unique_lock Lock(this->VerticalChunksInQuestionMutex);
    this->VerticalChunksInQuestion.SwapBuffers(NowVerticalChunksInQuestion);

    return;
}

Jafg::TArray<Jafg::LChunkKey2> Jafg::JChunkValidationSubsystem::CopyVerticalChunksInQuestion() const
{
    std::shared_lock Lock(this->VerticalChunksInQuestionMutex);
    TArray<LChunkKey2> Copy = this->VerticalChunksInQuestion;
    return Copy;
}
