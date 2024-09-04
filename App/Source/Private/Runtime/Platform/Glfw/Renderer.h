// Copyright mzoesch. All rights reserved.

#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Glfw
{

#define GL_CALL(stmt) Glfw::GlClearError(); stmt; Glfw::GlCheckError();

void GlClearError();
void GlCheckError();

}

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"

class Renderer
{
public:

    void Draw(const VertexArray& Va, const IndexBuffer& Ib, const Shader& Sh) const;
    void Clear() const;
};
