// Copyright mzoesch. All rights reserved.

#pragma once

#include "Subsystems/FixedTickableWorldSubsystem.h"
#include "MyWorld/Chunk/Chunk.h"
#include "ChunkGenerationSubsystem.generated.h"

namespace Jafg
{

//#
//# Loads and unloads chunks into / from the world based on what the current validation subsystem has determined.
//#
DECLARE_JAFG_CLASS()
class JChunkGenerationSubsystem final : public JFixedTickableWorldSubsystem
{
    GENERATED_CLASS_BODY()

public:

    typedef std::unordered_map<LChunkKey, AChunk*> LChunkMap;
    typedef std::optional<LChunkMap>               LLoadedChunks;

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JChunkGenerationSubsystem)

    // JFixedTickableWorldSubsystem implementation
    virtual void Initialize(LSubsystemCollection& Collection) override;
    virtual void FixedTick(const f32 EngineDeltaTime, const f32 FixedDeltaTime) override;
    virtual void TearDown() override;
    // ~JFixedTickableWorldSubsystem implementation

public:

    //# Threadsafe but is heavy. Do not use in high proximity.
    FORCEINLINE TArray<LChunkKey> GetCurrentActiveChunkSnapshot() const;
    FORCEINLINE TArray<LChunkKey> GetRequestedChunksSnapshot() const;

    //# Threadsafe.
    FORCEINLINE AChunk* FindChunk(const LChunkKey& ChunkKey) const;
    FORCEINLINE AChunk* FindChunkChecked(const LChunkKey& InChunkKey) const;
    FORCEINLINE AChunk* FindChunkAsserted(const LChunkKey& InChunkKey) const;

    FORCEINLINE i32 GetRenderDistance() const { return *this->RenderDistance; }
    FORCEINLINE i32 GetRenderHeight() const { return *this->RenderHeight; }

    //# Not thread safe. Only use on master thread.
    FORCEINLINE void SetRequestedChunks(TArray<LChunkKey>&& InChunks);
    FORCEINLINE auto GetRequestedChunks() const -> const TArray<LChunkKey>& { check( Tasks::IsOnMasterThread() ) return this->RequestedChunks; }

    template <typename Predicate>
    FORCEINLINE void MutateRequestedPreSpawnedChunks(Predicate&& InPredicate);

private:

    //
    // BEGIN
    // World optimization stuff not part of the chunk generation subsystem and logic.
    // Just helper functions for really heavy optimizations.
    //
    bool LineTraceByChannel(
        TArray<LHitResult>& OutHits,
        const LVector& Start,
        const LVector& End,
        const LCollisionQueryParams& Params
    ) const;

    void OnStaticDraw(
        const LViewport& Viewport,
        const LEye& Eye,
        const std::span<LVector>& Corners
    ) const;
    //
    // END World optimization stuff
    //

    AChunk* SpawnWeakChunk(const LChunkKey& InChunkKey);
    void SafeLoadPersistentPreSpawnedChunk(const LChunkKey& ChunkKey);

    //#
    //# Transient or persistent chunks that are loaded in any state.
    //# @remark std::unordered_map is not trivially copyable when empty. So we have to use an optional.
    //#         We should really implement our own hash map.
    //#
    LLoadedChunks LoadedChunks;
    mutable std::shared_mutex LoadedChunksMutex;

    //#
    //# Based on the current validation subsystem.
    //# If points of interest do not move, these would be the remaining chunks that should be pre spawned and loaded.
    //#
    TArray<LChunkKey> RequestedChunks;
    //# Same as #RequestedChunks, but these are the chunks that have not yet been processed and loaded into the world.
    TArray<LChunkKey> RequestedRemainingChunks;
    mutable std::shared_mutex RequestedChunksMutex;

    //# Requested chunks that should be pre spawned immediately.
    TArray<LChunkKey> RequestedPreSpawnedChunks;
    mutable std::shared_mutex RequestedPreSpawnedChunksMutex;

    LSharedChunkArgs SharedChunkArgs;

    //# Cached args common attributes.
    const i32* RenderDistance = nullptr;
    const i32* RenderHeight   = nullptr;
};

template<typename Predicate>
FORCEINLINE void JChunkGenerationSubsystem::MutateRequestedPreSpawnedChunks(Predicate&& InPredicate)
{
    check( Tasks::IsOnMasterThread() == false && "No. This is bad design.")

    std::unique_lock Lock(this->RequestedPreSpawnedChunksMutex);
    InPredicate(this->RequestedPreSpawnedChunks);

    return;
}

FORCEINLINE TArray<LChunkKey> JChunkGenerationSubsystem::GetCurrentActiveChunkSnapshot() const
{
    check( Tasks::IsOnMasterThread() == false && "No. This is bad design.")

    std::shared_lock Lock(this->LoadedChunksMutex);
    TArray<LChunkKey> Out;
    for (const auto& [Fst, Snd] : *this->LoadedChunks)
    {
        if (Snd->GetChunkState() == EChunkState::Active)
        {
            Out.Add(Fst);
        }
    }
    return Out;
}

FORCEINLINE TArray<LChunkKey> JChunkGenerationSubsystem::GetRequestedChunksSnapshot() const
{
    check( Tasks::IsOnMasterThread() == false && "No. This is bad design.")
    std::shared_lock Lock(this->RequestedChunksMutex);
    return this->RequestedChunks;
}

FORCEINLINE AChunk* JChunkGenerationSubsystem::FindChunk(const LChunkKey& ChunkKey) const
{
    std::shared_lock Lock(this->LoadedChunksMutex);
    const std::unordered_map<LChunkKey, AChunk*>::const_iterator It = this->LoadedChunks->find(ChunkKey);
    return It == this->LoadedChunks->end() ? nullptr : It->second;
}

FORCEINLINE AChunk* JChunkGenerationSubsystem::FindChunkChecked(const LChunkKey& InChunkKey) const
{
    AChunk* Out = this->FindChunk(InChunkKey);
    check( Out )
    return Out;
}

FORCEINLINE AChunk* JChunkGenerationSubsystem::FindChunkAsserted(const LChunkKey& InChunkKey) const
{
    AChunk* Out = this->FindChunk(InChunkKey);
    jassert( Out )
    return Out;
}

FORCEINLINE void JChunkGenerationSubsystem::SetRequestedChunks(TArray<LChunkKey>&& InChunks)
{
    check( Tasks::IsOnMasterThread() )
    std::unique_lock Lock(this->RequestedChunksMutex);
    this->RequestedChunks = std::move(InChunks);
    this->RequestedRemainingChunks = this->RequestedChunks;

    return;
}

} /* ~Namespace Jafg */
