// Copyright mzoesch. All rights reserved.

#include "MyWorld/Chunk/ChunkGenerationSubsystem.h"
#include "Engine/World.h"
#include "Engine/Framework/Camera.h"

void JChunkGenerationSubsystem::Initialize(LSubsystemCollection& Collection)
{
    JWorldSubsystem::Initialize(Collection);
}

void JChunkGenerationSubsystem::Tick(const float DeltaTime)
{
    const uint32 ModelLoc = glGetUniformLocation(this->GetWorld()->ShaderProgram.ID, "model");

    const float CamX = this->GetWorld()->MainCamera->Position.x;
    const float CamY = this->GetWorld()->MainCamera->Position.y;
    const float CamZ = this->GetWorld()->MainCamera->Position.z;

    int CamChunkX = CamX < 0 ? floor(CamX / ChunkSize) : CamX / ChunkSize;
    int CamChunkY = CamY < 0 ? floor(CamY / ChunkSize) : CamY / ChunkSize;
    int CamChunkZ = CamZ < 0 ? floor(CamZ / ChunkSize) : CamZ / ChunkSize;

    // Check if the camera moved to new chunk
    if (CamChunkX != LastCamX || CamChunkY != LastCamY || CamChunkZ != LastCamZ)
    {
        // Player moved chunks, start new chunk queue
        LastCamX = CamChunkX;
        LastCamY = CamChunkY;
        LastCamZ = CamChunkZ;

        // Current chunk
        ChunkQueue = {};
        if (!Chunks.contains(TupleToKey({CamChunkX, CamChunkY, CamChunkZ})))
            ChunkQueue.emplace(CamChunkX, CamChunkY, CamChunkZ);

        for (int r = 0; r < RenderDistance; r++)
        {
            // Add middle chunks
            for (int y = 0; y <= RenderHeight; y++)
            {
                ChunkQueue.emplace(CamChunkX, y, CamChunkZ + r);
                ChunkQueue.emplace(CamChunkX + r, y, CamChunkZ);
                ChunkQueue.emplace(CamChunkX, y, CamChunkZ - r);
                ChunkQueue.emplace(CamChunkX - r, y, CamChunkZ);

                if (y > 0)
                {
                    ChunkQueue.emplace(CamChunkX, -y, CamChunkZ + r);
                    ChunkQueue.emplace(CamChunkX + r, -y, CamChunkZ);
                    ChunkQueue.emplace(CamChunkX, -y, CamChunkZ - r);
                    ChunkQueue.emplace(CamChunkX - r, -y, CamChunkZ);
                }
            }

            // Add edges
            for (int e = 1; e < r; e++)
            {
                for (int y = 0; y <= RenderHeight; y++)
                {
                    ChunkQueue.emplace(CamChunkX + e, y, CamChunkZ + r);
                    ChunkQueue.emplace(CamChunkX - e, y, CamChunkZ + r);

                    ChunkQueue.emplace(CamChunkX + r, y, CamChunkZ + e);
                    ChunkQueue.emplace(CamChunkX + r, y, CamChunkZ - e);

                    ChunkQueue.emplace(CamChunkX + e, y, CamChunkZ - r);
                    ChunkQueue.emplace(CamChunkX - e, y, CamChunkZ - r);

                    ChunkQueue.emplace(CamChunkX - r, y, CamChunkZ + e);
                    ChunkQueue.emplace(CamChunkX - r, y, CamChunkZ - e);

                    if (y > 0)
                    {
                        ChunkQueue.emplace(CamChunkX + e, -y, CamChunkZ + r);
                        ChunkQueue.emplace(CamChunkX - e, -y, CamChunkZ + r);

                        ChunkQueue.emplace(CamChunkX + r, -y, CamChunkZ + e);
                        ChunkQueue.emplace(CamChunkX + r, -y, CamChunkZ - e);

                        ChunkQueue.emplace(CamChunkX + e, -y, CamChunkZ - r);
                        ChunkQueue.emplace(CamChunkX - e, -y, CamChunkZ - r);

                        ChunkQueue.emplace(CamChunkX - r, -y, CamChunkZ + e);
                        ChunkQueue.emplace(CamChunkX - r, -y, CamChunkZ - e);
                    }
                }
            }

            // Add corners
            for (int y = 0; y <= RenderHeight; y++)
            {
                ChunkQueue.emplace(CamChunkX + r, y, CamChunkZ + r);
                ChunkQueue.emplace(CamChunkX + r, y, CamChunkZ - r);
                ChunkQueue.emplace(CamChunkX - r, y, CamChunkZ + r);
                ChunkQueue.emplace(CamChunkX - r, y, CamChunkZ - r);

                if (y > 0)
                {
                    ChunkQueue.emplace(CamChunkX + r, -y, CamChunkZ + r);
                    ChunkQueue.emplace(CamChunkX + r, -y, CamChunkZ - r);
                    ChunkQueue.emplace(CamChunkX - r, -y, CamChunkZ + r);
                    ChunkQueue.emplace(CamChunkX - r, -y, CamChunkZ - r);
                }
            }
        }
    }
    else if (ChunksLoading == 0 && !ChunkQueue.empty())
    {
        // The queue is not empty. Process front item in queue
        glm::vec3 next = ChunkQueue.front();
        ChunkQueue.pop();

        std::tuple<int, int, int> chunkTuple{next.x, next.y, next.z};

        if (!Chunks.contains(TupleToKey(chunkTuple)))
        {
            Chunks.try_emplace(TupleToKey(chunkTuple), new Chunk(ChunkSize, next));
        }
    }

    ChunksLoading = 0;
    NumChunks = 0;
    NumChunksRendered = 0;
    for (auto it = Chunks.begin(); it != Chunks.end();)
    {
        NumChunks++;

        if (!it->second->ready)
            ChunksLoading++;

        int ChunkX = it->second->chunkPos.x;
        int ChunkY = it->second->chunkPos.y;
        int ChunkZ = it->second->chunkPos.z;
        if (it->second->ready && (abs(ChunkX - CamChunkX) > RenderDistance ||
            abs(ChunkY - CamChunkY) > RenderDistance ||
            abs(ChunkZ - CamChunkZ) > RenderDistance))
        {
            it->second->~Chunk();
            it = Chunks.erase(it);
        }
        else
        {
            NumChunksRendered++;
            it->second->Render(ModelLoc);
            ++it;
        }
    }

    return;
}

void JChunkGenerationSubsystem::TearDown()
{
    JWorldSubsystem::TearDown();
}
