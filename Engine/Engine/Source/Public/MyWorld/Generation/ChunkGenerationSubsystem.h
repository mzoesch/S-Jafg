// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/CappedTickableWorldSubsystem.h"
#include "MyWorld/Chunk/Chunk.h"
#include <queue>
#include "glm/glm.hpp"
#include <unordered_map>
#include "ChunkGenerationSubsystem.generated.h"
#include "MyWorld/Chunk/ChunkPersistency.h"

namespace Jafg
{

class LChunkShaderContext;

DECLARE_JAFG_CLASS()
class JChunkGenerationSubsystem final : public JCappedTickableWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JChunkGenerationSubsystem)

    // JTickableWorldSubsystem implementation
    virtual void Initialize(Jafg::LSubsystemCollection& Collection) override;
    virtual void FixedTick(const float EngineDeltaTime, const float SubsystemDeltaTime) override;
    virtual void TearDown() override;
    // ~JTickableWorldSubsystem implementation

    FORCEINLINE int32 TupleToKey(const std::tuple<int32, int32, int32>& Tuple) const
    {
        return std::get<0>(Tuple) + std::get<1>(Tuple) * ChunkSize + std::get<2>(Tuple) * ChunkSize * ChunkSize;
    }

public:

    FORCEINLINE auto HasChunkShaderContext() const -> bool { return this->ChunkShaderContext != nullptr; }
    FORCEINLINE auto GetChunkShaderContext() const -> LChunkShaderContext* { return this->ChunkShaderContext; }

    FORCEINLINE auto GetChunks() const -> const std::unordered_map<LChunkKey, AChunk*>& { return this->Chunks; }
    FORCEINLINE auto GetCurrentActiveChunkSnapshot() const -> TdhArray<LChunkKey>
    {
        TdhArray<LChunkKey> Out;
        for (const auto& [Fst, Snd] : this->Chunks)
        {
            if (Snd->GetChunkState() == EChunkState::Active)
            {
                Out.Add(Fst);
            }
        }
        return Out;
    }
    FORCEINLINE auto GetPanickedChunk(const LChunkKey& InChunkKey) -> AChunk*
    {
        if (AChunk** Chunk = this->FindChunkOrNull(InChunkKey); Chunk != nullptr)
        {
            return *Chunk;
        }
        panicMsgf( "Chunk {} not found.", InChunkKey.ToString() )
        return nullptr;
    }

private:

    void UpdateChunkQueue();
    void KillChunks();
    void GenerateChunks();

    void SafeLoadChunk(
        const LChunkKey& ChunkKey,
        const EChunkPersistency::Type Persistency = EChunkPersistency::Persistent,
        const float TimeToLive = 20.0f,
        const EChunkState::Type TargetState = EChunkState::Active
    );

    AChunk* SpawnChunk(const LChunkKey& InChunkKey) const;

    std::unordered_map<LChunkKey, AChunk*> Chunks;
    std::queue<glm::vec3> ChunkQueue;
    int RenderDistance = 1;
    int RenderHeight = 0;
    unsigned int ChunkSize = 32;
    int LastCamX = -100, LastCamY = -100;

    LSharedChunkArgs*    SharedChunkArgs    = nullptr;
    LChunkShaderContext* ChunkShaderContext = nullptr;

    AChunk** FindChunkOrNull(const LChunkKey& ChunkKey);

    void PrepareWorldForChunkTransit_Spawned(const LChunkKey& InChunkKey);
    void PrepareWorldForChunkTransit_Shaped(const LChunkKey& InChunkKey);
    void PrepareWorldForChunkTransit_SurfaceReplaced(const LChunkKey& InChunkKey);
};

} /* ~Namespace Jafg */
