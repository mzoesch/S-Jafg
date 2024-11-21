// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "MyWorld/Validation/ChunkValidationSubsystem.h"
#include "Engine/World.h"
#include "Engine/Framework/Pawn.h"
#include "MyWorld/Validation/ChunkValidationUtility.h"
#include "MyWorld/Generation/ChunkGenerationSubsystem.h"

void Jafg::JChunkValidationSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Collection.InitializeDependency<JChunkGenerationSubsystem>();
    Super::Initialize(Collection);
    this->SetTickInterval(0.5f);

    this->VerticalChunksInQuestionMutex = new std::mutex();
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

    TdhArray<LChunkKey2> NowVerticalChunksInQuestion;
    Validation::GetAllChunksFromCenterAsBox(CurrentKey.XY(), this->ChunkGenerationSubsystem->GetRenderDistance(), NowVerticalChunksInQuestion);

    TQueue<LChunkKey2>& OptimalQueue = this->ChunkGenerationSubsystem->GetOptimalVerticalChunkQueue();
    for (const LChunkKey2& Chunk : NowVerticalChunksInQuestion)
    {
        OptimalQueue.Enqueue(Chunk);
    }

    this->VerticalChunksInQuestionMutex->lock();
    this->VerticalChunksInQuestion.SwapBuffers(NowVerticalChunksInQuestion);
    this->VerticalChunksInQuestionMutex->unlock();

    return;
}

void Jafg::JChunkValidationSubsystem::TearDown()
{
    Super::TearDown();
}

Jafg::TdhArray<Jafg::LChunkKey2> Jafg::JChunkValidationSubsystem::CopyVerticalChunksInQuestion() const
{
    checkSlow( this->VerticalChunksInQuestionMutex )

    std::lock_guard<std::mutex> Lock(*this->VerticalChunksInQuestionMutex);
    TdhArray<LChunkKey2> Copy = this->VerticalChunksInQuestion;
    return Copy;
}
