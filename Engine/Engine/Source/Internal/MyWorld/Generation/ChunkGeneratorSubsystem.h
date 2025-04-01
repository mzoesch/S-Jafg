// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/ThreadedWorldSubsystem.h"
#include "MyWorld/ChunkKey.h"
#include "MyWorld/Chunk/ChunkStates.h"
#if PLATFORM_SUPPORTS_SIMD
    #include "FastNoise/FastNoise.h"
#endif /* PLATFORM_SUPPORTS_SIMD */
#include "ChunkGeneratorSubsystem.generated.h"

namespace Jafg
{

class AChunk;
class JChunkGenerationSubsystem;
class JChunkValidationSubsystem;

//#
//# The chunk generator system. This system is responsible for generating the content of all chunks.
//#
DECLARE_JAFG_CLASS()
class JChunkGeneratorSubsystem final : public JThreadedWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JChunkGeneratorSubsystem)

    virtual void OnInitialize(LSubsystemCollection& Collection) override;
    virtual void FixedTick(const float RunnableDeltaTime) override;

private:

    void TryToActivateVerticalChunk(const LChunkKey2& ChunkKey, std::set<LChunkKey>* Visited);
    bool TryToBringChunkToState(AChunk* Target, const EChunkState::Type TargetState, std::set<LChunkKey>* Visited);

    bool PrepareWorldForChunkTransit_Spawned(const LChunkKey& InChunkKey);
    bool PrepareWorldForChunkTransit_Shaped(const LChunkKey& InChunkKey);
    bool PrepareWorldForChunkTransit_SurfaceReplaced(const LChunkKey& InChunkKey, std::set<LChunkKey>* Visited);

    JChunkValidationSubsystem* ChunkValidationSubsystem = nullptr;
    JChunkGenerationSubsystem* ChunkGenerationSubsystem = nullptr;

    TArray<LChunkKey> VipChunksToLoad;

public:

#if PLATFORM_SUPPORTS_SIMD
    FastNoise::SmartNode<> FnGenerator;
#endif /* PLATFORM_SUPPORTS_SIMD */
};

} /* ~Namespace Jafg */
