// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Subsystems/WorldSubsystem.h"
#include "MyWorld/Chunk.h"
#include <queue>
#include "glm/glm.hpp"
#include <unordered_map>
#include "Engine/World.h"

namespace Jafg
{

DECLARE_JAFG_CLASS()
class JChunkGenerationSubsystem final : public Jafg::JTickableWorldSubsystem
{
    GENERATED_CLASS_BODY()

public:

    explicit JChunkGenerationSubsystem(Jafg::LWorld* InOuter) : Jafg::JTickableWorldSubsystem(InOuter) { }

    // WorldSubsystem implementation
    virtual void Initialize(Jafg::LSubsystemCollection& Collection) override;
    virtual void Tick(const float DeltaTime) override;
    virtual void TearDown() override;
    // ~WorldSubsystem implementation

    FORCEINLINE int32 TupleToKey(const std::tuple<int32, int32, int32>& Tuple) const
    {
        return std::get<0>(Tuple) + std::get<1>(Tuple) * ChunkSize + std::get<2>(Tuple) * ChunkSize * ChunkSize;
    }

private:

    void UpdateChunkQueue();
    void KillChunks();
    void GenerateChunks();
    void RenderChunks();

    std::unordered_map<int32, AChunk*> Chunks;
    std::queue<glm::vec3> ChunkQueue;
    int RenderDistance = 1;
    int RenderHeight = 0;
    unsigned int ChunkSize = 32;
    int LastCamX = -100, LastCamY = -100;
};

} /* ~Namespace Jafg */
