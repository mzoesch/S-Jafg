// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Renderer.h"

namespace Glfw
{

void GlClearError()
{
    while (glGetError() != GL_NO_ERROR) { }
}

void GlCheckError()
{
    while (GLenum Err = glGetError() != GL_NO_ERROR)
    {
        LOG_ERROR(LogCore, "OpenGL Error: {}", Err)
        checkNoEntry()
    }

    return;
}

}

void Renderer::Draw(const VertexArray& Va, const IndexBuffer& Ib, const Shader& Sh) const
{

    Sh.Bind();
    Va.Bind();
    Ib.Bind();

    GL_CALL( glDrawElements(GL_TRIANGLES, Ib.GetCount(), GL_UNSIGNED_INT, nullptr) )

    return;
}

void Renderer::Clear() const
{
    GL_CALL( glClear(GL_COLOR_BUFFER_BIT) )
}
