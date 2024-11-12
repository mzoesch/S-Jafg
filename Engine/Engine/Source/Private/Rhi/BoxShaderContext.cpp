// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Rhi/BoxShaderContext.h"
#include <glad/glad.h>  /* Include glad to get all the required OpenGL headers. */
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
    if (this->bIsMeaningful == false)
    {
        return;
    }

    this->bIsMeaningful = false;

    glDeleteVertexArrays(1, &this->Vao);
    glDeleteBuffers(1, &this->Vbo);

    this->Shader.Free();

    return;

}

void Jafg::LBoxShaderContext::Draw(const LViewport& Context, const LVector2& Size, const LVector2& TopLeft, const LColor& Color) const
{
    this->Shader.Use();
    glBindVertexArray(this->Vao);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    const float Scale = Context.GetScaleFactor();

    const float Vertices[] =
    {
        TopLeft.X * Scale,            TopLeft.Y * Scale,            /* Top    Left  */
        (TopLeft.X + Size.X) * Scale, TopLeft.Y * Scale,            /* Top    Right */
        TopLeft.X * Scale,            (TopLeft.Y + Size.Y) * Scale, /* Bottom Left  */

        (TopLeft.X + Size.X) * Scale, TopLeft.Y * Scale,            /* Top    Right */
        (TopLeft.X + Size.X) * Scale, (TopLeft.Y + Size.Y) * Scale, /* Bottom Right */
        TopLeft.X * Scale,            (TopLeft.Y + Size.Y) * Scale  /* Bottom Left  */
    };

    const LIntVector2 WindowDimensions = Context.GetDimensions();
    const glm::mat4 Projection = glm::ortho(0.0f, static_cast<float>(WindowDimensions.X), 0.0f, static_cast<float>(WindowDimensions.Y));
    this->Shader.SetMatrix4Uniform("Projection", Projection);
    this->Shader.SetFloatUniform("OrthoZDepth", Context.GetFrameOrthoZLayerDepth());
    this->Shader.SetIntUniform("BoxColor", *reinterpret_cast<const int32*>(&Color.Bits));

    glBindBuffer(GL_ARRAY_BUFFER, this->Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);

    return;
}
