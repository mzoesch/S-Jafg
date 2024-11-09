// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/Actor.h"
#include "Engine/Components/RenderComponent.h"
#include "MyWorld/Chunk/ChunkKey.h"
#include <glm/glm.hpp>
#include "MyWorld/WorldStatics.h"
#include "RhiFramework/Shader.h"
#include "JustTemp.h"
#include "Chunk.generated.h"

namespace Jafg
{

class AChunk;
class JChunkGenerationSubsystem;
class LChunkMesher;

class ENGINE_API LChunkRendererComponent final : public LRendererComponent
{
public:

    LChunkRendererComponent() = delete;
    explicit LChunkRendererComponent(AChunk& Owner);
    virtual ~LChunkRendererComponent() override = default;

    virtual void Draw(const LViewport& Context) override;

private:

    AChunk* Owner = nullptr;
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

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(AChunk)

public:

    virtual void BeginLife() override;
    virtual void EndLife() override;

    void GenerateChunk();
    void Render(unsigned int ModelLoc);

    uint32* RawVoxelData = nullptr;
    glm::vec3 ChunkPos = glm::vec3(0.0f);

    LSharedChunkArgs* SharedArgs = nullptr;
    LChunkKey  ChunkKey      = { };
    LVector    WorldLocation = { };

    bool bReady = false;
    bool bGenerated = false;

    FORCEINLINE static int32 GetIndex(const int32 X, const int32 Y, const int32 Z)
    {
        return X + Y * WorldStatics::ChunkSize + Z * WorldStatics::ChunkSize * WorldStatics::ChunkSize;
    }

private:

    unsigned int VertexArrayObject = 0;
    unsigned int Vbo = 0;
    unsigned int Ebo = 0;
    unsigned int NumTriangles= 0;

    TdhArray<Jafg::Vertex> Vertices = {};
    TdhArray<uint32> Indices = {};
    LChunkMesher* Mesher = nullptr;
};

} /* ~Namespace Jafg */
