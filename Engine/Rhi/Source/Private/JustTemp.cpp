// Copyright mzoesch. All rights reserved.

#include "JustTemp.h"
#include "glad/glad.h" /* Include glad to get all the required OpenGL headers. */
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>

#include "Forward/EngineForward.h"
#include "RhiFramework/Shader.h"

void JustTemp::A(uint32* Texture)
{
    glGenTextures(1, Texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_set_flip_vertically_on_load(true);

    LStringLegacy ExecPath(PLATFORM_MAX_PATH, '\0');
    GetModuleFileNameA(nullptr, ExecPath.data(), static_cast<uint32>(ExecPath.size()));
    ExecPath = ExecPath.substr(0, ExecPath.find_last_of('\\'));

    LStringLegacy TexPath = ExecPath + '/' + "Content/Textures/Map.png";

    int32 Width, Height, NrChannels;
    uint8* Data = stbi_load(&TexPath[0], &Width, &Height, &NrChannels, 0);
    if (Data == nullptr)
    {
        JAFG_ENGINE_FORWARD_REQUEST_EXIT(EPlatformExit::Fatal, "Failed to load texture.");
        return;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(Data);
}

void JustTemp::B(uint32 Texture)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
}

void JustTemp::C(float Zoom, Jafg::LShader* ShaderProgram, Jafg::LIntVector2 WindowDimensions, const Jafg::LMatrix& View)
{
    Jafg::TMatrix Proj = Jafg::Maths::MakePerspectiveProjectionMatrix(
        Jafg::Maths::ToRadians(Zoom),
        static_cast<float>(WindowDimensions.X) / static_cast<float>(WindowDimensions.Y),
        0.1f, 2000.0f
    );

    const int32 ViewLoc = glGetUniformLocation(ShaderProgram->GetId(), "view");
    const int32 ProjectionLoc = glGetUniformLocation(ShaderProgram->GetId(), "projection");

    glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, View.GetData());
    glUniformMatrix4fv(ProjectionLoc, 1, GL_FALSE, Proj.GetData());

    return;
}

uint32 JustTemp::D(Jafg::LShader* ShaderProgram)
{
    return glGetUniformLocation(ShaderProgram->GetId(), "model");
}

void JustTemp::E(uint32* vertexArrayObject, uint32* vbo, uint32* ebo)
{
    glDeleteBuffers(1, vbo);
    glDeleteBuffers(1, ebo);
    glDeleteVertexArrays(1, vertexArrayObject);
}

void JustTemp::F(uint32* vertexArrayObject, uint32* vbo, uint32* ebo, Jafg::TdhArray<Jafg::Vertex>* vertices,
    Jafg::TdhArray<uint32>* indices, uint32* numTriangles)
{
    using namespace Jafg;
    *numTriangles = static_cast<unsigned int>( indices->GetSize() );

    glGenVertexArrays(1, vertexArrayObject);
    glBindVertexArray(*vertexArrayObject);

    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices->GetSize() * sizeof(Jafg::Vertex)), vertices->GetData(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, posX)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texGridX)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices->GetSize() * sizeof(unsigned int)), indices->GetData(),
                 GL_STATIC_DRAW);
}

void JustTemp::G(uint32* vertexArrayObject, uint32* numTriangles, glm::vec3* worldPos, uint32* modelLoc)
{
    glBindVertexArray(*vertexArrayObject);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, *worldPos);
    glUniformMatrix4fv(static_cast<GLint>(*modelLoc), 1, GL_FALSE, glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(*numTriangles), GL_UNSIGNED_INT, 0);
}
