// Copyright mzoesch. All rights reserved.

#include "RhiFramework/SimpleShaderContext.h"
#include <glad/glad.h> /* Include glad to get all the required OpenGL headers. */
#include <GLFW/glfw3.h> /* Include glfw3 after glad to avoid include order issues. */

void Jafg::LSimpleShaderContext::Use()
{
    this->Shader.Use();
    glBindVertexArray(this->Vao);

    return;
}

void Jafg::LSimpleShaderContext::Draw()
{
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Jafg::LSimpleShaderContext::Unuse()
{
    glBindVertexArray(0);
}

void Jafg::LSimpleShaderContext::GenerateArrayBuffers()
{
    glGenVertexArrays(1, &this->Vao);
    glGenBuffers(1, &this->Vbo);

    return;
}

void Jafg::LSimpleShaderContext::UpdateStaticArrayBuffers(const float Vertices[], const uint32 Size)
{
    glBindVertexArray(this->Vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
    glBufferData(GL_ARRAY_BUFFER, Size, Vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    return;
}
