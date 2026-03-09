// Copyright mzoesch. All rights reserved.

#include "Widgets/TextBox.h"
#include "Framework/FontSubsystem.h"
#include "Rhi/NodeRenderInfo.h"
#include "Engine/Engine.h"
#include "User/UserPreferences.h"

namespace
{

f32 TextBoxInSptImpl(Jafg::ETextScale TextScale, Jafg::EApplicationScale Scale) noexcept
{
    auto const& Prefs{Jafg::GetSingleton<Jafg::JUserPreferences>()};

    switch (Scale)
    {
    case Jafg::EApplicationScale::Single:
    {
        switch (TextScale)
        {
        case Jafg::ETextScale::Header:    { return *Prefs.HeaderFontSizeSingle; }
        case Jafg::ETextScale::SubHeader: { return *Prefs.SubHeaderFontSizeSingle; }
        case Jafg::ETextScale::Body:      { return *Prefs.BodyFontSizeSingle; }
        case Jafg::ETextScale::Compact:   { return *Prefs.CompactFontSizeSingle; }
        case Jafg::ETextScale::Small:     { return *Prefs.SmallFontSizeSingle; }
        case Jafg::ETextScale::Tiny:      { return *Prefs.TinyFontSizeSingle; }
        default: break;
        }
    }
    case Jafg::EApplicationScale::Double:
    {
        switch (TextScale)
        {
        case Jafg::ETextScale::Header:    { return *Prefs.HeaderFontSizeDouble; }
        case Jafg::ETextScale::SubHeader: { return *Prefs.SubHeaderFontSizeDouble; }
        case Jafg::ETextScale::Body:      { return *Prefs.BodyFontSizeDouble; }
        case Jafg::ETextScale::Compact:   { return *Prefs.CompactFontSizeDouble; }
        case Jafg::ETextScale::Small:     { return *Prefs.SmallFontSizeDouble; }
        case Jafg::ETextScale::Tiny:      { return *Prefs.TinyFontSizeDouble; }
        default: break;
        }
    }
    case Jafg::EApplicationScale::Triple:
    {
        switch (TextScale)
        {
        case Jafg::ETextScale::Header:    { return *Prefs.HeaderFontSizeTriple; }
        case Jafg::ETextScale::SubHeader: { return *Prefs.SubHeaderFontSizeTriple; }
        case Jafg::ETextScale::Body:      { return *Prefs.BodyFontSizeTriple; }
        case Jafg::ETextScale::Compact:   { return *Prefs.CompactFontSizeTriple; }
        case Jafg::ETextScale::Small:     { return *Prefs.SmallFontSizeTriple; }
        case Jafg::ETextScale::Tiny:      { return *Prefs.TinyFontSizeTriple; }
        default: break;
        }
    }
    default: break;
    }

    unreachable()
}

} /* ~Namespace <Anonymous> */

f32 Jafg::LTextScale::InSptImpl(LViewport const& Viewport, ETextScale TextScale) noexcept
{
    EApplicationScale Scale{Viewport.GetMaxAllowApplicationScale()};
    if (const EApplicationScale UserMaxScale{*GetSingleton<JUserPreferences>().ApplicationScaleMode}; UserMaxScale != EApplicationScale::Auto)
    {
        Scale = EApplicationScale{maths::min(std::to_underlying(Scale), std::to_underlying(UserMaxScale))};
    }
    return ::TextBoxInSptImpl(TextScale, Scale);
}

void Jafg::WTextBox::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);

    if (this->Content.empty() == false)
    {
        if (this->RenderData.bDirty)
        {
            this->UpdateRenderData(Info.FontSubsystem, this->TextBrush.TextScale.InSpt(Info.Viewport));
        }

        LVec2F TopLeft{this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Info.Viewport) + this->GetPadding().GetTopLeftOffsetInSpt(this->GetViewport())};
        LVec2F PlayRoom{this->GetAnchoredSize_v2() - this->GetPadding().GetDesiredSizeInSpt(this->GetViewport()) - this->RenderData.DesiredSize};
        TopLeft += LVec2F
        {
            this->IsTextLeftAligned() ? 0.0f : (this->IsTextHCenterAligned() ? PlayRoom.x * 0.5f : PlayRoom.x),
            this->IsTextTopAligned()  ? 0.0f : (this->IsTextVCenterAligned() ? PlayRoom.y * 0.5f : PlayRoom.y)
        };

        for (auto const& GlyphInfo : this->RenderData.Glyphes)
        {
            Info.VisualInstances.emplace_back(LVisualInstance{
                .Rect = { TopLeft.x + GlyphInfo.Rect.x, TopLeft.y + GlyphInfo.Rect.y, GlyphInfo.Rect.z, GlyphInfo.Rect.w },
                .Tint = this->TextBrush.Tint.ToVector4(),
                .BackgroundTint = Colors::Transparent.ToVector4(),
                .Radii = maths::zero_vector<LVec4F>,
                .OutlineTint = this->TextBrush.OutlineTint.ToVector4(),
                .TexCoordRect = GlyphInfo.TexCoordRect,
                .OutlineThickness = this->TextBrush.OutlineThickness,
                .TextureIndex = GlyphInfo.BindlessTextureIndex,
                .SamplerIndex = GlyphInfo.SamplerIndex,
                .MsdfPixelRange = GlyphInfo.MsdfPixelRange,
                });
        }
    }

    return;
}

void Jafg::WTextBox::UpdateDesiredSize() const
{
    if (this->RenderData.bDirty)
    {
        this->UpdateRenderData(*this->GetFrontend().GetSubsystemChecked<JFontSubsystem>(), this->TextBrush.TextScale.InSpt(this->GetViewport()));
    }

    this->SetDesiredSizeInSpt(this->GetPadding().GetDesiredSizeInSpt(this->GetViewport()) + this->RenderData.DesiredSize);

    return;
}

// i32 Jafg::WTextBox::GoToWidth(const LString& InString, f32 InWidth) const noexcept
// {
//     if (InString.empty())
//     {
//         return 0;
//     }

    // const LOrthographicTextShader* Shader{ GEngine->GetShaderChecked<LOrthographicTextShader>(Name_ShaderOrthographicText) };

    // f32 Width { 0.0f };
    // i32 Index { 0 };
    // for (auto const Rune : InString)
    // {
    //     ++Index;
    //
    //     if (auto It { Shader->GetCharacters().find(static_cast<i8>(Rune)) }; It != Shader->GetCharacters().end())
    //     {
    //         // TODO: Improve this algorithm to better reflect the actual width of one single character instead of the advance.
    //
    //         const f32 OldWidth { Width };
    //         Width += static_cast<f32>(It->second.Advance.X) * this->TextScale.InSpt(this->GetViewport()) / 64.0f;
    //
    //         if (Width >= InWidth)
    //         {
    //             const f32 A
    //             {
    //                 Maths::Absolute
    //                 (
    //                     OldWidth - InWidth
    //                     /* Super sketchy solution. This calculation should just not be based of the advance of the character. */
    //                     + (15.0f * this->TextScale.InSpt(this->GetViewport()))
    //                 )
    //             };
    //
    //             if (Maths::IsNearlyEqual(A, Maths::Min(A, Maths::Absolute(Width - InWidth))))
    //             {
    //                 return --Index;
    //             }
    //
    //             return Index;
    //         }
    //     }
    //
    //     continue;
    // }
    //
    // return Index;

//     return {};
// }

void Jafg::WTextBox::UpdateRenderData(JFontSubsystem const& FontSubsystem, f32 TargetFontSize) const
{
    this->RenderData.bDirty = false;
    this->RenderData.FontSize = TargetFontSize;

    LVec2F Pencil{maths::zero_vector<LVec2F>};
    this->RenderData.Glyphes = FontSubsystem.GetGlyphInfos(this->Content
        , this->RenderData.FontSize
        , Pencil
        , 0
        );

    LVec4F GlyphesRect{Pencil.x, Pencil.y, Pencil.x, Pencil.y};
    for (auto const& GlyphInfo : this->RenderData.Glyphes)
    {
        GlyphesRect.x = maths::min(GlyphesRect.x, GlyphInfo.Rect.x);
        GlyphesRect.y = maths::min(GlyphesRect.y, GlyphInfo.Rect.y);
        GlyphesRect.z = maths::max(GlyphesRect.z, GlyphInfo.Rect.x + GlyphInfo.Rect.z);
        GlyphesRect.w = maths::max(GlyphesRect.w, GlyphInfo.Rect.y + GlyphInfo.Rect.w);
    }
    this->RenderData.DesiredSize = LVec2F{GlyphesRect.z - GlyphesRect.x, GlyphesRect.w - GlyphesRect.y};

    return;
}
