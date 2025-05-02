// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Actor.h"
#include "MyWorld/Chunk/ChunkStates.h"
#include "MyWorld/MyWorldStatics.h"
#include "Components/RenderComponent.h"
#include "MyWorld/ChunkKey.h"
#include "ChunkPersistency.h"
#include "MyWorld/CommonTypes.h"
#include "MyWorld/VoxelKey.h"
#include "Rhi/ChunkShader.h"
#include "Chunk.generated.h"

namespace Jafg
{

class JMaterialSubsystem;
class JVoxelSubsystem;
class LChunkMesher;
class JTextureSubsystem;
class LFastChunkMesher;
class AChunk;
class JChunkGenerationSubsystem;
class JChunkGeneratorSubsystem;
class LChunkMesher;
class LChunkShader;

MAKE_MULTICAST_SIGNATURE(LChunkStateChangedDelegateSignature, EChunkState::Type /* NewChunkState */)

class ENGINE_API LChunkRendererComponent final : public LRendererComponent
{
public:

    LChunkRendererComponent() = delete;
    FORCEINLINE explicit LChunkRendererComponent(AChunk& Owner) : Owner(&Owner) { }
    ~LChunkRendererComponent() override = default;

    virtual bool Cull(const std::span<LVector>& Corners) const override;
    virtual void Draw(const LViewport& Context, const LEye& Eye) override;

    FORCEINLINE auto GetOwner()       noexcept ->       AChunk& { return *this->Owner; }
    FORCEINLINE auto GetOwner() const noexcept -> const AChunk& { return *this->Owner; }
    FORCEINLINE auto GetShaderInstance()       noexcept ->       LChunkShaderInstance* { return &this->Instance; }
    FORCEINLINE auto GetShaderInstance() const noexcept -> const LChunkShaderInstance* { return &this->Instance; }

private:

    AChunk*              Owner    = nullptr;
    LChunkShaderInstance Instance = { };
};

//#
//# Defines shared arguments that are used by every chunk inside a context.
//#
struct LSharedChunkArgs final
{
    JChunkGenerationSubsystem* ChunkGenerationSubsystem  = nullptr;
    JChunkGeneratorSubsystem*  ChunkGeneratorSubsystem   = nullptr;
    JVoxelSubsystem*           VoxelSubsystem            = nullptr;
    JMaterialSubsystem*        MaterialSubsystem         = nullptr;
    JTextureSubsystem*         TextureSubsystem          = nullptr;
    LChunkShader               ChunkShader;
    u32                        ChunkShaderHandle         = NULL;
    TFunction<LChunkMesher*(AChunk& Owner)> GetNewMesher = nullptr;
};

DECLARE_JAFG_CLASS()
class ENGINE_API AChunk final : public AActor
{
    GENERATED_CLASS_BODY()

public:

    //# The index in the raw voxel data array.
    typedef i32 LVoxelIndex;

    static_assert(
        MwStatics::ChunkSize < std::numeric_limits<LVoxelKeyDomainTy>::max() - 1,
        "Encountered to large chunk size for indexing the raw voxels domains "
        "- this will result in undefined overflow behavior."
    );

protected:

    explicit AChunk(const LObjectInitializer& ObjectInitializer);

public:

    // AActor implementation
    virtual void BeginLife() override;
    virtual void EndLife() override;
    FORCEINLINE auto GetChunkRendererComponent() -> LChunkRendererComponent* { return reinterpret_cast<LChunkRendererComponent*>(this->GetRendererComponent()); }
    FORCEINLINE auto GetChunkRendererComponent() const -> const LChunkRendererComponent* { return reinterpret_cast<LChunkRendererComponent*>(this->GetRendererComponent()); }
    // ~AActor implementation

                auto SetChunkState(const EChunkState::Type NewChunkState) -> void;
    FORCEINLINE auto GetChunkState() const -> EChunkState::Type { return this->ChunkState; }
    FORCEINLINE auto SetHuntedChunkState(const EChunkState::Type NewHuntedChunkState) -> void;
    FORCEINLINE auto GetHuntedChunkState() const -> EChunkState::Type { return this->HuntedChunkState; }

    void OnAlloc(const LChunkKey& InChunkKey);

                auto SetChunkPersistency(const EChunkPersistency::Type NewPersistency, const float TimeToLive = 10.0f) -> void;
    FORCEINLINE auto GetChunkPersistency() const -> EChunkPersistency::Type { return this->ChunkPersistency; }
    FORCEINLINE auto IsPersistent() const -> bool { return this->ChunkPersistency == EChunkPersistency::Persistent; }
    FORCEINLINE auto IsTransient() const -> bool { return this->ChunkPersistency == EChunkPersistency::Transient;  }
    FORCEINLINE auto ShouldBeFreed() const -> bool;

    FORCEINLINE auto GetChunkKey() const -> const LChunkKey& { return this->ChunkKey; }

    FORCEINLINE bool IsSharedArgsValid() const noexcept { return this->SharedArgs != nullptr; }
    FORCEINLINE auto GetSharedArgs() noexcept -> LSharedChunkArgs* { return this->SharedArgs; }
    FORCEINLINE auto GetSharedArgs() const noexcept -> const LSharedChunkArgs* { return this->SharedArgs; }
    FORCEINLINE void SetSharedArgs(LSharedChunkArgs* NewSharedArgs);

    FORCEINLINE auto IsMesherValid() const -> bool { return this->Mesher != nullptr; }
    FORCEINLINE auto GetMesher() -> LChunkMesher* { return this->Mesher; }
    FORCEINLINE auto GetMesher() const -> const LChunkMesher* { return this->Mesher; }

    //#
    //# Create a relative voxel key from the world location of this chunk.
    //#
    FORCEINLINE auto CreateRelativeVoxelKey(const LVector& InWorldLocation) const -> LVoxelKey;

private:

    //#
    //# The real time (not stopped or dilated / clamped) when this chunk should be killed by the generation subsystem.
    //# The time is relative to the time when the world was launched where this AActor lives in.
    //# Only meaningful when the persistency of this chunk is transient.
    //#
    float RealTimeInSecondsWhenTransientChunkShouldBeKilled = 0.0f;
    EChunkPersistency::Type ChunkPersistency = EChunkPersistency::Persistent;

    bool IsStateChangeValid(const EChunkState::Type NewChunkState) const;
    EChunkState::Type ChunkState       = EChunkState::Invalid;
    EChunkState::Type HuntedChunkState = EChunkState::Invalid;

    void OnSpawned();
    void Shape();
    void ReplaceSurface();
    void OnActive();

    LChunkKey         ChunkKey   = { };
    LSharedChunkArgs* SharedArgs = nullptr;
    LChunkMesher*     Mesher     = nullptr;

public:

    //////////////////////////////////////////////////////////////////////////
    // Raw Voxel Data
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE bool HasRawVoxelData() const { return this->RawVoxelData; }

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

    voxel_t* RawVoxelData = nullptr;

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

    //# Has to be local or a direct neighbor.
    FORCEINLINE auto GetNeighboringChunk(LVoxelKey* InOutKey) -> AChunk*;
    FORCEINLINE auto GetCheckedNeighboringChunk(LVoxelKey* InOutKey) -> AChunk*;
    FORCEINLINE auto GetPanickedNeighboringChunk(LVoxelKey* InOutKey) -> AChunk*;
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
};

void AChunk::SetHuntedChunkState(const EChunkState::Type NewHuntedChunkState)
{
    check( EChunkState::Freed < NewHuntedChunkState && NewHuntedChunkState < EChunkState::Special )
    check( NewHuntedChunkState > this->ChunkState )
    this->HuntedChunkState = NewHuntedChunkState;
    return;
}

bool AChunk::ShouldBeFreed() const
{
    return this->IsTransient()
        &&   this->RealTimeInSecondsWhenTransientChunkShouldBeKilled
           < this->GetWorld()->GetRealTimeSecondsSinceWorldLaunch();
}

void AChunk::SetSharedArgs(LSharedChunkArgs* NewSharedArgs)
{
    checkSlow( NewSharedArgs )
    check( this->IsSharedArgsValid() == false )
#if DO_DOUBLE_CHECK_LIFETIMES
    check( this->HasBegunLife() == false )
#endif /* DO_DOUBLE_CHECK_LIFETIMES */

    this->SharedArgs = NewSharedArgs;

    return;
}

LVoxelKey AChunk::CreateRelativeVoxelKey(const LVector& InWorldLocation) const
{
    return LVoxelKey::FromWorldLocationPreserveLocalSpace(InWorldLocation - this->GetTranslation());
}

AChunk::LVoxelIndex AChunk::GetRawVoxelIndex(const LVoxelKey InKey)
{
    return InKey.X * MwStatics::ChunkSizeSquared + InKey.Y * MwStatics::ChunkSize + InKey.Z;
}

AChunk::LVoxelIndex AChunk::GetRawVoxelIndex(const LVoxelKeyDomainTy InX, const LVoxelKeyDomainTy InY, const LVoxelKeyDomainTy InZ)
{
    return InX * MwStatics::ChunkSizeSquared + InY * MwStatics::ChunkSize + InZ;
}

AChunk::LVoxelIndex AChunk::GetRawVoxelIndex(const i32 InX, const i32 InY, const i32 InZ)
{
    return InX * MwStatics::ChunkSizeSquared + InY * MwStatics::ChunkSize + InZ;
}

voxel_t AChunk::GetRawVoxelData(const LVoxelKey InKey) const
{
    checkSlow( this->HasRawVoxelData() )
    return this->RawVoxelData[AChunk::GetRawVoxelIndex(InKey)];
}

voxel_t AChunk::GetRawVoxelData(const LVoxelKeyDomainTy InX, const LVoxelKeyDomainTy InY, const LVoxelKeyDomainTy InZ) const
{
    checkSlow( this->HasRawVoxelData() )
    return this->RawVoxelData[AChunk::GetRawVoxelIndex(InX, InY, InZ)];
}

voxel_t AChunk::GetRawVoxelData(const i32 InX, const i32 InY, const i32 InZ) const
{
    checkSlow( this->HasRawVoxelData() )
    return this->RawVoxelData[AChunk::GetRawVoxelIndex(InX, InY, InZ)];
}

voxel_t AChunk::GetSafeRawVoxelData(const LVoxelKey InKey, const voxel_t InFallback /* = ECompileTimeVoxels::Air */) const
{
    checkSlow( this->HasRawVoxelData() )
    if (InKey.IsLocal())
    {
        return this->GetRawVoxelData(InKey);
    }
    return InFallback;
}

voxel_t AChunk::GetSafeRawVoxelData(const LVoxelKeyDomainTy InX, const LVoxelKeyDomainTy InY, const LVoxelKeyDomainTy InZ, const voxel_t InFallback /* = ECompileTimeVoxels::Air */) const
{
    checkSlow( this->HasRawVoxelData() )
    if (LVoxelKey(InX, InY, InZ).IsLocal())
    {
        return this->GetRawVoxelData(InX, InY, InZ);
    }
    return InFallback;
}

voxel_t AChunk::GetSafeRawVoxelData(const i32 InX, const i32 InY, const i32 InZ, const voxel_t InFallback /* = ECompileTimeVoxels::Air */) const
{
    checkSlow( this->HasRawVoxelData() )
    if (LVoxelKey(InX, InY, InZ).IsLocal())
    {
        return this->GetRawVoxelData(InX, InY, InZ);
    }
    return InFallback;
}

voxel_t AChunk::GetRawVoxelDataByNonZeroOrigin(LVoxelKey InKey) const
{
    return this->GetCheckedNeighboringChunk(&InKey)->GetRawVoxelData(InKey);
}

voxel_t AChunk::GetRawVoxelDataByNonZeroOrigin(LVoxelKey InKey, const voxel_t Fallback) const
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

void AChunk::OverrideRawVoxelData(const LVoxelKey InKey, const voxel_t NewVoxel)
{
    this->RawVoxelData[AChunk::GetRawVoxelIndex(InKey)] = NewVoxel;
}

AChunk* AChunk::GetNeighboringChunk(LVoxelKey* InOutKey)
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

AChunk* AChunk::GetCheckedNeighboringChunk(LVoxelKey* InOutKey)
{
    if (AChunk* Target = this->GetNeighboringChunk(InOutKey); Target)
    {
        return Target;
    }
    checkNoEntry()
    return nullptr;
}

AChunk* AChunk::GetPanickedNeighboringChunk(LVoxelKey* InOutKey)
{
    if (AChunk* Target = this->GetNeighboringChunk(InOutKey); Target)
    {
        return Target;
    }
    panic( "Failed to find target chunk by local voxel key." )
    return nullptr;
}

const AChunk* AChunk::GetNeighboringChunk(LVoxelKey* InOutKey) const
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

const AChunk* AChunk::GetCheckedNeighboringChunk(LVoxelKey* InOutKey) const
{
    if (const AChunk* Target = this->GetNeighboringChunk(InOutKey); Target)
    {
        return Target;
    }
    checkNoEntry()
    return nullptr;
}

const AChunk* AChunk::GetPanickedNeighboringChunk(LVoxelKey* InOutKey) const
{
    if (const AChunk* Target = this->GetNeighboringChunk(InOutKey); Target)
    {
        return Target;
    }
    panic( "Failed to find target chunk by local voxel key." )
    return nullptr;
}

} /* ~Namespace Jafg */
