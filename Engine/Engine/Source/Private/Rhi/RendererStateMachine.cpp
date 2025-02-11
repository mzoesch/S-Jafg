// Copyright mzoesch. All rights reserved.

#include "Rhi/RendererStateMachine.h"
#include "Rhi/RhiVendorInclude.h"
#include "User/UserPreferences.h"

void Jafg::RendererStateMachine::PrepareForPerspectivePainting()
{
#if !PLATFORM_WASM
    if (const JUserPreferences* Preferences = GetDefault<JUserPreferences>())
    {
        if (Preferences->PolygonMode == EPolygonMode::Wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else if (Preferences->PolygonMode == EPolygonMode::Fill)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else
        {
            panic("Encountered unknown polygon mode.")
        }
    }
#endif /* !PLATFORM_WASM */

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);

    return;
}

void Jafg::RendererStateMachine::PrepareForOrthographicPainting()
{
#if !PLATFORM_WASM
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif /* !PLATFORM_WASM */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glDisable(GL_DEPTH_TEST);

    return;
}
