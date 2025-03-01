// Copyright mzoesch. All rights reserved.

#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "Engine/World.h"
#include "Engine/Framework/Pawn.h"
#include "Rhi/ChunkShaderContext.h"
#include "MyWorld/Chunk/ChunkStates.h"
#include "MyWorld/Generation/ChunkGeneratorSubsystem.h"
#include "MyWorld/Meshing/NaiveMesher.h"
#include "System/VoxelSubsystem.h"
#include "System/MaterialSubsystem.h"
#include "System/TextureSubsystem.h"
#include "User/UserPreferences.h"
#include "Engine/Engine.h"

void Jafg::JChunkGenerationSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    this->SetTickInterval(0.0f);

    this->LoadedChunks = new std::unordered_map<LChunkKey, AChunk*>();

    this->ChunkShaderContext = new LChunkShaderContext();
    this->ChunkShaderContext->Make();

    this->SharedChunkArgs = new LSharedChunkArgs();
    this->SharedChunkArgs->ChunkGenerationSubsystem = this;
    this->SharedChunkArgs->ChunkGeneratorSubsystem  = Collection.GetCheckedSubsystem<JChunkGeneratorSubsystem>();
    this->SharedChunkArgs->VoxelSubsystem  = this->GetEngine()->GetCheckedSubsystem<JVoxelSubsystem>();
    this->SharedChunkArgs->MaterialSubsystem  = this->GetEngine()->GetCheckedSubsystem<JMaterialSubsystem>();
    this->SharedChunkArgs->TextureSubsystem  = this->GetEngine()->GetCheckedSubsystem<JTextureSubsystem>();
    this->SharedChunkArgs->GetNewMesher = [] (AChunk& Owner) -> LChunkMesher* { return new LNaiveMesher(Owner); };

    const JUserPreferences* Preferences = GetDefault<JUserPreferences>();
    this->RenderDistance = &Preferences->ChunkRenderDistance;
    this->RenderHeight   = &Preferences->ChunkRenderHeight;

    return;
}

void Jafg::JChunkGenerationSubsystem::FixedTick(const float EngineDeltaTime, const float SubsystemDeltaTime)
{
    Super::FixedTick(EngineDeltaTime, SubsystemDeltaTime);

    this->DequeueVipChunks();

    int32 GeneratedChunks = 0;
    while (this->OptimalVerticalChunkQueue.IsEmpty() == false && GeneratedChunks < 5)
    {
        if (this->DequeueNextOptimalVerticalChunk())
        {
            ++GeneratedChunks;
        }
    }

    return;
}

void Jafg::JChunkGenerationSubsystem::TearDown()
{
    Super::TearDown();

    check( Tasks::IsOnMasterThread() )

    this->ChunkShaderContext->Free();
    delete this->ChunkShaderContext;
    this->ChunkShaderContext = nullptr;

    checkSlow( this->SharedChunkArgs )
    delete this->SharedChunkArgs;
    this->SharedChunkArgs = nullptr;

    this->LoadedChunksMutex.lock();
    this->VipChunksToLoadMutex.lock();

    checkSlow( this->LoadedChunks )
    delete this->LoadedChunks;
    this->LoadedChunks = nullptr;

    return;
}

Jafg::AChunk* Jafg::JChunkGenerationSubsystem::SpawnChunk(const LChunkKey& InChunkKey) const
{
    AChunk* Chunk = SpawnDeferredActor<AChunk>(this->GetWorld());
    Chunk->SetSharedArgs(this->SharedChunkArgs);
    MakeDeferredActorFinal(Chunk);
    Chunk->OnAlloc(InChunkKey);
    return Chunk;
}

void Jafg::JChunkGenerationSubsystem::DequeueVipChunks()
{
    this->VipChunksToLoadMutex.lock();
    LChunkKey VipChunkKey;
    while (this->VipChunksToLoad.Dequeue(VipChunkKey))
    {
        this->SafeLoadPersistentChunkPreSpawnedChunk(VipChunkKey);
    }
    this->VipChunksToLoadMutex.unlock();

    return;
}

bool Jafg::JChunkGenerationSubsystem::DequeueNextOptimalVerticalChunk()
{
    LChunkKey2 OptimalVerticalChunk;
    if (this->OptimalVerticalChunkQueue.Dequeue(OptimalVerticalChunk) == false)
    {
        checkNoEntry()
        return false;
    }

    bool Ret = false;
    for (int32 Z = 0; Z <= this->GetRenderHeight(); ++Z)
    {
        AChunk* Chunk;
        if (Chunk = this->FindLoadedChunkOrNull(LChunkKey(OptimalVerticalChunk, Z)); Chunk == nullptr)
        {
            this->SafeLoadPersistentChunkPreSpawnedChunk(LChunkKey(OptimalVerticalChunk, Z));
            Ret = true;
            continue;
        }

        if (Chunk->GetHuntedChunkState() != EChunkState::Active)
        {
            this->SafeLoadPersistentChunkPreSpawnedChunk(LChunkKey(OptimalVerticalChunk, Z));
            Ret = true;
            continue;
        }

        continue;
    }

    return Ret;
}

void Jafg::JChunkGenerationSubsystem::SafeLoadPersistentChunkPreSpawnedChunk(const LChunkKey& ChunkKey)
{
    std::unique_lock Lock(this->LoadedChunksMutex);
    AChunk* Chunk = this->SpawnChunk(ChunkKey);
    this->LoadedChunks->emplace(ChunkKey, Chunk);

    return;
}
