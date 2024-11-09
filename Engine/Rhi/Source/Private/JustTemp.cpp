// Copyright mzoesch. All rights reserved.

#include "JustTemp.h"
#include "glad/glad.h" /* Include glad to get all the required OpenGL headers. */
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>
#include "RhiFramework/Shader.h"

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

void JustTemp::G(uint32* vertexArrayObject, uint32* numTriangles, Jafg::LVector* worldPos, uint32* modelLoc)
{
    glBindVertexArray(*vertexArrayObject);

    glm::vec3 worldPosVec = glm::vec3(worldPos->X, worldPos->Y, worldPos->Z);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, worldPosVec);
    glUniformMatrix4fv(static_cast<GLint>(*modelLoc), 1, GL_FALSE, glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(*numTriangles), GL_UNSIGNED_INT, 0);
}
