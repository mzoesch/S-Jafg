// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "MyWorld/ChunkKey.h"
#include "MyWorld/Chunk/ChunkStates.h"
#if LAL_PLATFORM_SUPPORTS_SIMD
    #include "FastNoise/FastNoise.h"
#endif /* PLATFORM_SUPPORTS_SIMD */
#include "ChunkGeneratorSubsystem.generated.h"

namespace Jafg
{

class AChunk;
class LChunkGeneratorWorker;
class JChunkGenerationSubsystem;
class JChunkValidationSubsystem;

class LChunkGeneratorWorker : public LRunnable
{
public:

    explicit LChunkGeneratorWorker(const LString&& InHumanReadableName, JChunkGenerationSubsystem* InChunkGenerationSubsystem);

    virtual ETaskExit::Type Run() override;

private:

    void MakeChunkActive(AChunk* Target);

    f32 YieldTime { 0.5f };
    JChunkGenerationSubsystem* ChunkGenerationSubsystem { nullptr };
};

//#
//# The chunk generator sub system. This system is responsible for generating the content of all chunks.
//#
DECLARE_JAFG_CLASS()
class JChunkGeneratorSubsystem final : public JWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    explicit JChunkGeneratorSubsystem(const LObjectInitializer& ObjectInitializer);

    virtual bool ShouldCreateSubsystem(const LObjectContext* InOuter) const override;
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void TearDown() override;

private:

    JChunkGenerationSubsystem* ChunkGenerationSubsystem = nullptr;

    TArray<ENamedThreads::Type> Workers;

public:

    FORCEINLINE const FastNoise::SmartNode<>& GetFastNoiseGenerator() const noexcept { return this->FnGenerator; }

private:

#if LAL_PLATFORM_SUPPORTS_SIMD
    //# READ ONLY after #Initialize was run. Do not modify this object ever.
    FastNoise::SmartNode<> FnGenerator;
#endif /* PLATFORM_SUPPORTS_SIMD */
};

} /* ~Namespace Jafg */
