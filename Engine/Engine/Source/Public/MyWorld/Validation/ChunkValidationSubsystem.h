// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/FixedTickableWorldSubsystem.h"
#include "MyWorld/ChunkKey.h"
#include "ChunkValidationSubsystem.generated.h"

namespace Jafg
{

class JChunkGenerationSubsystem;

DECLARE_JAFG_CLASS()
class JChunkValidationSubsystem final : public JFixedTickableWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JChunkValidationSubsystem)

    // JFixedTickableWorldSubsystem implementation
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void FixedTick(const float EngineDeltaTime, const float FixedDeltaTime) override;
    // ~JFixedTickableWorldSubsystem implementation

public:

    TArray<LChunkKey2> CopyVerticalChunksInQuestion() const;

private:

    LChunkKey LastChunkKey = { std::numeric_limits<LChunkKeyDomainTy>::max() };
    TArray<LChunkKey2> VerticalChunksInQuestion;
    JChunkGenerationSubsystem* ChunkGenerationSubsystem = nullptr;
    mutable std::shared_mutex VerticalChunksInQuestionMutex;
};

} /* ~Namespace Jafg */
