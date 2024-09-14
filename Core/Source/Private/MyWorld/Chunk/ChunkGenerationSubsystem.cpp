// Copyright mzoesch. All rights reserved.

#include "MyWorld/Chunk/ChunkGenerationSubsystem.h"
#include "Engine/World.h"
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

void JChunkGenerationSubsystem::Initialize(LSubsystemCollection& Collection)
{
    JWorldSubsystem::Initialize(Collection);
}

void JChunkGenerationSubsystem::Tick(const float DeltaTime)
{
    this->UpdateChunkQueue();
    this->KillChunks();
    this->GenerateChunks();
    this->RenderChunks();

    // const uint32 ModelLoc = glGetUniformLocation(this->GetWorld()->ShaderProgram.ID, "model");
    //
    // const float CamX = this->GetWorld()->MainCamera->Position.x;
    // const float CamY = this->GetWorld()->MainCamera->Position.y;
    // const float CamZ = this->GetWorld()->MainCamera->Position.z;
    //
    // int CamChunkX = CamX < 0 ? floor(CamX / ChunkSize) : CamX / ChunkSize;
    // int CamChunkY = CamY < 0 ? floor(CamY / ChunkSize) : CamY / ChunkSize;
    // int CamChunkZ = CamZ < 0 ? floor(CamZ / ChunkSize) : CamZ / ChunkSize;
    //
    // // Check if the camera moved to new chunk
    // if (CamChunkX != LastCamX || CamChunkY != LastCamY || CamChunkZ != LastCamZ)
    // {
    //     // Player moved chunks, start new chunk queue
    //     LastCamX = CamChunkX;
    //     LastCamY = CamChunkY;
    //     LastCamZ = CamChunkZ;
    //
    //     // Current chunk
    //     ChunkQueue = {};
    //     if (!Chunks.contains(TupleToKey({CamChunkX, CamChunkY, CamChunkZ})))
    //         ChunkQueue.emplace(CamChunkX, CamChunkY, CamChunkZ);
    //
    //     for (int r = 0; r < RenderDistance; r++)
    //     {
    //         // Add middle chunks
    //         for (int y = 0; y <= RenderHeight; y++)
    //         {
    //             ChunkQueue.emplace(CamChunkX, y, CamChunkZ + r);
    //             ChunkQueue.emplace(CamChunkX + r, y, CamChunkZ);
    //             ChunkQueue.emplace(CamChunkX, y, CamChunkZ - r);
    //             ChunkQueue.emplace(CamChunkX - r, y, CamChunkZ);
    //
    //             if (y > 0)
    //             {
    //                 ChunkQueue.emplace(CamChunkX, -y, CamChunkZ + r);
    //                 ChunkQueue.emplace(CamChunkX + r, -y, CamChunkZ);
    //                 ChunkQueue.emplace(CamChunkX, -y, CamChunkZ - r);
    //                 ChunkQueue.emplace(CamChunkX - r, -y, CamChunkZ);
    //             }
    //         }
    //
    //         // Add edges
    //         for (int e = 1; e < r; e++)
    //         {
    //             for (int y = 0; y <= RenderHeight; y++)
    //             {
    //                 ChunkQueue.emplace(CamChunkX + e, y, CamChunkZ + r);
    //                 ChunkQueue.emplace(CamChunkX - e, y, CamChunkZ + r);
    //
    //                 ChunkQueue.emplace(CamChunkX + r, y, CamChunkZ + e);
    //                 ChunkQueue.emplace(CamChunkX + r, y, CamChunkZ - e);
    //
    //                 ChunkQueue.emplace(CamChunkX + e, y, CamChunkZ - r);
    //                 ChunkQueue.emplace(CamChunkX - e, y, CamChunkZ - r);
    //
    //                 ChunkQueue.emplace(CamChunkX - r, y, CamChunkZ + e);
    //                 ChunkQueue.emplace(CamChunkX - r, y, CamChunkZ - e);
    //
    //                 if (y > 0)
    //                 {
    //                     ChunkQueue.emplace(CamChunkX + e, -y, CamChunkZ + r);
    //                     ChunkQueue.emplace(CamChunkX - e, -y, CamChunkZ + r);
    //
    //                     ChunkQueue.emplace(CamChunkX + r, -y, CamChunkZ + e);
    //                     ChunkQueue.emplace(CamChunkX + r, -y, CamChunkZ - e);
    //
    //                     ChunkQueue.emplace(CamChunkX + e, -y, CamChunkZ - r);
    //                     ChunkQueue.emplace(CamChunkX - e, -y, CamChunkZ - r);
    //
    //                     ChunkQueue.emplace(CamChunkX - r, -y, CamChunkZ + e);
    //                     ChunkQueue.emplace(CamChunkX - r, -y, CamChunkZ - e);
    //                 }
    //             }
    //         }
    //
    //         // Add corners
    //         for (int y = 0; y <= RenderHeight; y++)
    //         {
    //             ChunkQueue.emplace(CamChunkX + r, y, CamChunkZ + r);
    //             ChunkQueue.emplace(CamChunkX + r, y, CamChunkZ - r);
    //             ChunkQueue.emplace(CamChunkX - r, y, CamChunkZ + r);
    //             ChunkQueue.emplace(CamChunkX - r, y, CamChunkZ - r);
    //
    //             if (y > 0)
    //             {
    //                 ChunkQueue.emplace(CamChunkX + r, -y, CamChunkZ + r);
    //                 ChunkQueue.emplace(CamChunkX + r, -y, CamChunkZ - r);
    //                 ChunkQueue.emplace(CamChunkX - r, -y, CamChunkZ + r);
    //                 ChunkQueue.emplace(CamChunkX - r, -y, CamChunkZ - r);
    //             }
    //         }
    //     }
    // }
    // else if (ChunksLoading == 0 && !ChunkQueue.empty())
    // {
    //     // The queue is not empty. Process front item in queue
    //     glm::vec3 next = ChunkQueue.front();
    //     ChunkQueue.pop();
    //
    //     std::tuple<int, int, int> chunkTuple{next.x, next.y, next.z};
    //
    //     if (!Chunks.contains(TupleToKey(chunkTuple)))
    //     {
    //         Chunks.try_emplace(TupleToKey(chunkTuple), new Chunk(ChunkSize, next));
    //     }
    // }
    //
    // ChunksLoading = 0;
    // NumChunks = 0;
    // NumChunksRendered = 0;
    // for (auto it = Chunks.begin(); it != Chunks.end();)
    // {
    //     NumChunks++;
    //
    //     if (!it->second->ready)
    //         ChunksLoading++;
    //
    //     int ChunkX = it->second->chunkPos.x;
    //     int ChunkY = it->second->chunkPos.y;
    //     int ChunkZ = it->second->chunkPos.z;
    //     if (it->second->ready && (abs(ChunkX - CamChunkX) > RenderDistance ||
    //         abs(ChunkY - CamChunkY) > RenderDistance ||
    //         abs(ChunkZ - CamChunkZ) > RenderDistance))
    //     {
    //         it->second->~Chunk();
    //         it = Chunks.erase(it);
    //     }
    //     else
    //     {
    //         NumChunksRendered++;
    //         it->second->Render(ModelLoc);
    //         ++it;
    //     }
    // }

    return;
}

void JChunkGenerationSubsystem::TearDown()
{
    JWorldSubsystem::TearDown();
}

void JChunkGenerationSubsystem::UpdateChunkQueue()
{
    const float CamX = this->GetWorld()->MainCamera->Position.x;
    const float CamY = this->GetWorld()->MainCamera->Position.z;

    const int32 CurrentCamX = static_cast<int32>(CamX < 0 ? floor(CamX / static_cast<float>(ChunkSize)) : CamX / static_cast<float>(ChunkSize));
    const int32 CurrentCamY = static_cast<int32>(CamY < 0 ? floor(CamY / static_cast<float>(ChunkSize)) : CamY / static_cast<float>(ChunkSize));

    if (CurrentCamX == LastCamX && CurrentCamY == LastCamY)
    {
        return;
    }

    std::cout << "Camera moved to new chunk (" << CurrentCamX << ", " << CurrentCamY << ")" << '\n';
    std::cout << "Precious loc (" << this->GetWorld()->MainCamera->Position.x << ", " << this->GetWorld()->MainCamera->Position.z << ")" << '\n';

    std::cout.flush();

    LastCamX = CurrentCamX;
    LastCamY = CurrentCamY;

    ChunkQueue = { };
    const TIntVector2<int32> Center(CurrentCamX, CurrentCamY);
    std::vector<TIntVector2<int32>> VerticalChunks;
    GetAllChunksInDistance(Center, RenderDistance, VerticalChunks);

    for (const auto& Chunk : VerticalChunks)
    {
        for (int32 y = 0; y <= RenderHeight; y++)
        {
            ChunkQueue.emplace(Chunk.X, y, Chunk.Y);
        }
    }

    return;
}

void JChunkGenerationSubsystem::KillChunks()
{
    for (auto It = Chunks.begin(); It != Chunks.end();)
    {
        if (It->second->ready && (
               abs(It->second->chunkPos.x - static_cast<float>(LastCamX)) > static_cast<float>(RenderDistance)
            || abs(It->second->chunkPos.z - static_cast<float>(LastCamY)) > static_cast<float>(RenderDistance)
        ))
        {
            It->second->~Chunk();
            It = Chunks.erase(It);
        }
        else
        {
            ++It;
        }
    }

    return;
}

void JChunkGenerationSubsystem::GenerateChunks()
{
    int32 GeneratedChunks = 0;
    while (!ChunkQueue.empty() && GeneratedChunks < 2)
    {
        glm::vec3 Next = ChunkQueue.front();
        ChunkQueue.pop();

        if (!Chunks.contains(TupleToKey({Next.x, Next.y, Next.z})))
        {
            Chunks.try_emplace(TupleToKey({Next.x, Next.y, Next.z}), new Chunk(ChunkSize, Next));
            ++GeneratedChunks;
        }
    }

    return;
}

void JChunkGenerationSubsystem::RenderChunks()
{
    const uint32 ModelLoc = glGetUniformLocation(this->GetWorld()->ShaderProgram.ID, "model");

    for (const auto& Chunk : Chunks | std::views::values)
    {
        Chunk->Render(ModelLoc);
    }

    return;
}
