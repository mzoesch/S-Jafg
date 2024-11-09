// Copyright mzoesch. All rights reserved.

#include "RhiFramework/ChunkShaderContext.h"
#include "RhiFramework/Shader.h"
#include "Forward/EngineForward.h"
#include <glad/glad.h>  /* Include glad to get all the required OpenGL headers. */
#include <GLFW/glfw3.h> /* Include glfw3 after glad to avoid include order issues. */
#include <stb_image.h>

#include "Widgets/Viewport.h"

void Jafg::LChunkShaderContext::Make()
{
    LGenericShaderContext::Make();

    check( this->Program == nullptr )

    this->Program = new LShader("Content/Shaders/Chunk.vert", "Content/Shaders/Chunk.frag");
    this->Program->Use();
    this->Program->SetFloatUniform("texMultiplier", 0.5f);

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

    if (this->Program)
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

    return;
}
