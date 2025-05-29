// Copyright mzoesch. All rights reserved.

#include "Widgets/Region.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "Rhi/OrthographicBoxShader.h"
#include "Rhi/OrthographicImageBoxShader.h"
#include "Rhi/OrthographicRoundedBoxShader.h"
#include "Rhi/OrthographicRoundedImageBoxShader.h"
#include "Rhi/OrthographicOutlineBoxShader.h"
#include "Rhi/OrthographicOutlineImageBoxShader.h"
#include "Rhi/OrthographicRoundedOutlineBoxShader.h"
#include "Rhi/OrthographicRoundedOutlineImageBoxShader.h"

Jafg::WRegion::WRegion(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    Tasks::Make(ENamedThreads::Master, ETaskTime::BeforeEngineInitButAfterAlloc, [](void) -> void
    {
        check( GEngine )

        if (GEngine->IsShaderValid(Name_ShaderOrthographicBox) == false)
        {
            (new LOrthographicBoxShader())->MakeChecked(Name_ShaderOrthographicBox);
        }

        if (GEngine->IsShaderValid(Name_ShaderOrthographicImageBox) == false)
        {
            (new LOrthographicImageBoxShader())->MakeChecked(Name_ShaderOrthographicImageBox);
        }

        if (GEngine->IsShaderValid(Name_ShaderOrthographicRoundedBox) == false)
        {
            (new LOrthographicRoundedBoxShader())->MakeChecked(Name_ShaderOrthographicRoundedBox);
        }

        if (GEngine->IsShaderValid(Name_ShaderOrthographicRoundedImageBox) == false)
        {
            (new LOrthographicRoundedImageBoxShader())->MakeChecked(Name_ShaderOrthographicRoundedImageBox);
        }

        if (GEngine->IsShaderValid(Name_ShaderOrthographicOutlineBox) == false)
        {
            (new LOrthographicOutlineBoxShader())->MakeChecked(Name_ShaderOrthographicOutlineBox);
        }

        if (GEngine->IsShaderValid(Name_ShaderOrthographicOutlineImageBox) == false)
        {
            (new LOrthographicOutlineImageBoxShader())->MakeChecked(Name_ShaderOrthographicOutlineImageBox);
        }


        if (GEngine->IsShaderValid(Name_ShaderOrthographicRoundedOutlineBox) == false)
        {
            (new LOrthographicRoundedOutlineBoxShader())->MakeChecked(Name_ShaderOrthographicRoundedOutlineBox);
        }

        if (GEngine->IsShaderValid(Name_ShaderOrthographicRoundedOutlineImageBox) == false)
        {
            (new LOrthographicRoundedOutlineImageBoxShader())->MakeChecked(Name_ShaderOrthographicRoundedOutlineImageBox);
        }

        return;
    });

    return;
}

void Jafg::WRegion::Draw(LViewport& Context) const
{
    if (this->Brush.Type != ERegionBrush::None)
    {
        if (this->Brush.Type == ERegionBrush::Box)
        {
            if (this->Brush.Image.IsTextureValid())
            {
                GEngine->GetShaderChecked<LOrthographicImageBoxShader>(Name_ShaderOrthographicImageBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->GetBrush().Tint,
                    this->GetBrush().Image
                );
            }
            else
            {
                GEngine->GetShaderChecked<LOrthographicBoxShader>(Name_ShaderOrthographicBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->GetBrush().Tint
                );
            }
        }

        else if (this->Brush.Type == ERegionBrush::RoundedBox)
        {
            if (this->Brush.Image.IsTextureValid())
            {
                GEngine->GetShaderChecked<LOrthographicRoundedImageBoxShader>(Name_ShaderOrthographicRoundedImageBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->GetBrush().Tint,
                    this->GetBrush().Radii,
                    this->GetBrush().Image
                );
            }
            else
            {
                GEngine->GetShaderChecked<LOrthographicRoundedBoxShader>(Name_ShaderOrthographicRoundedBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->GetBrush().Tint,
                    this->GetBrush().Radii
                );
            }
        }

        else if (this->Brush.Type == ERegionBrush::OutlineBox)
        {
            if (this->Brush.Image.IsTextureValid())
            {
                GEngine->GetShaderChecked<LOrthographicOutlineImageBoxShader>(Name_ShaderOrthographicOutlineImageBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->GetBrush().Tint,
                    this->GetBrush().OutlineThickness,
                    this->GetBrush().OutlineTint,
                    this->GetBrush().Image
                );
            }
            else
            {
                GEngine->GetShaderChecked<LOrthographicOutlineBoxShader>(Name_ShaderOrthographicOutlineBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->GetBrush().Tint,
                    this->GetBrush().OutlineThickness,
                    this->GetBrush().OutlineTint
                );
            }
        }

        else if (this->Brush.Type == ERegionBrush::RoundedOutlineBox)
        {
            if (this->Brush.Image.IsTextureValid())
            {
                GEngine->GetShaderChecked<LOrthographicRoundedOutlineImageBoxShader>(Name_ShaderOrthographicRoundedOutlineImageBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->GetBrush().Tint,
                    this->GetBrush().OutlineThickness,
                    this->GetBrush().OutlineTint,
                    this->GetBrush().Radii,
                    this->GetBrush().Image
                );
            }
            else
            {
                GEngine->GetShaderChecked<LOrthographicRoundedOutlineBoxShader>(Name_ShaderOrthographicRoundedOutlineBox)->Draw
                (
                    Context,
                    this->GetAnchoredSize(),
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
                    this->GetBrush().Tint,
                    this->GetBrush().OutlineThickness,
                    this->GetBrush().OutlineTint,
                    this->GetBrush().Radii
                );
            }
        }
    }

    Super::Draw(Context);

    return;
}
