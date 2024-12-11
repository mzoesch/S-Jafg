// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/WidgetRegion.h"

void Jafg::WWidgetRegion::Draw(LViewport& Context) const
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
        this->GetAnchoredTopLeftFromMostOuter(Context, this),
        this->GetBrush().Tint,
        this->GetBrush().Image.GetTexture()
    );

    Super::Draw(Context);

    return;
}

void Jafg::WWidgetRegion::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    LVector2 DesiredSize = LVector2::Zero();
    for (const LWidgetSlot* ChildSlot : this->GetChildren())
    {
        DesiredSize.X = Maths::Max(DesiredSize.X, ChildSlot->Content->GetDesiredSize().X);
        DesiredSize.Y = Maths::Max(DesiredSize.Y, ChildSlot->Content->GetDesiredSize().Y);

        continue;
    }

    DesiredSize += this->GetPadding().GetDesiredSize();

    this->SetDesiredSize(DesiredSize);

    return;
}

Jafg::WWidgetRegion& Jafg::WWidgetRegion::SetTint(const LColor& InTint)
{
    if (this->HasBrush())
    {
        this->Brush.GetValue().Tint = InTint;
        return *this;
    }

    return this->SetBrush(LRegionBrush({.Tint = InTint}));
}

Jafg::WWidgetRegion& Jafg::WWidgetRegion::SetTexture(const LTexture2* InTexture)
{
    if (this->HasBrush())
    {
        this->Brush.GetValue().Image.SetTexture(InTexture);
        return *this;
    }

    return this->SetBrush(LRegionBrush({.Image = LImage().SetTexture(InTexture)}));
}

Jafg::WWidgetRegion& Jafg::WWidgetRegion::SetImage(const LImage& InImage)
{
    if (this->HasBrush())
    {
        this->Brush.GetValue().Image = InImage;
        return *this;
    }

    return this->SetBrush(LRegionBrush({.Image = InImage}));
}

void Jafg::WWidgetRegion::CreateNewShaderContext() const
{
    this->ShaderContext->Make();
}
