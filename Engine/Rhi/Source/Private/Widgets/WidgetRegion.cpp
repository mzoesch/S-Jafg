// Copyright mzoesch. All rights reserved.

#include "Widgets/WidgetRegion.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

void Jafg::WWidgetRegion::Draw(LViewport* Context) const
{
    Super::Draw(Context);

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
        this->CreateNewShaderContext();
    }



    return;
}

void Jafg::WWidgetRegion::CreateNewShaderContext() const
{
    LIntVector2 WDim = this->GetViewportSize();

    LShader& Shader = this->ShaderContext->GetShader();
    Shader = LShader("Content/Shaders/visual.vert", "Content/Shaders/visual.frag");
    Shader.Use();

    glm::mat4 projection =
        glm::ortho(0.0f, static_cast<float>(WDim.X), static_cast<float>(WDim.Y), 0.0f, -1.0f, 1.0f);


    return;
}


