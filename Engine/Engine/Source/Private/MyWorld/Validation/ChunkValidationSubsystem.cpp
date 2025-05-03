// Copyright mzoesch. All rights reserved.

#include "MyWorld/Validation/ChunkValidationSubsystem.h"
#include "Engine/Engine.h"
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

void Jafg::JChunkValidationSubsystem::FixedTick(const float EngineDeltaTime, const f32 FixedDeltaTime)
{
    Super::FixedTick(EngineDeltaTime, FixedDeltaTime);

    bool bVerifyChunks = true;
    this->GetEngine()->GetCommandLineInterface()->GetVariableChecked("vc")->GetValue<bool>(&bVerifyChunks);
    if (bVerifyChunks == false)
    {
        return;
    }

    const LVector   Translation = this->GetWorld()->GetLocalPawn()->GetTranslation();
    const LChunkKey CurrentKey  = LChunkKey(Translation);

    if (CurrentKey == this->LastChunkKey)
    {
        return;
    }

    LOG_TRACE(LogChunkValidation, "Local pawn moved to chunk: {}.", CurrentKey.ToString())
    this->LastChunkKey = CurrentKey;

    const i32 RHeight = this->ChunkGenerationSubsystem->GetRenderHeight();
    TArray<LChunkKey> TargetChunks = Validation::GetAllChunksFromCenterAsBox(
        CurrentKey,
        this->ChunkGenerationSubsystem->GetRenderDistance(),
        RHeight,
        CurrentKey.Z - (RHeight / 2)
        );

    TArray<LChunkKey> Reversed; Reversed.Reserve(TargetChunks.GetSize());
    for (i32 i = TargetChunks.GetSize() - 1; i >= 0; --i)
    {
        Reversed.Emplace(TargetChunks[i]);
    }

    this->ChunkGenerationSubsystem->SetRequestedChunks(std::move(Reversed));

    return;
}
