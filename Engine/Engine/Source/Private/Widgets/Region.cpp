// Copyright mzoesch. All rights reserved.

#include "Widgets/Region.h"

void Jafg::WRegion::Draw(LViewport& Context) const
{
    if (this->HasBrush() == false)
    {
        if (this->ShaderContext)
        {
            this->ShaderContext.Reset();
        }

        Super::Draw(Context);

        return;
    }

    if (this->ShaderContext == false)
    {
        LOG_TRACE(LogWidgets, "Creating new shader context for WWidgetRegion.")
        this->ShaderContext.MakeMeaningful();
        this->CreateNewShaderContext();
    }

    this->ShaderContext->Draw(
        Context,
        this->GetAnchoredSize(),
        this->GetAnchoredTopLeftFromMostOuter(Context),
        this->GetBrush().Tint,
        this->GetBrush().Image.GetTexture()
    );

    Super::Draw(Context);

    return;
}

void Jafg::WRegion::SetTint(const LColor& InTint)
{
    if (this->HasBrush())
    {
        this->Brush.GetValue().Tint = InTint;
        return;
    }

    this->SetBrush(LRegionBrush({.Tint = InTint}));
    return;
}

void Jafg::WRegion::SetTexture(const LTexture2* InTexture)
{
    if (this->HasBrush())
    {
        this->Brush.GetValue().Image.SetTexture(InTexture);
        return;
    }

    this->SetBrush(LRegionBrush({.Image = LImage().SetTexture(InTexture)}));
    return;
}

void Jafg::WRegion::SetImage(const LImage& InImage)
{
    if (this->HasBrush())
    {
        this->Brush.GetValue().Image = InImage;
        return;
    }

    this->SetBrush(LRegionBrush({.Image = InImage}));
    return;
}

void Jafg::WRegion::CreateNewShaderContext() const
{
    this->ShaderContext->Make();
}
