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
    WWidgetNode::Construct();

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
    Args.Size = this->GetAnchoredSize();
    Args.TopLeft = this->GetAnchoredTopLeftFromMostOuter(Context);
    Args.Strength = this->Strength;
    BlurShader.Draw(Context, Args);

    Super::Draw(Context);

    return;
}

Jafg::LReply Jafg::WBackgroundBlur::OnKeyDown(LKeyEvent& InKeyEvent)
{
    LOG_WARNING(LogTemporal, "A")
    return WWidgetNode::OnKeyDown(InKeyEvent);
}

void Jafg::WBackgroundBlur::OnFocusReceived()
{
    Super::OnFocusReceived();
    LOG_WARNING(LogTemporal, "A")
}
