#include "CoreAFX.h"
#include "MyWorld/WorldGen.h"
#include <OpenSimplexNoise.hh>
#include "MyWorld/Blocks.h"
#include "MyWorld/Chunk.h"

void WorldGen::GenerateChunkData(int chunkX, int chunkY, int chunkZ, int chunkSize, uint32*& chunkData)
{
    const OSN::Noise<2> SurfaceNoise;
    const OSN::Noise<3> CaveNoise;
    const int ChunkLocX = chunkX * chunkSize;
    const int ChunkLocY = chunkY * chunkSize;
    const int ChunkLocZ = chunkZ * chunkSize;

    for (int X = 0; X < chunkSize; X++)
    {
        for (int Y = 0; Y < chunkSize; Y++)
        {
            const int32 Noise2D =
                static_cast<int32>(SurfaceNoise.eval(
                    static_cast<float>(X + ChunkLocX) * .1f,
                    static_cast<float>(Y + ChunkLocY) * .1f
                ) * 3.0f) + 20;

            for (int Z = 0; Z < chunkSize; Z++)
            {
                //chunkData.push_back(0);                              // Empty
                //chunkData.push_back(1);                              // Full
                //chunkData.push_back(((x + y + z) % 2 == 0) ? 1 : 0); // Checkerboard
                /*chunkData.push_back(                                   // Edges
                    (x == 0 || x == chunkSize - 1) ||
                    (y == 0 || y == chunkSize - 1) ||
                    (z == 0 || z == chunkSize - 1)
                    ? 1 : 0
                );*/

                const float NoiseCaves = CaveNoise.eval(
                    static_cast<float>(X + ChunkLocX) * .1f,
                    static_cast<float>(Y + ChunkLocY) * .1f,
                    static_cast<float>(Z + ChunkLocZ) * .1f
                );

                if (Z + ChunkLocZ < Noise2D)
                {
                    chunkData[AChunk::GetIndex(X, Y, Z)] = Blocks::GRASS_BLOCK;
                }
                else
                {
                    chunkData[AChunk::GetIndex(X, Y, Z)] = 0;
                }
                //
                // if (Z + ChunkLocZ > Noise2D || NoiseCaves > .5f)
                // {
                //     chunkData[AChunk::GetIndex(X, Y, Z)] = 0;
                // }
                // else if (Z + ChunkLocZ == Noise2D)
                // {
                //     chunkData[AChunk::GetIndex(X, Y, Z)] = Blocks::GRASS_BLOCK;
                // }
                // else if (Z + ChunkLocZ > 10)
                // {
                //     chunkData[AChunk::GetIndex(X, Y, Z)] = Blocks::DIRT_BLOCK;
                // }
                // else
                // {
                //     chunkData[AChunk::GetIndex(X, Y, Z)] = Blocks::STONE_BLOCK;
                // }
            }
        }
    }
}
