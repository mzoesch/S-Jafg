// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "MyWorld/Chunk/ChunkGenerationSubsystem.h"
#include "JustTemp.h"
#include "Engine/Framework/Camera.h"

void GetAllChunksInDistance(const TIntVector2<int32>& Center, const int32 Distance, std::vector<TIntVector2<int32>>& OutChunks)
{
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
    JWorldSubsystem::Initialize(Collection);
}

void Jafg::JChunkGenerationSubsystem::Tick(const float DeltaTime)
{
    this->UpdateChunkQueue();
    this->KillChunks();
    this->GenerateChunks();
    this->RenderChunks();

    return;
}

void Jafg::JChunkGenerationSubsystem::TearDown()
{
    JWorldSubsystem::TearDown();
}

void Jafg::JChunkGenerationSubsystem::UpdateChunkQueue()
{
    const float CamX = this->GetWorld()->MainCamera->Position.x;
    const float CamY = this->GetWorld()->MainCamera->Position.y;

    const int32 CurrentCamX = static_cast<int32>(CamX < 0 ? floor(CamX / static_cast<float>(ChunkSize)) : CamX / static_cast<float>(ChunkSize));
    const int32 CurrentCamY = static_cast<int32>(CamY < 0 ? floor(CamY / static_cast<float>(ChunkSize)) : CamY / static_cast<float>(ChunkSize));

    if (CurrentCamX == LastCamX && CurrentCamY == LastCamY)
    {
        return;
    }

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
        if (It->second->ready && (
               abs(It->second->chunkPos.x - static_cast<float>(LastCamX)) > static_cast<float>(RenderDistance)
            || abs(It->second->chunkPos.y - static_cast<float>(LastCamY)) > static_cast<float>(RenderDistance)
        ))
        {
            It->second->~AChunk();
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
        glm::vec3 Next = ChunkQueue.front();
        ChunkQueue.pop();

        if (!Chunks.contains(TupleToKey({Next.x, Next.y, Next.z})))
        {
            Chunks.try_emplace(TupleToKey({Next.x, Next.y, Next.z}), new AChunk(Next));
            ++GeneratedChunks;
        }
    }

    return;
}

void Jafg::JChunkGenerationSubsystem::RenderChunks()
{
    // const uint32 ModelLoc = glGetUniformLocation(this->GetWorld()->ShaderProgram->ID, "model");
    const uint32 ModelLoc = JustTemp::D(this->GetWorld()->ShaderProgram);

    for (const auto& Chunk : Chunks | std::views::values)
    {
        Chunk->Render(ModelLoc);
    }

    return;
}
