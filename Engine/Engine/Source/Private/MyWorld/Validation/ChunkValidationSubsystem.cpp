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

    TdhArray<LChunkKey2> VerticalChunksInQuestion;
    Validation::GetAllChunksFromCenterAsBox(CurrentKey.XY(), this->ChunkGenerationSubsystem->GetRenderDistance(), VerticalChunksInQuestion);

    TQueue<LChunkKey2>& OptimalQueue = this->ChunkGenerationSubsystem->GetOptimalVerticalChunkQueue();
    for (const LChunkKey2& Chunk : VerticalChunksInQuestion)
    {
        OptimalQueue.Enqueue(Chunk);
    }

    return;
}

void Jafg::JChunkValidationSubsystem::TearDown()
{
    Super::TearDown();
}
