// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Subsystems/WorldSubsystem.h"
#include "MyWorld/Chunk.h"
#include <queue>
#include "glm/glm.hpp"
#include <unordered_map>

class JChunkGenerationSubsystem final : public JTickableWorldSubsystem
{
public:

    // WorldSubsystem implementation
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void Tick(const float DeltaTime) override;
    virtual void TearDown() override;
    // ~WorldSubsystem implementation

    FORCEINLINE int32 TupleToKey(const std::tuple<int32, int32, int32>& Tuple) const
    {
        return std::get<0>(Tuple) + std::get<1>(Tuple) * ChunkSize + std::get<2>(Tuple) * ChunkSize * ChunkSize;
    }

private:

    uint32 NumChunks = 0, NumChunksRendered = 0;
    std::unordered_map<int32, Chunk*> Chunks;
    std::queue<glm::vec3> ChunkQueue;
    int RenderDistance = 10;
    int RenderHeight = 1;
    unsigned int ChunkSize = 32;
    unsigned int ChunksLoading = 0;
    int LastCamX = -100, LastCamY = -100, LastCamZ = -100;
};
