// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "ChunkGeneratorSubsystem.generated.h"
#include "FastNoise/FastNoise.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class JChunkGeneratorSubsystem final : public JWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JChunkGeneratorSubsystem)

    virtual void Initialize(LSubsystemCollection& Collection) override;

public:

    float* UsableContainer = nullptr;
    FastNoise::SmartNode<> FnGenerator;
};

} /* ~Namespace Jafg */
