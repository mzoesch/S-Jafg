// Copyright mzoesch. All rights reserved.

#include "Widgets/WidgetRegion.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

void Jafg::WWidgetRegion::Draw(LViewport* Context) const
{
    if (this->HasBrush() == false)
    {
        if (this->ShaderContext)
        {
            this->ShaderContext.Reset();
        }

        return;
    }

    if (this->ShaderContext == false)
    {
        LOG_TRACE(LogTemporal, "Creating new shader context for WWidgetRegion.")
        this->ShaderContext.MakeMeaningful();
        this->CreateNewShaderContext();
    }

    this->ShaderContext->Use();
    this->ShaderContext->Draw();
    this->ShaderContext->Unuse();

    Super::Draw(Context);

    return;
}

void Jafg::WWidgetRegion::CreateNewShaderContext() const
{
    LShader& Shader = this->ShaderContext->GetShader();
    Shader = LShader("Content/Shaders/visual.vert", "Content/Shaders/visual.frag");
    Shader.Use();

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
         0.5f, -0.5f, 0.0f, // right
         0.0f,  0.5f, 0.0f  // top
    };

    this->ShaderContext->GenerateArrayBuffers();
    this->ShaderContext->UpdateStaticArrayBuffers(vertices, sizeof(vertices));

    return;
}
