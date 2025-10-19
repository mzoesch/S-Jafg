// Copyright mzoesch. All rights reserved.

#include "MyWorld/Validation/ChunkValidationSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Framework/Pawn.h"
#include "MyWorld/Validation/ChunkValidationUtility.h"
#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "Stats/Stats.h"
#include "User/UserPreferences.h"

bool Jafg::JChunkValidationSubsystem::ShouldCreateSubsystem(LClassOuter const* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return Super::IsOuterWorld(Outer);
}

void Jafg::JChunkValidationSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Collection.InitializeDependency<JChunkGenerationSubsystem>();
    Super::Initialize(Collection);
    this->SetTickInterval(0.5f);

    return;
}

void Jafg::JChunkValidationSubsystem::FixedTick(const f32 EngineDeltaTime, const f32 FixedDeltaTime)
{
    STAT_CYCLE_FUNCTION()

    Super::FixedTick(EngineDeltaTime, FixedDeltaTime);

    bool bVerifyChunks = true;
    this->GetEngine().GetCommandLineInterface().GetVariableChecked("VerifyChunks")->GetValue<bool>(&bVerifyChunks);
    if (bVerifyChunks == false)
    {
        return;
    }

    check( this->GetWorld() )

    if (true) //(this->GetWorld()->IsLocalPawnValid() == false)
    {
        LOG_WARNING(LogChunkValidation, "Local pawn is invalid. Nothing to do.")
        return;
    }

    const LVector   Translation;// = this->GetWorld()->GetLocalPawn()->GetTranslation();
    const LChunkKey CurrentKey  = LChunkKey(Translation);

    if (CurrentKey == this->LastChunkKey)
    {
        return;
    }

    LOG_TRACE(LogChunkValidation, "Local pawn moved to chunk [{}].", CurrentKey.ToString())
    this->LastChunkKey = CurrentKey;

    const JUserPreferences* Prefs = GetDefault<JUserPreferences>();
    this->GetWorld()->GetSubsystemChecked<JChunkGenerationSubsystem>()->SetRequestedChunks(Validation::GetAllChunksFromCenterAsBox(
        CurrentKey,
        Prefs->ChunkRenderDistance,
        Prefs->ChunkRenderHeight,
        CurrentKey.Z - (Prefs->ChunkRenderHeight / 2)
        ));

    return;
}
