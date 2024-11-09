// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "MyWorld/Generation/ChunkGenerationSubsystem.h"
#include "JustTemp.h"
#include "Engine/Framework/Camera.h"
#include "Engine/World.h"
#include "RhiFramework/ChunkShaderContext.h"

void GetAllChunksInDistance(const Jafg::TIntVector2<int32>& Center, const int32 Distance, std::vector<Jafg::TIntVector2<int32>>& OutChunks)
{
    using namespace Jafg;

    const int32 PredictedOutChunkCount = (Distance * 2 + 1) * (Distance * 2 + 1);
    OutChunks.reserve(PredictedOutChunkCount);

    auto MoveCursorRight = [] (const TIntVector2<int32>& CursorLocation) { return TIntVector2<int32>(CursorLocation.X + 1, CursorLocation.Y); };
    auto MoveCursorDown  = [] (const TIntVector2<int32>& CursorLocation) { return TIntVector2<int32>(CursorLocation.X, CursorLocation.Y - 1); };
    auto MoveCursorLeft  = [] (const TIntVector2<int32>& CursorLocation) { return TIntVector2<int32>(CursorLocation.X - 1, CursorLocation.Y); };
    auto MoveCursorUp    = [] (const TIntVector2<int32>& CursorLocation) { return TIntVector2<int32>(CursorLocation.X, CursorLocation.Y + 1); };
    const std::vector<TIntVector2<int32>(*)(const TIntVector2<int32>&)> Moves =
    {
        MoveCursorRight, MoveCursorDown, MoveCursorLeft, MoveCursorUp
    };

    int32 Cursor = 1;
    int32 CurrentMoveIndex = 0;
    int32 TimesToMove = 1;
    TIntVector2<int32> TargetPoint = Center;

    OutChunks.emplace_back(Center.X, Center.Y);

    while (true)
    {
        for (int32 _1 = 0; _1 < 2; ++_1)
        {
            CurrentMoveIndex = (CurrentMoveIndex + 1) % static_cast<int32>(Moves.size());
            for (int32 _2 = 0; _2 < TimesToMove; ++_2)
            {
                TargetPoint = Moves[CurrentMoveIndex](TargetPoint);

                if (Cursor++ >= PredictedOutChunkCount)
                {
                    goto FunctionEnd;
                }

                OutChunks.emplace_back(TargetPoint.X, TargetPoint.Y);

                continue;
            }

            continue;
        }

        ++TimesToMove;
        continue;
    }

FunctionEnd:

    checkCode(
        check( static_cast<int32>(OutChunks.size()) == PredictedOutChunkCount )

        for (uint64 i = 0; i < OutChunks.size(); ++i)
        {
            for (uint64 j = i + 1; j < OutChunks.size(); ++j)
            {
                check( OutChunks[i] != OutChunks[j] )
            }
        }
    )

    return;
}

void Jafg::JChunkGenerationSubsystem::Initialize(Jafg::LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);
    this->SetTickInterval(0.0f);

    this->ChunkShaderContext = new LChunkShaderContext();
    this->ChunkShaderContext->Make();

    this->SharedChunkArgs = new LSharedChunkArgs();
    this->SharedChunkArgs->ChunkGenerationSubsystem = this;

    return;
}

void Jafg::JChunkGenerationSubsystem::FixedTick(const float EngineDeltaTime, const float SubsystemDeltaTime)
{
    Super::FixedTick(EngineDeltaTime, SubsystemDeltaTime);

    this->UpdateChunkQueue();
    this->KillChunks();
    this->GenerateChunks();

    return;
}

void Jafg::JChunkGenerationSubsystem::TearDown()
{
    Super::TearDown();

    this->ChunkShaderContext->Free();
    delete this->ChunkShaderContext;
    this->ChunkShaderContext = nullptr;

    delete this->SharedChunkArgs;
    this->SharedChunkArgs = nullptr;

    return;
}

void Jafg::JChunkGenerationSubsystem::UpdateChunkQueue()
{
    const float CamX = this->GetWorld()->MainCamera->Location.X;
    const float CamY = this->GetWorld()->MainCamera->Location.Y;

    const int32 CurrentCamX = static_cast<int32>(CamX < 0 ? floor(CamX / static_cast<float>(ChunkSize)) : CamX / static_cast<float>(ChunkSize));
    const int32 CurrentCamY = static_cast<int32>(CamY < 0 ? floor(CamY / static_cast<float>(ChunkSize)) : CamY / static_cast<float>(ChunkSize));

    if (CurrentCamX == LastCamX && CurrentCamY == LastCamY)
    {
        return;
    }

    LOG_TRACE(LogTemporal, "{:.2f} {:.2f} - {} {}",
        CamX, CamY, CurrentCamX, CurrentCamY)

    LastCamX = CurrentCamX;
    LastCamY = CurrentCamY;


    ChunkQueue = { };
    const TIntVector2<int32> Center(CurrentCamX, CurrentCamY);
    std::vector<TIntVector2<int32>> VerticalChunks;
    GetAllChunksInDistance(Center, RenderDistance, VerticalChunks);

    for (const auto& Chunk : VerticalChunks)
    {
        for (int32 z = 0; z <= RenderHeight; z++)
        {
            ChunkQueue.emplace(Chunk.X, Chunk.Y, z);
        }
    }

    return;
}

void Jafg::JChunkGenerationSubsystem::KillChunks()
{
    for (auto It = Chunks.begin(); It != Chunks.end();)
    {
        if (It->second->bReady && (
               abs(It->second->ChunkPos.x - static_cast<float>(LastCamX)) > static_cast<float>(RenderDistance)
            || abs(It->second->ChunkPos.y - static_cast<float>(LastCamY)) > static_cast<float>(RenderDistance)
        ))
        {
            It->second->KillYourSelfNow();
            It = Chunks.erase(It);
        }
        else
        {
            ++It;
        }
    }

    return;
}

void Jafg::JChunkGenerationSubsystem::GenerateChunks()
{
    int32 GeneratedChunks = 0;
    while (!ChunkQueue.empty() && GeneratedChunks < 2)
    {
        const glm::vec3 Next = ChunkQueue.front();
        ChunkQueue.pop();

        int32 Key = TupleToKey({static_cast<int32>(Next.x), static_cast<int32>(Next.y), static_cast<int32>(Next.z)});
        if (!Chunks.contains(Key))
        {
            AChunk* Chunk = SpawnDeferredActor<AChunk>(this->GetWorld());
            Chunk->ChunkPos = Next;
            Chunk->ChunkKey =
                { static_cast<LChunkKeyDomainTy>(Next.x), static_cast<LChunkKeyDomainTy>(Next.y), static_cast<LChunkKeyDomainTy>(Next.z) };
            Chunk->SharedArgs = this->SharedChunkArgs;
            MakeDeferredActorFinal(Chunk);
            Chunks.try_emplace(Key, Chunk);
            ++GeneratedChunks;
        }
    }

    return;
}
