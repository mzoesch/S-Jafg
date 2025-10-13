// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Actor.h"
#include "MyWorld/Chunk/ChunkStates.h"
#include "MyWorld/MyWorldStatics.h"
#include "Components/RenderComponent.h"
#include "MyWorld/ChunkKey.h"
#include "MyWorld/CommonTypes.h"
#include "MyWorld/VoxelKey.h"
#include "Rhi/ChunkShader.h"
#include "MyWorld/Meshing/ChunkMesher.h"
#include "Chunk.generated.h"

namespace Jafg
{

class JMaterialSubsystem;
class JVoxelSubsystem;
class LChunkMesher;
class JVoxelTextureSubsystem;
class LFastChunkMesher;
class AChunk;
class JChunkGenerationSubsystem;
class JChunkGeneratorSubsystem;
class LChunkShader;

MAKE_MULTICAST_SIGNATURE(LChunkStateChangedDelegateSignature, EChunkState::Type /* NewChunkState */)

namespace EChunkStateTimeoutBehavior
{

enum Type : u8
{
    //# Panic if timeout was reached.
    Panic,

    //# Ignore and give an invalid feedback.
    Ignore,
};

} /* ~Namespace ChunkStateTimeoutBehavior */

class LChunkRendererComponent final : public LRendererComponent
{
public:

    LChunkRendererComponent() = delete;
    FORCEINLINE explicit LChunkRendererComponent(AChunk& Owner) : Owner(Owner) { }
    ~LChunkRendererComponent() override = default;

    virtual bool Cull(const std::span<LVector>& Corners) const override;
    virtual void Draw(const LViewport& Context, const LEye& Eye) override;

    FORCEINLINE auto GetOwner()       noexcept ->       AChunk& { return this->Owner; }
    FORCEINLINE auto GetOwner() const noexcept -> const AChunk& { return this->Owner; }
    FORCEINLINE auto GetShaderInstance()       noexcept ->       LChunkShaderInstance* { return &this->Instance; }
    FORCEINLINE auto GetShaderInstance() const noexcept -> const LChunkShaderInstance* { return &this->Instance; }

private:

    AChunk& Owner;
    LChunkShaderInstance Instance;
};

//#
//# Defines shared arguments that are used by every chunk inside a context.
//#
struct LSharedChunkArgs final
{
    constexpr LSharedChunkArgs() noexcept = default;
    LSharedChunkArgs(LSharedChunkArgs const& Other) noexcept
        : ChunkGenerationSubsystem(Other.ChunkGenerationSubsystem)
        , ChunkGeneratorSubsystem(Other.ChunkGeneratorSubsystem)
        , VoxelSubsystem(Other.VoxelSubsystem)
        , MaterialSubsystem(Other.MaterialSubsystem)
        , VoxelTextureSubsystem(Other.VoxelTextureSubsystem)
        , GetNewMesher(Other.GetNewMesher)
        , bSuperFlat(Other.bSuperFlat)
    {
        check( Other.ChunkShader.IsValid() == false )
    }
    LSharedChunkArgs& operator=(LSharedChunkArgs const& Other) noexcept = delete;
    PROHIBIT_MOVE(LSharedChunkArgs)

    JChunkGenerationSubsystem* ChunkGenerationSubsystem { nullptr };
    JChunkGeneratorSubsystem*  ChunkGeneratorSubsystem  { nullptr };
    JVoxelSubsystem*           VoxelSubsystem           { nullptr };
    JMaterialSubsystem*        MaterialSubsystem        { nullptr };
    JVoxelTextureSubsystem*    VoxelTextureSubsystem    { nullptr };
    LChunkShader               ChunkShader;
    TFunction<TUnique<LChunkMesher>(AChunk& Owner)> GetNewMesher;

    bool bSuperFlat { false };
};

DECLARE_JAFG_CLASS()
class AChunk final : public AActor
{
    GENERATED_CLASS_BODY()

    friend JChunkGenerationSubsystem;

public:

    //# The index in the raw voxel data array.
    typedef i32 LVoxelIndex;

    static_assert
    (
        MwStatics::ChunkSize < std::numeric_limits<LVoxelKeyDomainTy>::max() - 1,
        "Encountered to large chunk size for indexing the raw voxels domains "
        "- this will result in undefined overflow behavior."
    );

protected:

    DEFAULT_OBJECT_CTOR(AChunk)
    //# When do we have reflection... :(
    explicit AChunk(AChunk const& CDR) noexcept
        : Super(CDR)
        , SharedArgs{CDR.SharedArgs}
    {
        check( CDR.State == EChunkState::Invalid )
        check( CDR.HuntedState == EChunkState::Invalid )
        check( this->SharedArgs )
        check( CDR.Mesher.get() == nullptr )
        check( CDR.RawVoxelData.get() == nullptr )
        check( !(CDR.NNorth || CDR.NEast || CDR.NSouth || CDR.NWest || CDR.NUp || CDR.NDown) )

        return;
    }

public:

    // AActor implementation
    virtual void BeginLife() override;
    virtual void EndLife() override;
    FORCEINLINE auto GetChunkRendererComponent() -> LChunkRendererComponent* { return reinterpret_cast<LChunkRendererComponent*>(this->GetRendererComponent()); }
    FORCEINLINE auto GetChunkRendererComponent() const -> const LChunkRendererComponent* { return reinterpret_cast<LChunkRendererComponent*>(this->GetRendererComponent()); }
    // ~AActor implementation

    //#
    //# Set the state of this chunk.
    //# @param NewChunkState The new state to set.
    //# @param bKeepHunt     If true, the chunk will be locked for other state changes and the function returns so
    //#                      that chunk state logic can be laid of (e.g., to another thread).
    //# @param WaitTime      The time to wait for checking the atomic boolean if bAbort is set.
    //# @param bAbort        If set, the chunk state change will be aborted if the atomic boolean is set to true.
    //# @param bOutChanged   If set, the function will return true if the state was changed, false otherwise.
    //#
    void SetState(const EChunkState::Type NewChunkState, const bool bKeepHunt = false, const f32 WaitTime = 0.5f, const std::atomic_bool* bAbort = nullptr, bool* bOutChanged = nullptr);
    //# Only on aggregating thread where the chunk is manipulated.
    EChunkState::Type GetLockedState() const;
    //# Dangerous function. Use with care.
    EChunkState::Type GetCurrentStateDangerous() const { return this->State; }
    EChunkState::Type GetCurrentHuntedStateDangerous() const { return this->HuntedState; }
    void FinishHunt(const EChunkState::Type InState) noexcept;

    FORCEINLINE const LChunkKey& GetChunkKey() const noexcept { return this->ChunkKey; }

    FORCEINLINE bool IsSharedArgsValid() const noexcept { return this->SharedArgs; }
    FORCEINLINE const LSharedChunkArgs* GetSharedArgs() const noexcept { return this->SharedArgs; }
    FORCEINLINE void SetSharedArgs(LSharedChunkArgs* NewSharedArgs);

    FORCEINLINE bool IsMesherValid() const noexcept { return this->Mesher != nullptr; }
    FORCEINLINE LChunkMesher* GetMesher() noexcept { return this->Mesher.get(); }
    FORCEINLINE const LChunkMesher* GetMesher() const noexcept { return this->Mesher.get(); }

    //#
    //# Create a relative voxel key from the world location of this chunk.
    //#
    FORCEINLINE LVoxelKey CreateRelativeVoxelKey(const LVector& InWorldLocation) const;

private:

    bool IsStateChangeValid(const EChunkState::Type NewChunkState) const;
    //#
    //# The current state of a chunk.
    //# @remark This variable is only meaningful if the #HuntedState is set to #EChunkState::Invalid.
    //#
    std::atomic<EChunkState::Type> State { EChunkState::Invalid };
    //#
    //# The hunted state of this chunk. This is the state that is currently being processed in any way. E.g. through
    //# other worker threads or networked requests.
    //# Invalid means that the current chunk is not hunting any other state.
    //#
    std::atomic<EChunkState::Type> HuntedState { EChunkState::Invalid };
    // For the #State and the #HuntedState variables.
    std::timed_mutex ChunkStateMutex;

    void Spawn();
    void Shape();
    void ReplaceSurface();
    void Activate();

    void RegenerateNeighboringMeshes() const;

    LChunkKey ChunkKey;
    LSharedChunkArgs* SharedArgs { nullptr };
    TUnique<LChunkMesher> Mesher;

public:

    //////////////////////////////////////////////////////////////////////////
    // Raw Voxel Data
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE bool HasRawVoxelData() const { return this->RawVoxelData.get() != nullptr; }

    FORCEINLINE static LVoxelIndex GetRawVoxelIndex(const LVoxelKey InKey);
    FORCEINLINE static LVoxelIndex GetRawVoxelIndex(const LVoxelKeyDomainTy InX, const LVoxelKeyDomainTy InY, const LVoxelKeyDomainTy InZ);
    FORCEINLINE static LVoxelIndex GetRawVoxelIndex(const i32 InX, const i32 InY, const i32 InZ);

    FORCEINLINE voxel_t GetRawVoxelData(const LVoxelKey InKey) const;
    FORCEINLINE voxel_t GetRawVoxelData(const LVoxelKeyDomainTy InX, const LVoxelKeyDomainTy InY, const LVoxelKeyDomainTy InZ) const;
    FORCEINLINE voxel_t GetRawVoxelData(const i32 InX, const i32 InY, const i32 InZ) const;

    FORCEINLINE voxel_t GetSafeRawVoxelData(const LVoxelKey InKey, const voxel_t InFallback = ECompileTimeVoxels::Air) const;
    FORCEINLINE voxel_t GetSafeRawVoxelData(const LVoxelKeyDomainTy InX, const LVoxelKeyDomainTy InY, const LVoxelKeyDomainTy InZ, const voxel_t InFallback = ECompileTimeVoxels::Air) const;
    FORCEINLINE voxel_t GetSafeRawVoxelData(const i32 InX, const i32 InY, const i32 InZ, const voxel_t InFallback = ECompileTimeVoxels::Air) const;

    FORCEINLINE voxel_t GetRawVoxelDataByNonZeroOrigin(LVoxelKey InKey) const;
    FORCEINLINE voxel_t GetRawVoxelDataByNonZeroOrigin(LVoxelKey InKey, const voxel_t Fallback) const;

    FORCEINLINE void OverrideRawVoxelData(const LVoxelKey InKey, const voxel_t NewVoxel);

private:

    TUnique<voxel_t[]> RawVoxelData;

    //////////////////////////////////////////////////////////////////////////
    // Data Manipulation
    //////////////////////////////////////////////////////////////////////////

public:

    //#
    //# Modify a single voxel in the local voxel space with all side effects.
    //#
    void ModifySingleLocalVoxel(const LVoxelKey InKey, const voxel_t NewVoxel);
    void ModifySingleVoxelByNonZeroOrigin(const LVoxelKey InKey, const voxel_t NewVoxel);

public:

    //////////////////////////////////////////////////////////////////////////
    // Neighbors
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE bool    IsNNorthValid() const noexcept { return this->NNorth != nullptr; }
    FORCEINLINE AChunk* GetNNorth() const noexcept { return this->NNorth; }
    FORCEINLINE bool    IsNEastValid() const noexcept { return this->NEast != nullptr; }
    FORCEINLINE AChunk* GetNEast() const noexcept { return this->NEast; }
    FORCEINLINE bool    IsNSouthValid() const noexcept { return this->NSouth != nullptr; }
    FORCEINLINE AChunk* GetNSouth() const noexcept { return this->NSouth; }
    FORCEINLINE bool    IsNWestValid() const noexcept { return this->NWest != nullptr; }
    FORCEINLINE AChunk* GetNWest() const noexcept { return this->NWest; }
    FORCEINLINE bool    IsNUpValid() const noexcept { return this->NUp != nullptr; }
    FORCEINLINE AChunk* GetNUp() const noexcept { return this->NUp; }
    FORCEINLINE bool    IsNDownValid() const noexcept { return this->NDown != nullptr; }
    FORCEINLINE AChunk* GetNDown() const noexcept { return this->NDown; }

    //# Has to be local or a direct neighbor.
    FORCEINLINE AChunk* GetNeighboringChunk(LVoxelKey* InOutKey);
    FORCEINLINE AChunk* GetNeighboringChunkChecked(LVoxelKey* InOutKey);
    FORCEINLINE AChunk* GetNeighboringChunkAsserted(LVoxelKey* InOutKey);
    FORCEINLINE const AChunk* GetNeighboringChunk(LVoxelKey* InOutKey) const;
    FORCEINLINE const AChunk* GetNeighboringChunkChecked(LVoxelKey* InOutKey) const;
    FORCEINLINE const AChunk* GetNeighboringChunkAsserted(LVoxelKey* InOutKey) const;

private:

    AChunk* NNorth { nullptr };
    AChunk* NEast  { nullptr };
    AChunk* NSouth { nullptr };
    AChunk* NWest  { nullptr };
    AChunk* NUp    { nullptr };
    AChunk* NDown  { nullptr };
};

FORCEINLINE EChunkState::Type AChunk::GetLockedState() const
{
    checkCode
    (
        if (const_cast<AChunk*>(this)->ChunkStateMutex.try_lock())
        {
            const_cast<AChunk*>(this)->ChunkStateMutex.unlock();
            panic("Chunk was not locked. Function was not called inside aggregating thread.")
        }
    )

    return this->State;
}

FORCEINLINE void AChunk::FinishHunt(const EChunkState::Type InState) noexcept
{
    check( this->ChunkStateMutex.try_lock() == false )

    check( this->HuntedState == InState )
    this->HuntedState = EChunkState::Invalid;
    this->State       = InState;

    return;
}

FORCEINLINE void AChunk::SetSharedArgs(LSharedChunkArgs* NewSharedArgs)
{
    checkSlow( NewSharedArgs )
    check( this->IsSharedArgsValid() == false )
#if DO_DOUBLE_CHECK_LIFETIMES
    check( this->HasBegunLife() == false )
#endif /* DO_DOUBLE_CHECK_LIFETIMES */

    this->SharedArgs = NewSharedArgs;

    return;
}

FORCEINLINE LVoxelKey AChunk::CreateRelativeVoxelKey(const LVector& InWorldLocation) const
{
    return LVoxelKey::FromWorldLocationPreserveLocalSpace(InWorldLocation - this->GetTranslation());
}

FORCEINLINE AChunk::LVoxelIndex AChunk::GetRawVoxelIndex(const LVoxelKey InKey)
{
    return InKey.X * MwStatics::ChunkSizeSquared + InKey.Y * MwStatics::ChunkSize + InKey.Z;
}

FORCEINLINE AChunk::LVoxelIndex AChunk::GetRawVoxelIndex(const LVoxelKeyDomainTy InX, const LVoxelKeyDomainTy InY, const LVoxelKeyDomainTy InZ)
{
    return InX * MwStatics::ChunkSizeSquared + InY * MwStatics::ChunkSize + InZ;
}

FORCEINLINE AChunk::LVoxelIndex AChunk::GetRawVoxelIndex(const i32 InX, const i32 InY, const i32 InZ)
{
    return InX * MwStatics::ChunkSizeSquared + InY * MwStatics::ChunkSize + InZ;
}

FORCEINLINE voxel_t AChunk::GetRawVoxelData(const LVoxelKey InKey) const
{
    checkSlow( this->HasRawVoxelData() )
    AChunk::LVoxelIndex Idx = AChunk::GetRawVoxelIndex(InKey);
    return this->RawVoxelData[Idx];
}

FORCEINLINE voxel_t AChunk::GetRawVoxelData(const LVoxelKeyDomainTy InX, const LVoxelKeyDomainTy InY, const LVoxelKeyDomainTy InZ) const
{
    checkSlow( this->HasRawVoxelData() )
    return this->RawVoxelData[AChunk::GetRawVoxelIndex(InX, InY, InZ)];
}

FORCEINLINE voxel_t AChunk::GetRawVoxelData(const i32 InX, const i32 InY, const i32 InZ) const
{
    checkSlow( this->HasRawVoxelData() )
    return this->RawVoxelData[AChunk::GetRawVoxelIndex(InX, InY, InZ)];
}

FORCEINLINE voxel_t AChunk::GetSafeRawVoxelData(const LVoxelKey InKey, const voxel_t InFallback /* = ECompileTimeVoxels::Air */) const
{
    if (this->HasRawVoxelData() && InKey.IsLocal())
    {
        return this->GetRawVoxelData(InKey);
    }
    return InFallback;
}

FORCEINLINE voxel_t AChunk::GetSafeRawVoxelData(const LVoxelKeyDomainTy InX, const LVoxelKeyDomainTy InY, const LVoxelKeyDomainTy InZ, const voxel_t InFallback /* = ECompileTimeVoxels::Air */) const
{
    if (this->HasRawVoxelData() && LVoxelKey{InX, InY, InZ}.IsLocal())
    {
        return this->GetRawVoxelData(InX, InY, InZ);
    }
    return InFallback;
}

FORCEINLINE voxel_t AChunk::GetSafeRawVoxelData(const i32 InX, const i32 InY, const i32 InZ, const voxel_t InFallback /* = ECompileTimeVoxels::Air */) const
{
    if (this->HasRawVoxelData() && LVoxelKey{InX, InY, InZ}.IsLocal())
    {
        return this->GetRawVoxelData(InX, InY, InZ);
    }
    return InFallback;
}

FORCEINLINE voxel_t AChunk::GetRawVoxelDataByNonZeroOrigin(LVoxelKey InKey) const
{
    return this->GetNeighboringChunkChecked(&InKey)->GetRawVoxelData(InKey);
}

FORCEINLINE voxel_t AChunk::GetRawVoxelDataByNonZeroOrigin(LVoxelKey InKey, const voxel_t Fallback) const
{
#if LAL_DO_CHECKS
    const LVoxelKey In = InKey;
#endif /* LAL_DO_CHECKS */
    if (const AChunk* Target = this->GetNeighboringChunk(&InKey); Target)
    {
#if LAL_DO_CHECKS
        if (Target == this) { check( InKey == In ) }
        else { check( InKey != In && InKey.IsLocal() ) }
#endif /* LAL_DO_CHECKS */
        return Target->GetSafeRawVoxelData(InKey, Fallback);
    }

    return Fallback;
}

FORCEINLINE void AChunk::OverrideRawVoxelData(const LVoxelKey InKey, const voxel_t NewVoxel)
{
    this->RawVoxelData[AChunk::GetRawVoxelIndex(InKey)] = NewVoxel;
}

FORCEINLINE AChunk* AChunk::GetNeighboringChunk(LVoxelKey* InOutKey)
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
    default: { checkNoEntry() return nullptr; }
    }
}

FORCEINLINE AChunk* AChunk::GetNeighboringChunkChecked(LVoxelKey* InOutKey)
{
    AChunk* Target = this->GetNeighboringChunk(InOutKey);
    check( Target )
    return Target;
}

FORCEINLINE AChunk* AChunk::GetNeighboringChunkAsserted(LVoxelKey* InOutKey)
{
    AChunk* Target = this->GetNeighboringChunk(InOutKey);
    jassert( Target )
    return Target;
}

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
    default: { checkNoEntry() return nullptr; }
    }
}

FORCEINLINE const AChunk* AChunk::GetNeighboringChunkChecked(LVoxelKey* InOutKey) const
{
    const AChunk* Target = this->GetNeighboringChunk(InOutKey);
    check( Target )
    return Target;
}

FORCEINLINE const AChunk* AChunk::GetNeighboringChunkAsserted(LVoxelKey* InOutKey) const
{
    const AChunk* Target = this->GetNeighboringChunk(InOutKey);
    jassert( Target )
    return Target;
}

} /* ~Namespace Jafg */
