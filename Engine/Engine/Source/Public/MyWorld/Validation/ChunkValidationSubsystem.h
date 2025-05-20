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
    virtual bool ShouldCreateSubsystem(const LObjectContext* InOuter) const override;
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void FixedTick(const float EngineDeltaTime, const float FixedDeltaTime) override;
    // ~JFixedTickableWorldSubsystem implementation

private:

    LChunkKey LastChunkKey = { std::numeric_limits<LChunkKeyDomain>::max() };
    JChunkGenerationSubsystem* ChunkGenerationSubsystem = nullptr;
};

} /* ~Namespace Jafg */
