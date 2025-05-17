// Copyright mzoesch. All rights reserved.

#include "Widgets/Region.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "Rhi/OrthographicBoxShader.h"

Jafg::WRegion::WRegion(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    Tasks::Make(ENamedThreads::Master, ETaskTime::BeforeEngineInitButAfterAlloc, [](void) -> void
    {
        if (GEngine->IsShaderValid(Name_ShaderOrthographicBox) == false)
        {
            LOrthographicBoxShader* Shader = new LOrthographicBoxShader();
            Shader->MakeChecked(Name_ShaderOrthographicBox);
        }

        return;
    });

    return;
}

void Jafg::WRegion::Draw(LViewport& Context) const
{
    if (this->HasBrush() == false)
    {
        Super::Draw(Context);
        return;
    }

    GEngine->GetShaderChecked<LOrthographicBoxShader>(Name_ShaderOrthographicBox)->Draw
    (
        Context,
        this->GetAnchoredSize(),
        this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
        this->GetBrush().Tint
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
