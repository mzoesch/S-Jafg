// Copyright mzoesch. All rights reserved.

#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "Core/CoreNames.h"
#include "Framework/Pawn.h"
#include "MyWorld/Chunk/ChunkStates.h"
#include "MyWorld/Generation/ChunkGeneratorSubsystem.h"
#include "MyWorld/Meshing/NaiveMesher.h"
#include "System/VoxelSubsystem.h"
#include "System/MaterialSubsystem.h"
#include "System/VoxelTextureSubsystem.h"
#include "User/UserPreferences.h"
#include "Engine/Engine.h"
#include "Physics/PhysicCompontent.h"
#include "Rhi/ChunkShader.h"
#include "Stats/Stats.h"

bool Jafg::JChunkGenerationSubsystem::ShouldCreateSubsystem(LClassOuter const* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer) == false)
    {
        return false;
    }

    return Super::IsOuterWorld(Outer);
}

void Jafg::JChunkGenerationSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    this->SetTickInterval(0.0f);

    const JUserPreferences* Prefs { GetDefault<JUserPreferences>() };

    this->LoadedChunks = std::unordered_map<LChunkKey, AChunk*>
    (
        static_cast<i64>(static_cast<f32>((Prefs->ChunkRenderDistance * Prefs->ChunkRenderDistance) * Prefs->ChunkRenderHeight) * 1.2f)
    );

    this->SharedChunkArgs.ChunkGenerationSubsystem = this;
    this->SharedChunkArgs.ChunkGeneratorSubsystem = Collection.GetSubsystemChecked<JChunkGeneratorSubsystem>();
    this->SharedChunkArgs.VoxelSubsystem = this->GetEngine().GetSubsystemChecked<JVoxelSubsystem>();
    this->SharedChunkArgs.MaterialSubsystem = this->GetEngine().GetSubsystemChecked<JMaterialSubsystem>();
    this->SharedChunkArgs.VoxelTextureSubsystem = this->GetEngine().GetSubsystemChecked<JVoxelTextureSubsystem>();
    this->SharedChunkArgs.ChunkShader.MakeChecked(Name_ShaderChunk);
    this->SharedChunkArgs.GetNewMesher = [](AChunk& Owner) -> TUnique<LChunkMesher>
    {
        return std::make_unique<LNaiveMesher>(Owner);
    };

    this->SharedChunkArgs.bSuperFlat = true;

    this->GetWorld()->OnStaticLineTrace.BindMember(this, &JChunkGenerationSubsystem::LineTraceByChannel);
    this->GetWorld()->OnStaticDraw.BindMember(this, &JChunkGenerationSubsystem::OnStaticDraw);

    return;
}

void Jafg::JChunkGenerationSubsystem::FixedTick(const f32 EngineDeltaTime, const f32 FixedDeltaTime)
{
    STAT_CYCLE_FUNCTION()

    check( Tasks::IsOnMasterThread() )

    Super::FixedTick(EngineDeltaTime, FixedDeltaTime);

    constexpr i32 OutBuffer { 30 };

    const i32 Size { this->OutActiveChunks.GetSizeSlow() };

    if (const i32 New { OutBuffer - Size }; New > 0)
    {
        i32 GeneratedChunks { 0 };
        while (this->Requested.empty() == false && GeneratedChunks < 20)
        {
            const LChunkKey& Key = this->Requested.back();

            AChunk* Chunk { this->FindChunk(Key) };
            if (Chunk == nullptr)
            {
                Chunk = this->SpawnWeakChunk(Key);
            }
            check( Chunk )

            if (Chunk->HuntedState == EChunkState::Invalid)
            {
                if (Chunk->State != EChunkState::Active)
                {
                    this->OutActiveChunks.Enqueue(Chunk);
                    ++GeneratedChunks;
                }
            }

            this->Requested.pop_back();

            continue;
        }
    }

    return;
}

void Jafg::JChunkGenerationSubsystem::TearDown()
{
    STAT_CYCLE_FUNCTION()

    Super::TearDown();

    check( Tasks::IsOnMasterThread() )

    this->LoadedChunksMutex.lock();
    check( static_cast<bool>(this->LoadedChunks) )

    for (const std::pair<const LChunkKey&, AChunk*> Pair : this->LoadedChunks.value())
    {
        Pair.second->KillYourSelfNow_v2();
    }

    this->LoadedChunks.reset();
    GEngine->UnregisterShader(Name_ShaderChunk, false);

    return;
}

bool Jafg::JChunkGenerationSubsystem::LineTraceByChannel(
    TArray<LHitResult>& OutHits,
    const LVector& Start,
    const LVector& End,
    const LCollisionQueryParams& Params
) const
{
    STAT_CYCLE_FUNCTION()

    constexpr f32 Step { 0.5f };

    const f32 TraversableDistance = (End - Start).Magnitude();
    const LVector NormalStep = (End - Start).GetUnsafeNormalized() * Step;

    f32 CurDistance = 0.0f;
    LVector Cursor { Start };

    LChunkKey Last;
    LHitResult Dummy;

    std::shared_lock Lock(this->LoadedChunksMutex);

    while (CurDistance < TraversableDistance)
    {
        const LChunkKey Current { Cursor };

        if (Current == Last)
        {
            CurDistance += Step;
            Cursor += NormalStep;
            continue;
        }
        Last = Current;

        if
        (
            const std::unordered_map<LChunkKey, AChunk*>::const_iterator Element = this->LoadedChunks->find(Current);
            Element != this->LoadedChunks->end()
        )
        {
            if (Element->second->GetPhysicsComponent()->Sweep(Start, End, Dummy))
            {
                OutHits.push_back(Dummy);

                if (Params.bSingleHit)
                {
                    break;
                }

                Dummy.Reset();
            }
        }

        CurDistance += Step;
        Cursor += NormalStep;

        continue;
    }

    return OutHits.empty() == false;
}

void Jafg::JChunkGenerationSubsystem::OnStaticDraw(const LViewport& Viewport, const LEye& Eye, const std::span<LVector>& Corners) const
{
    std::shared_lock Lock(this->LoadedChunksMutex);

    for (const std::pair<const LChunkKey&, AChunk*> Pair : this->LoadedChunks.value())
    {
        check( Pair.second->IsGarbage() == false )

        if
        (
               Pair.second->IsRendererComponentValid()
            && Pair.second->GetRendererComponent()->Cull(Corners) == false
        )
        {
            Pair.second->GetRendererComponent()->Draw(Viewport, Eye);
        }
    }

    return;
}

Jafg::AChunk* Jafg::JChunkGenerationSubsystem::SpawnWeakChunk(const LChunkKey& InChunkKey)
{
    STAT_CYCLE_FUNCTION()

    AChunk* Chunk = SpawnDeferredActor<AChunk>(this->GetWorld(), AChunk::StaticClass());
    Chunk->ChunkKey = InChunkKey;
    Chunk->SetSharedArgs(&this->SharedChunkArgs);
    MakeDeferredActorFinal(Chunk);

    std::unique_lock Lock(this->LoadedChunksMutex);
    check( this->LoadedChunks->contains(InChunkKey) == false )
    this->LoadedChunks->emplace(InChunkKey, Chunk);

    return Chunk;
}
