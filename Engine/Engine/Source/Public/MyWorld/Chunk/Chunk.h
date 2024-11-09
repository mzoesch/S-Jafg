// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Actor.h"
#include "MyWorld/Chunk/ChunkStates.h"
#include "Engine/Components/RenderComponent.h"
#include "MyWorld/Chunk/ChunkKey.h"
#include <glm/glm.hpp>
#include "MyWorld/WorldStatics.h"
#include "RhiFramework/Shader.h"
#include "ChunkPersistency.h"
#include "RhiFramework/ChunkShaderContext.h"
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

    friend LChunkMesher;
    friend LFastChunkMesher;

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
    // Procedural Mesh
    //////////////////////////////////////////////////////////////////////////

    FORCEINLINE bool HasRawVoxelData() const { return this->RawVoxelData; }

    uint32* RawVoxelData = nullptr;
    glm::vec3 ChunkPos = glm::vec3(0.0f);

    LSharedChunkArgs* SharedArgs = nullptr;
    LChunkKey  ChunkKey      = { };
    LVector    WorldLocation = { };

    FORCEINLINE static int32 GetIndex(const int32 X, const int32 Y, const int32 Z)
    {
        return X + Y * WorldStatics::ChunkSize + Z * WorldStatics::ChunkSize * WorldStatics::ChunkSize;
    }

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

private:

    AChunk* NNorth = nullptr;
    AChunk* NEast  = nullptr;
    AChunk* NSouth = nullptr;
    AChunk* NWest  = nullptr;
    AChunk* NUp    = nullptr;
    AChunk* NDown  = nullptr;

#pragma endregion Neighbors
};

} /* ~Namespace Jafg */
