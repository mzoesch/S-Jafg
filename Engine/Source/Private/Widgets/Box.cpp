// Copyright mzoesch. All rights reserved.

#include "Widgets/Box.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"

void Jafg::WBox::Draw(LNodeRenderInfo const& Info) const
{
    // if (this->Brush.Type != ERegionBrush::None)
    // {
    //     if (this->Brush.Type == ERegionBrush::Box)
    //     {
    //         if (this->Brush.Image.IsTextureValid())
    //         {
    //             GEngine->GetShaderChecked<LOrthographicImageBoxShader>(Name_ShaderOrthographicImageBox)->Draw
    //             (
    //                 Context,
    //                 this->GetAnchoredSize_v2(),
    //                 this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
    //                 this->GetBrush().Tint,
    //                 this->GetBrush().Image,
    //                 this->GetBrush().ImageTint,
    //                 this->GetBrush().ImageScale,
    //                 this->GetBrush().ImageBehavior,
    //                 this->GetBrush().ImageOobm,
    //                 this->GetBrush().ImagePadding
    //             );
    //         }
    //         else
    //         {
    //             GEngine->GetShaderChecked<LOrthographicBoxShader>(Name_ShaderOrthographicBox)->Draw
    //             (
    //                 Context,
    //                 this->GetAnchoredSize_v2(),
    //                 this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
    //                 this->GetBrush().Tint
    //             );
    //         }
    //     }
    //
    //     else if (this->Brush.Type == ERegionBrush::RoundedBox)
    //     {
    //         if (this->Brush.Image.IsTextureValid())
    //         {
    //             GEngine->GetShaderChecked<LOrthographicRoundedImageBoxShader>(Name_ShaderOrthographicRoundedImageBox)->Draw
    //             (
    //                 Context,
    //                 this->GetAnchoredSize_v2(),
    //                 this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
    //                 this->GetBrush().Tint,
    //                 this->GetBrush().Radii,
    //                 this->GetBrush().Image,
    //                 this->GetBrush().ImageTint,
    //                 this->GetBrush().ImageScale,
    //                 this->GetBrush().ImageBehavior,
    //                 this->GetBrush().ImageOobm,
    //                 this->GetBrush().ImagePadding
    //             );
    //         }
    //         else
    //         {
    //             GEngine->GetShaderChecked<LOrthographicRoundedBoxShader>(Name_ShaderOrthographicRoundedBox)->Draw
    //             (
    //                 Context,
    //                 this->GetAnchoredSize_v2(),
    //                 this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
    //                 this->GetBrush().Tint,
    //                 this->GetBrush().Radii
    //             );
    //         }
    //     }
    //
    //     else if (this->Brush.Type == ERegionBrush::OutlineBox)
    //     {
    //         if (this->Brush.Image.IsTextureValid())
    //         {
    //             GEngine->GetShaderChecked<LOrthographicOutlineImageBoxShader>(Name_ShaderOrthographicOutlineImageBox)->Draw
    //             (
    //                 Context,
    //                 this->GetAnchoredSize_v2(),
    //                 this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
    //                 this->GetBrush().Tint,
    //                 this->GetBrush().OutlineThickness,
    //                 this->GetBrush().OutlineTint,
    //                 this->GetBrush().Image,
    //                 this->GetBrush().ImageTint,
    //                 this->GetBrush().ImageScale,
    //                 this->GetBrush().ImageBehavior,
    //                 this->GetBrush().ImageOobm,
    //                 this->GetBrush().ImagePadding
    //             );
    //         }
    //         else
    //         {
    //             GEngine->GetShaderChecked<LOrthographicOutlineBoxShader>(Name_ShaderOrthographicOutlineBox)->Draw
    //             (
    //                 Context,
    //                 this->GetAnchoredSize_v2(),
    //                 this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
    //                 this->GetBrush().Tint,
    //                 this->GetBrush().OutlineThickness,
    //                 this->GetBrush().OutlineTint
    //             );
    //         }
    //     }
    //
    //     else if (this->Brush.Type == ERegionBrush::RoundedOutlineBox)
    //     {
    //         if (this->Brush.Image.IsTextureValid())
    //         {
    //             GEngine->GetShaderChecked<LOrthographicRoundedOutlineImageBoxShader>(Name_ShaderOrthographicRoundedOutlineImageBox)->Draw
    //             (
    //                 Context,
    //                 this->GetAnchoredSize_v2(),
    //                 this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
    //                 this->GetBrush().Tint,
    //                 this->GetBrush().OutlineThickness,
    //                 this->GetBrush().OutlineTint,
    //                 this->GetBrush().Radii,
    //                 this->GetBrush().Image,
    //                 this->GetBrush().ImageTint,
    //                 this->GetBrush().ImageScale,
    //                 this->GetBrush().ImageBehavior,
    //                 this->GetBrush().ImageOobm,
    //                 this->GetBrush().ImagePadding
    //             );
    //         }
    //         else
    //         {
    //             GEngine->GetShaderChecked<LOrthographicRoundedOutlineBoxShader>(Name_ShaderOrthographicRoundedOutlineBox)->Draw
    //             (
    //                 Context,
    //                 this->GetAnchoredSize_v2(),
    //                 this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
    //                 this->GetBrush().Tint,
    //                 this->GetBrush().OutlineThickness,
    //                 this->GetBrush().OutlineTint,
    //                 this->GetBrush().Radii
    //             );
    //         }
    //     }
    // }

    Super::Draw(Info);

    return;
}

void Jafg::WBox::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();
    this->SetDesiredSize(this->Brush.Padding.GetDesiredSize());

    return;
}
