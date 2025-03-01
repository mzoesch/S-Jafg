// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/CappedTickableWorldSubsystem.h"
#include "MyWorld/Chunk/Chunk.h"
#include "ChunkGenerationSubsystem.generated.h"

namespace Jafg
{

class LChunkShaderContext;

//#
//# Loads and unloads chunks into / from the world based on what the current validation subsystem has determined.
//#
DECLARE_JAFG_CLASS()
class JChunkGenerationSubsystem final : public JCappedTickableWorldSubsystem
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JChunkGenerationSubsystem)

    // JTickableWorldSubsystem implementation
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void FixedTick(const float EngineDeltaTime, const float SubsystemDeltaTime) override;
    virtual void TearDown() override;
    // ~JTickableWorldSubsystem implementation

public:

    FORCEINLINE auto HasChunkShaderContext() const -> bool { return this->ChunkShaderContext != nullptr; }
    FORCEINLINE auto GetChunkShaderContext() const -> LChunkShaderContext* { return this->ChunkShaderContext; }

    FORCEINLINE auto GetCurrentActiveChunkSnapshot() const -> TdhArray<LChunkKey>;
    FORCEINLINE auto GetPanickedChunk(const LChunkKey& InChunkKey) const -> AChunk*;
    FORCEINLINE auto FindLoadedChunkOrNull(const LChunkKey& ChunkKey) const -> AChunk*;

    FORCEINLINE int32 GetRenderDistance() const { return *this->RenderDistance; }
    FORCEINLINE int32 GetRenderHeight() const { return *this->RenderHeight; }

    FORCEINLINE auto GetOptimalVerticalChunkQueue() -> TQueue<LChunkKey2>& { return this->OptimalVerticalChunkQueue; }

    FORCEINLINE auto AcquireVipChunksToLoad() -> TQueue<LChunkKey>&;
    FORCEINLINE void ReleaseVipChunksToLoad() { this->VipChunksToLoadMutex.unlock(); }

private:

    AChunk* SpawnChunk(const LChunkKey& InChunkKey) const;

    //#
    //# Transient or persistent chunks that are loaded in any state.
    //# @remark std::unordered_map is not trivially copyable when empty. So we have to use a pointer.
    //#         We should really implement our own hash map.
    //#
    std::unordered_map<LChunkKey, AChunk*>* LoadedChunks = nullptr;
    mutable std::shared_mutex LoadedChunksMutex;

    //#
    //# Very important persistent chunks to load to the world.
    //#
    TQueue<LChunkKey> VipChunksToLoad;
    std::mutex VipChunksToLoadMutex;
    void DequeueVipChunks();

    //#
    //# Based on the current validation subsystem.
    //# If the pawns do not move, these would be the remaining chunks that should be loaded.
    //#
    TQueue<LChunkKey2> OptimalVerticalChunkQueue;
    bool DequeueNextOptimalVerticalChunk();

    void SafeLoadPersistentChunkPreSpawnedChunk(const LChunkKey& ChunkKey);

    LSharedChunkArgs*    SharedChunkArgs    = nullptr;
    LChunkShaderContext* ChunkShaderContext = nullptr;
    const int32* RenderDistance = nullptr;
    const int32* RenderHeight   = nullptr;
};

TdhArray<LChunkKey> JChunkGenerationSubsystem::GetCurrentActiveChunkSnapshot() const
{
    std::shared_lock Lock(this->LoadedChunksMutex);
    TdhArray<LChunkKey> Out;
    for (const auto& [Fst, Snd] : *this->LoadedChunks)
    {
        if (Snd->GetChunkState() == EChunkState::Active)
        {
            Out.Add(Fst);
        }
    }
    return Out;
}

AChunk* JChunkGenerationSubsystem::GetPanickedChunk(const LChunkKey& InChunkKey) const
{
    if (AChunk* Chunk = this->FindLoadedChunkOrNull(InChunkKey); Chunk != nullptr)
    {
        return Chunk;
    }
    panicMsgf( "Chunk {} not found.", InChunkKey.ToString() )
    return nullptr;
}

AChunk* JChunkGenerationSubsystem::FindLoadedChunkOrNull(const LChunkKey& ChunkKey) const
{
    std::shared_lock Lock(this->LoadedChunksMutex);
    const std::unordered_map<LChunkKey, AChunk*>::iterator It = this->LoadedChunks->find(ChunkKey);
    return It == this->LoadedChunks->end() ? nullptr : It->second;
}

FORCEINLINE TQueue<LChunkKey>& JChunkGenerationSubsystem::AcquireVipChunksToLoad()
{
    this->VipChunksToLoadMutex.lock();
    return this->VipChunksToLoad;
}

} /* ~Namespace Jafg */
