// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/CappedTickableWorldSubsystem.h"
#include "MyWorld/ChunkKey.h"
#include "ChunkValidationSubsystem.generated.h"

namespace Jafg
{


class JChunkGenerationSubsystem;

DECLARE_JAFG_CLASS()
class JChunkValidationSubsystem final : public JCappedTickableWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JChunkValidationSubsystem)

    // JCappedTickableWorldSubsystem implementation
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void FixedTick(const float EngineDeltaTime, const float SubsystemDeltaTime) override;
    virtual void TearDown() override;
    // ~JCappedTickableWorldSubsystem implementation

public:

    TdhArray<LChunkKey2> CopyVerticalChunksInQuestion() const;

private:

    std::mutex* VerticalChunksInQuestionMutex = nullptr;
    TdhArray<LChunkKey2> VerticalChunksInQuestion;
    JChunkGenerationSubsystem* ChunkGenerationSubsystem = nullptr;
    LChunkKey LastChunkKey = { std::numeric_limits<LChunkKeyDomainTy>::max() };
};

} /* ~Namespace Jafg */
