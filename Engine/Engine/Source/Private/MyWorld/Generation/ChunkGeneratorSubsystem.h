// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/ThreadedWorldSubsystem.h"
#include "FastNoise/FastNoise.h"
#include "ChunkGeneratorSubsystem.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class JChunkGeneratorSubsystem final : public JThreadedWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JChunkGeneratorSubsystem)

    virtual void OnInitialize(LSubsystemCollection& Collection) override;
    virtual void FixedTick(const float RunnableDeltaTime) override;

public:

    float* UsableContainer = nullptr;
    FastNoise::SmartNode<> FnGenerator;
};

} /* ~Namespace Jafg */
