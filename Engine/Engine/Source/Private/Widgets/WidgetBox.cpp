// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/WidgetBox.h"

void Jafg::WWidgetBox::Draw(LViewport& Context) const
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
        LOG_TRACE(LogWidgets, "Creating new shader context for WWidgetBox.")
        this->ShaderContext.MakeMeaningful();
        this->CreateNewShaderContext();
    }

    this->ShaderContext->Draw(
        Context,
        this->GetAnchoredSize(),
        this->GetAnchoredTopLeftFromMostOuter(Context, this),
        this->Brush.GetValue().Tint,
        this->Brush.GetValue().Image.GetTexture()
    );

    Super::Draw(Context);

    return;
}

void Jafg::WWidgetBox::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();
    this->SetDesiredSize(this->Padding.GetDesiredSize());

    return;
}

void Jafg::WWidgetBox::SetTint(const LColor& InTint)
{
    if (this->HasBrush())
    {
        this->Brush.GetValue().Tint = InTint;
        return;
    }

    this->SetBrush(LBoxBrush({.Tint = InTint}));

    return;
}

void Jafg::WWidgetBox::SetTexture(const LTexture2* InTexture)
{
    if (this->HasBrush())
    {
        this->Brush.GetValue().Image.SetTexture(InTexture);
        return;
    }

    this->SetBrush(LBoxBrush({.Image = LImage().SetTexture(InTexture)}));
    return;
}

void Jafg::WWidgetBox::SetImage(const LImage& InImage)
{
    if (this->HasBrush())
    {
        this->Brush.GetValue().Image = InImage;
        return;
    }

    this->SetBrush(LBoxBrush({.Image = InImage}));
    return;
}

void Jafg::WWidgetBox::CreateNewShaderContext() const
{
    this->ShaderContext->Make();
}
