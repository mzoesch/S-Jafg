// Copyright mzoesch. All rights reserved.

#include "RhiFramework/ChunkShaderContext.h"
#include "RhiFramework/Shader.h"
#include "Forward/EngineForward.h"
#include "Widgets/Viewport.h"
#include <glad/glad.h>  /* Include glad to get all the required OpenGL headers. */
#include <GLFW/glfw3.h> /* Include glfw3 after glad to avoid include order issues. */
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/ObjectBaseUtility.h"
#include "User/UserPreferences.h"

void Jafg::LChunkShaderContext::Make()
{
    LGenericShaderContext::Make();

    check( this->Program == nullptr )

    this->Program = new LShader("Content/Shaders/Chunk.vert", "Content/Shaders/Chunk.frag");
    this->Program->Use();

    glGenTextures(1, &this->Texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(true);
    LStringLegacy ExecPath(PLATFORM_MAX_PATH, '\0');
    GetModuleFileNameA(nullptr, ExecPath.data(), static_cast<uint32>(ExecPath.size()));
    ExecPath = ExecPath.substr(0, ExecPath.find_last_of('\\'));
    LStringLegacy TexPath = ExecPath + '/' + "Content/Textures/Map.png";

    int32 Width;
    int32 Height;
    int32 NrChannels;
    uint8* Data = stbi_load(TexPath.data(), &Width, &Height, &NrChannels, 0);
    if (Data == nullptr)
    {
        JAFG_ENGINE_FORWARD_REQUEST_EXIT(EPlatformExit::Fatal, "Failed to load texture.")
        return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(Data);

    return;
}

void Jafg::LChunkShaderContext::OnFree()
{
    LGenericShaderContext::OnFree();

    if (ensure(this->Program))
    {
        this->Program->Free();
        delete this->Program;
        this->Program = nullptr;
    }

    return;
}

void Jafg::LChunkShaderContext::Draw(const LViewport& Context, LGenericShaderContextDrawArgs& InArgs) const
{
    GENERIC_SHADER_DRAW_BODY(LChunkShaderDrawArgs)

    if (const JUserPreferences* Preferences = GetDefault<JUserPreferences>();
        Preferences->GetPolygonMode() == EPolygonMode::Fill)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CW);
    }
    else if (Preferences->GetPolygonMode() == EPolygonMode::Wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CW);
    }
    else
    {
        panic( "Encountered unknown polygon mode." )
    }

    checkSlow( this->Program )
    this->Program->Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->Texture);

    const TMatrix Projection = Maths::MakePerspectiveProjectionMatrix(
        Maths::ToRadians(Args.DegYFov),
        static_cast<float>(Context.GetDimensions().X) / static_cast<float>(Context.GetDimensions().Y),
        0.1f, 2000.0f
    );

    this->Program->SetMatrix4Uniform("view", Args.ViewMatrix);
    this->Program->SetMatrix4Uniform("projection", Projection);

    const int32 ModelLocation = glGetUniformLocation(this->Program->GetId(), "model");

    glBindVertexArray(Args.Instance->GetVertexArrayObject());

    glm::vec3 worldPosVec = glm::vec3(Args.WorldLocation.X, Args.WorldLocation.Y, Args.WorldLocation.Z);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, worldPosVec);
    glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, Args.NumTriangles, GL_UNSIGNED_INT, 0);

    return;
}

Jafg::LChunkShaderInstance::~LChunkShaderInstance()
{
    glDeleteBuffers(1, &this->Vbo);
    glDeleteBuffers(1, &this->Ebo);
    glDeleteVertexArrays(1, &this->Vao);

    return;
}

void Jafg::LChunkShaderInstance::LoadMeshToGraphicsMemory(const TdhArray<ChunkBoxVertex>& Vertices, const TdhArray<uint32>& Indices)
{
    static_assert(sizeof(GLsizeiptr) == sizeof(int64), "GLsizeiptr is not 64 bits");

    glGenVertexArrays(1, &this->Vao);
    glBindVertexArray(this->Vao);

    glGenBuffers(1, &this->Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
    glBufferData(GL_ARRAY_BUFFER,
        Vertices.GetSize() * static_cast<int64>(sizeof(::Jafg::ChunkBoxVertex)), Vertices.GetData(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(ChunkBoxVertex), reinterpret_cast<void*>(offsetof(ChunkBoxVertex, LocationX)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, sizeof(ChunkBoxVertex), reinterpret_cast<void*>(offsetof(ChunkBoxVertex, TextureGridX)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &this->Ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->Ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        Indices.GetSize() * static_cast<int64>(sizeof(uint32)), Indices.GetData(), GL_STATIC_DRAW);

    return;
}
