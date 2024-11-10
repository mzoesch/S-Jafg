// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Actor.h"
#include "MyWorld/Chunk/ChunkStates.h"
#include "MyWorld/MyWorldStatics.h"
#include "Engine/Components/RenderComponent.h"
#include "MyWorld/ChunkKey.h"
#include <glm/glm.hpp>
#include "RhiFramework/Shader.h"
#include "ChunkPersistency.h"
#include "RhiFramework/ChunkShaderContext.h"
#include "MyWorld/CommonTypes.h"
#include "MyWorld/VoxelKey.h"
#include "Chunk.generated.h"

namespace Jafg
{

class LChunkMesher;
class LFastChunkMesher;
class AChunk;
class JChunkGenerationSubsystem;
class LChunkMesher;

MAKE_MULTICAST_SIGNATURE(LChunkStateChangedDelegateSignature, EChunkState::Type /* NewChunkState */)

class ENGINE_API LChunkRendererComponent final : public LRendererComponent
{
public:

    LChunkRendererComponent() = delete;
    explicit LChunkRendererComponent(AChunk& Owner);
    ~LChunkRendererComponent() override = default;

    virtual void Draw(const LViewport& Context) override;

    FORCEINLINE auto GetOwner()       ->       AChunk& { return *this->Owner; }
    FORCEINLINE auto GetOwner() const -> const AChunk& { return *this->Owner; }
    FORCEINLINE auto GetShaderInstance()       ->       LChunkShaderInstance* { return &this->Instance; }
    FORCEINLINE auto GetShaderInstance() const -> const LChunkShaderInstance* { return &this->Instance; }

private:

    AChunk*              Owner    = nullptr;
    LChunkShaderInstance Instance = { };
};

struct LSharedChunkArgs final
{
    JChunkGenerationSubsystem* ChunkGenerationSubsystem;
    TFunction<LChunkMesher*(AChunk& Owner)> GetNewMesher;
};

DECLARE_JAFG_CLASS()
class ENGINE_API AChunk final : public AActor
{
    GENERATED_CLASS_BODY()

public:

    /** The index in the raw voxel data array. */
    typedef int32 LVoxelIndex;

    static_assert(
        MwStatics::ChunkSize < std::numeric_limits<LVoxelKeyDomainTy>::max() - 1,
        "Encountered to large chunk size for indexing the raw voxels domains "
        "- this will result in undefined overflow behavior."
    );

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(AChunk)

public:

    virtual void BeginLife() override;
    virtual void EndLife() override;

    FORCEINLINE void SetChunkState(const EChunkState::Type NewChunkState);
    FORCEINLINE auto GetChunkState() const -> EChunkState::Type { return this->ChunkState; }

    void OnAlloc(const LChunkKey& InChunkKey);

    void SetChunkPersistency(const EChunkPersistency::Type NewPersistency, const float TimeToLive = 10.0f);
    FORCEINLINE auto GetChunkPersistency() const -> EChunkPersistency::Type { return this->ChunkPersistency; }
    FORCEINLINE auto IsPersistent() const -> bool { return this->ChunkPersistency == EChunkPersistency::Persistent; }
    FORCEINLINE auto IsTransient() const -> bool { return this->ChunkPersistency == EChunkPersistency::Transient;  }
    FORCEINLINE bool ShouldBeFreed() const
    {
        return this->IsTransient()
            &&   this->RealTimeInSecondsWhenTransientChunkShouldBeKilled
               < this->GetWorld()->GetRealTimeSecondsSinceWorldLaunch();
    }

private:

    /**
     * The real time (not stopped or dilated / clamped) when this chunk should be killed by the generation subsystem.
     * The time is relative to the time when the world was launched where this AActor lives in.
     * Only meaningful when the persistency of this chunk is transient.
     */
    float RealTimeInSecondsWhenTransientChunkShouldBeKilled = 0.0f;
    EChunkPersistency::Type ChunkPersistency = EChunkPersistency::Persistent;

    bool IsStateChangeValid(const EChunkState::Type NewChunkState) const;
    EChunkState::Type ChunkState = EChunkState::Invalid;

    void OnSpawned();
    void Shape();
    void ReplaceSurface();
    void OnActive();

public:

    //////////////////////////////////////////////////////////////////////////
    // Raw Data
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE bool HasRawVoxelData() const { return this->RawVoxelData; }
    FORCEINLINE static LVoxelIndex GetRawVoxelIndex(const LVoxelKey InKey)
    {
        return InKey.X + InKey.Y * MwStatics::ChunkSize + InKey.Z * MwStatics::ChunkSizeSquared;
    }
    FORCEINLINE static LVoxelIndex GetRawVoxelIndex(const LVoxelKeyDomainTy InX, const LVoxelKeyDomainTy InY, const LVoxelKeyDomainTy InZ)
    {
        return InX + InY * MwStatics::ChunkSize + InZ * MwStatics::ChunkSizeSquared;
    }
    FORCEINLINE static LVoxelIndex GetRawVoxelIndex(const int32 InX, const int32 InY, const int32 InZ)
    {
        return InX + InY * MwStatics::ChunkSize + InZ * MwStatics::ChunkSizeSquared;
    }

    FORCEINLINE voxel_t GetRawVoxelData(const LVoxelKey InKey) const
    {
        checkSlow( this->HasRawVoxelData() )
        return this->RawVoxelData[AChunk::GetRawVoxelIndex(InKey)];
    }
    FORCEINLINE voxel_t GetRawVoxelData(const LVoxelKeyDomainTy InX, const LVoxelKeyDomainTy InY, const LVoxelKeyDomainTy InZ) const
    {
        checkSlow( this->HasRawVoxelData() )
        return this->RawVoxelData[AChunk::GetRawVoxelIndex(InX, InY, InZ)];
    }
    FORCEINLINE voxel_t GetRawVoxelData(const int32 InX, const int32 InY, const int32 InZ) const
    {
        checkSlow( this->HasRawVoxelData() )
        return this->RawVoxelData[AChunk::GetRawVoxelIndex(InX, InY, InZ)];
    }

    FORCEINLINE voxel_t GetSafeRawVoxelData(
        const LVoxelKey InKey, const voxel_t InFallback = ECompileTimeVoxels::Air
    ) const
    {
        checkSlow( this->HasRawVoxelData() )
        if (InKey.IsLocal())
        {
            return this->GetRawVoxelData(InKey);
        }
        return InFallback;
    }
    FORCEINLINE voxel_t GetSafeRawVoxelData(
        const LVoxelKeyDomainTy InX, const LVoxelKeyDomainTy InY, const LVoxelKeyDomainTy InZ, const voxel_t InFallback = ECompileTimeVoxels::Air
    ) const
    {
        checkSlow( this->HasRawVoxelData() )
        if (LVoxelKey(InX, InY, InZ).IsLocal())
        {
            return this->GetRawVoxelData(InX, InY, InZ);
        }
        return InFallback;
    }
    FORCEINLINE voxel_t GetSafeRawVoxelData(
        const int32 InX, const int32 InY, const int32 InZ, const voxel_t InFallback = ECompileTimeVoxels::Air
    ) const
    {
        checkSlow( this->HasRawVoxelData() )
        if (LVoxelKey(InX, InY, InZ).IsLocal())
        {
            return this->GetRawVoxelData(InX, InY, InZ);
        }
        return InFallback;
    }

    FORCEINLINE voxel_t GetRawVoxelDataByNonZeroOrigin(LVoxelKey InKey) const
    {
        return this->GetCheckedNeighboringChunk(&InKey)->GetRawVoxelData(InKey);
    }
    FORCEINLINE voxel_t GetRawVoxelDataByNonZeroOrigin(LVoxelKey InKey, const voxel_t Fallback) const
    {
#if DO_CHECKS
        const LVoxelKey In = InKey;
#endif /* DO_CHECKS */
        if (const AChunk* Target = this->GetNeighboringChunk(&InKey); Target)
        {
#if DO_CHECKS
            if (Target == this) { check( InKey == In ) }
            else { check( InKey != In && InKey.IsLocal() ) }
#endif /* DO_CHECKS */
            return Target->GetRawVoxelData(InKey);
        }

        return Fallback;
    }

    voxel_t* RawVoxelData = nullptr;
    glm::vec3 ChunkPos = glm::vec3(0.0f);

    LSharedChunkArgs* SharedArgs = nullptr;
    LChunkKey  ChunkKey      = { };
    LVector    WorldLocation = { };

    FORCEINLINE auto GetChunkRendererComponent() -> LChunkRendererComponent*
    {
        return reinterpret_cast<LChunkRendererComponent*>(this->GetRendererComponent());
    }
    FORCEINLINE auto GetChunkRendererComponent() const -> const LChunkRendererComponent*
    {
        return reinterpret_cast<LChunkRendererComponent*>(this->GetRendererComponent());
    }

    FORCEINLINE auto GetMesher()       ->       LChunkMesher* { return this->Mesher; }
    FORCEINLINE auto GetMesher() const -> const LChunkMesher* { return this->Mesher; }

private:

    LChunkMesher* Mesher = nullptr;

#pragma region Neighbors

    //////////////////////////////////////////////////////////////////////////
    // Neighbors
    //////////////////////////////////////////////////////////////////////////

public:

    FORCEINLINE auto HasNNorth() const -> bool { return this->NNorth != nullptr; }
    FORCEINLINE auto GetNNorth() const -> AChunk* { return this->NNorth; }
    FORCEINLINE auto HasNEast() const -> bool { return this->NEast != nullptr; }
    FORCEINLINE auto GetNEast() const -> AChunk* { return this->NEast; }
    FORCEINLINE auto HasNSouth() const -> bool { return this->NSouth != nullptr; }
    FORCEINLINE auto GetNSouth() const -> AChunk* { return this->NSouth; }
    FORCEINLINE auto HasNWest() const -> bool { return this->NWest != nullptr; }
    FORCEINLINE auto GetNWest() const -> AChunk* { return this->NWest; }
    FORCEINLINE auto HasNUp() const -> bool { return this->NUp != nullptr; }
    FORCEINLINE auto GetNUp() const -> AChunk* { return this->NUp; }
    FORCEINLINE auto HasNDown() const -> bool { return this->NDown != nullptr; }
    FORCEINLINE auto GetNDown() const -> AChunk* { return this->NDown; }

    FORCEINLINE auto GetNeighboringChunk(LVoxelKey* InOutKey) const -> const AChunk*;
    FORCEINLINE auto GetCheckedNeighboringChunk(LVoxelKey* InOutKey) const -> const AChunk*;
    FORCEINLINE auto GetPanickedNeighboringChunk(LVoxelKey* InOutKey) const -> const AChunk*;

private:

    AChunk* NNorth = nullptr;
    AChunk* NEast  = nullptr;
    AChunk* NSouth = nullptr;
    AChunk* NWest  = nullptr;
    AChunk* NUp    = nullptr;
    AChunk* NDown  = nullptr;

#pragma endregion Neighbors
};

FORCEINLINE const AChunk* AChunk::GetNeighboringChunk(LVoxelKey* InOutKey) const
{
    switch (InOutKey->NormalizeKeyForNeighbor())
    {
    case EVoxelKeyLocation::Local: { return this; }
    case EVoxelKeyLocation::North: { return this->NNorth; }
    case EVoxelKeyLocation::East:  { return this->NEast;  }
    case EVoxelKeyLocation::South: { return this->NSouth; }
    case EVoxelKeyLocation::West:  { return this->NWest;  }
    case EVoxelKeyLocation::Up:    { return this->NUp;    }
    case EVoxelKeyLocation::Down:  { return this->NDown;  }
    default: { checkNoEntry(); return nullptr; }
    }
}

FORCEINLINE const AChunk* AChunk::GetCheckedNeighboringChunk(LVoxelKey* InOutKey) const
{
    if (const AChunk* Target = this->GetNeighboringChunk(InOutKey); Target)
    {
        return Target;
    }
    checkNoEntry()
    return nullptr;
}

FORCEINLINE const AChunk* AChunk::GetPanickedNeighboringChunk(LVoxelKey* InOutKey) const
{
    if (const AChunk* Target = this->GetNeighboringChunk(InOutKey); Target)
    {
        return Target;
    }
    panic( "Failed to find target chunk by local voxel key." )
    return nullptr;
}

} /* ~Namespace Jafg */
