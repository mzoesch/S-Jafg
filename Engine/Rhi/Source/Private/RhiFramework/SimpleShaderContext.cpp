// Copyright mzoesch. All rights reserved.

#include "RhiFramework/SimpleShaderContext.h"
#include <glad/glad.h> /* Include glad to get all the required OpenGL headers. */
#include <GLFW/glfw3.h> /* Include glfw3 after glad to avoid include order issues. */
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "Widgets/Viewport.h"

void Jafg::LBoxShaderContext::Make()
{
    if (this->bIsMeaningful)
    {
        panic( "Tried to override a meaningful shader context." )
        return;
    }

    this->bIsMeaningful = true;

    this->Shader = LShader("Content/Shaders/VisualBox.vert", "Content/Shaders/VisualBox.frag");
    this->Shader.Use();

    glGenVertexArrays(1, &this->Vao);
    glGenBuffers(1, &this->Vbo);

    glBindVertexArray(this->Vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return;
}

void Jafg::LBoxShaderContext::Free()
{
}

void Jafg::LBoxShaderContext::Draw(const LViewport& Context, const LVector2& Size, const LVector2& TopLeft) const
{
    this->Shader.Use();
    glBindVertexArray(this->Vao);

    const float Scale = Context.GetScaleFactor();

    const float Vertices[] =
    {
        TopLeft.X * Scale,                  TopLeft.Y * Scale, /* Top left */
        (TopLeft.X + Size.X) * Scale, TopLeft.Y * Scale,      /* Top right */
        TopLeft.X * Scale, (TopLeft.Y + Size.Y) * Scale,      /* Bottom left */

        (TopLeft.X + Size.X) * Scale, TopLeft.Y * Scale,      /* Top right */
        (TopLeft.X + Size.X) * Scale, (TopLeft.Y + Size.Y) * Scale, /* Bottom right */
        TopLeft.X * Scale, (TopLeft.Y + Size.Y) * Scale       /* Bottom left */
    };

    const LIntVector2 WindowDimensions = Context.GetDimensions();
    const glm::mat4 Projection = glm::ortho(0.0f, static_cast<float>(WindowDimensions.X), 0.0f, static_cast<float>(WindowDimensions.Y));
    this->Shader.SetMatrix4Uniform("Projection", Projection);
    this->Shader.SetFloatUniform("OrthoZDepth", Context.GetFrameOrthoZLayerDepth());

    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Unbind
    glBindVertexArray(0);

    return;
}
