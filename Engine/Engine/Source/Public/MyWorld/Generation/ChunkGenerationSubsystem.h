// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/CappedTickableWorldSubsystem.h"
#include "MyWorld/Chunk/Chunk.h"
#include <queue>
#include "glm/glm.hpp"
#include <unordered_map>
#include "MyWorld/Chunk/ChunkPersistency.h"
#include "ChunkGenerationSubsystem.generated.h"

namespace Jafg
{

class LChunkShaderContext;

/* Just temp. This is not a solution, but we first have to implement this on our own -
 * just to satisfy the j-object requirements. */
inline std::unordered_map<LChunkKey, AChunk*> CgsChunks = { };
inline std::queue<glm::vec3> ChunkQueue = { };

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

public:

    FORCEINLINE auto HasChunkShaderContext() const -> bool { return this->ChunkShaderContext != nullptr; }
    FORCEINLINE auto GetChunkShaderContext() const -> LChunkShaderContext* { return this->ChunkShaderContext; }

    FORCEINLINE auto GetChunks() const -> const std::unordered_map<LChunkKey, AChunk*>& { return CgsChunks; }
    FORCEINLINE auto GetCurrentActiveChunkSnapshot() const -> TdhArray<LChunkKey>
    {
        TdhArray<LChunkKey> Out;
        for (const auto& [Fst, Snd] : CgsChunks)
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

    FORCEINLINE auto GetRenderDistance() const -> int { return this->RenderDistance; }

    FORCEINLINE auto GetOptimalVerticalChunkQueue() -> TQueue<LChunkKey2>& { return this->OptimalVerticalChunkQueue; }

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

    /**
     * Transient or persistent chunks that are loaded in any state.
     * @remark std::unordered_map is not trivially copyable when empty. So we have to use a pointer.
     *         We should really implement our own hash map.
     */
    std::unordered_map<LChunkKey, AChunk*>* LoadedChunks = nullptr;

    /**
     * Based on the current validation subsystem.
     * If the pawns do not move these would be the remaining chunks that should be loaded.
     */
    TQueue<LChunkKey2> OptimalVerticalChunkQueue;

    int RenderDistance = 0;
    int RenderHeight = 0; // Move this to usr pref.
    int LastCamX = -100, LastCamY = -100;

    LSharedChunkArgs*    SharedChunkArgs    = nullptr;
    LChunkShaderContext* ChunkShaderContext = nullptr;

    AChunk** FindChunkOrNull(const LChunkKey& ChunkKey);

    void PrepareWorldForChunkTransit_Spawned(const LChunkKey& InChunkKey);
    void PrepareWorldForChunkTransit_Shaped(const LChunkKey& InChunkKey);
    void PrepareWorldForChunkTransit_SurfaceReplaced(const LChunkKey& InChunkKey);
};

} /* ~Namespace Jafg */
