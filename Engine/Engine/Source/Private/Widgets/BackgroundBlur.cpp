// Copyright mzoesch. All rights reserved.

#include "Widgets/BackgroundBlur.h"
#include "Rhi/BlurShaderContext.h"
#include "Widgets/Viewport.h"

namespace
{

Jafg::LBlurShaderContext BlurShader;

} /* ~Namespace <Anonymous> */

void Jafg::WBackgroundBlur::Construct()
{
    WNode::Construct();

    if (BlurShader.IsMeaningful() == false)
    {
        BlurShader.Make();
    }

    return;
}

void Jafg::WBackgroundBlur::Draw(LViewport& Context) const
{
    if (Maths::IsNearlyEqual(this->Strength, 0.0f))
    {
        Super::Draw(Context);
        return;
    }

    LBlurShaderContextDrawArgs Args;
    Args.Size = this->GetAnchoredSize_v2();
    Args.TopLeft = this->GetAnchoredTopLeftFromMostOuter(Context);
    Args.Strength = this->Strength;
    BlurShader.Draw(Context, Args);

    Super::Draw(Context);

    return;
}
