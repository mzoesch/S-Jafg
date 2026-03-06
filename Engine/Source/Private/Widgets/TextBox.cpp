// Copyright mzoesch. All rights reserved.

#include "Widgets/TextBox.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "User/UserPreferences.h"

namespace
{

f32 TextBoxInSptImpl(Jafg::ETextScale::Type TextScale, Jafg::EApplicationScale Scale) noexcept
{
    auto* const Prefs{ &Jafg::GetSingleton<Jafg::JUserPreferences>() };

    switch (Scale)
    {
    case Jafg::EApplicationScale::Single:
    {
        switch (TextScale)
        {
        case Jafg::ETextScale::Header:    return *Prefs->HeaderFontSizeSingle;
        case Jafg::ETextScale::SubHeader: return *Prefs->SubHeaderFontSizeSingle;
        case Jafg::ETextScale::Body:      return *Prefs->BodyFontSizeSingle;
        case Jafg::ETextScale::Compact:   return *Prefs->CompactFontSizeSingle;
        case Jafg::ETextScale::Small:     return *Prefs->SmallFontSizeSingle;
        case Jafg::ETextScale::Tiny:      return *Prefs->TinyFontSizeSingle;
        default: break;
        }
    }
    case Jafg::EApplicationScale::Double:
    {
        switch (TextScale)
        {
        case Jafg::ETextScale::Header:    return *Prefs->HeaderFontSizeDouble;
        case Jafg::ETextScale::SubHeader: return *Prefs->SubHeaderFontSizeDouble;
        case Jafg::ETextScale::Body:      return *Prefs->BodyFontSizeDouble;
        case Jafg::ETextScale::Compact:   return *Prefs->CompactFontSizeDouble;
        case Jafg::ETextScale::Small:     return *Prefs->SmallFontSizeDouble;
        case Jafg::ETextScale::Tiny:      return *Prefs->TinyFontSizeDouble;
        default: break;
        }
    }
    case Jafg::EApplicationScale::Triple:
    {
        switch (TextScale)
        {
        case Jafg::ETextScale::Header:    return *Prefs->HeaderFontSizeTriple;
        case Jafg::ETextScale::SubHeader: return *Prefs->SubHeaderFontSizeTriple;
        case Jafg::ETextScale::Body:      return *Prefs->BodyFontSizeTriple;
        case Jafg::ETextScale::Compact:   return *Prefs->CompactFontSizeTriple;
        case Jafg::ETextScale::Small:     return *Prefs->SmallFontSizeTriple;
        case Jafg::ETextScale::Tiny:      return *Prefs->TinyFontSizeTriple;
        default: break;
        }
    }
    default: break;
    }

    unreachable()
}

} /* ~Namespace <Anonymous> */

f32 Jafg::LTextScale::InSpt(LViewport const& Viewport) const noexcept
{
    if (this->IsCustom())
    {
        return this->GetCustomScale();
    }

    EApplicationScale Scale{ Viewport.GetMaxAllowApplicationScale() };

    if (const EApplicationScale UserMaxScale{ *GetSingleton<JUserPreferences>().ApplicationScaleMode }; UserMaxScale != EApplicationScale::Auto)
    {
        Scale = EApplicationScale{maths::min(std::to_underlying(Scale), std::to_underlying(UserMaxScale))};
    }

    return ::TextBoxInSptImpl(this->GetPredefinedScale(), Scale);
}

f32 Jafg::LTextScale::InSpt(WNode const& Node) const noexcept
{
    return this->InSpt(Node.GetViewport());
}

// void Jafg::WTextBox::BeginLifeCDR()
// {
//     Super::BeginLifeCDR();
//
    // if (GEngine)
    // {
    //     this->RegisterShaders();
    // }
    // else
    // {
    //     Tasks::Make(ENamedThreads::Master, ETaskTime::BeforeEngineInitButAfterAlloc, [this](void) -> void
    //     {
    //         this->RegisterShaders();
    //
    //         return;
    //     });
    // }
//
//     return;
// }

void Jafg::WTextBox::Draw(LNodeRenderInfo const& Info) const
{
    Super::Draw(Info);
    //
    // GEngine->GetShaderChecked<LOrthographicTextShader>(Name_ShaderOrthographicText)->Draw
    // (
    //     Context,
    //     this->GetAnchoredSize_v2(),
    //     this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
    //     this->GetPadding(),
    //     this->TextDesiredSize,
    //     this->TextHAlign,
    //     this->TextVAlign,
    //     this->TextColor,
    //     this->TextScale.InSpt(Context),
    //     this->Content
    // );

    return;
}

void Jafg::WTextBox::UpdateDesiredSizeForString(LString const& String) const noexcept
{
    LVec2F DesiredSize{ this->GetDesiredSizeForString(String) };

    // if (!GEngine->GetShader<LOrthographicTextShader>(Name_ShaderOrthographicText))
    // {
    //     return;
    // }
    //
    // if (this->bRespectContentHeight == false)
    // {
    //     DesiredSize.Y = GEngine->GetShaderChecked<LOrthographicTextShader>(Name_ShaderOrthographicText)
    //         ->GetApproxBearingHeight(this->TextScale.InSpt(this->GetViewport()));
    // }

    this->TextDesiredSize = DesiredSize;

    DesiredSize += this->GetPadding().GetDesiredSizeInSpt(this->GetViewport());
    this->SetDesiredSizeInSpt(DesiredSize);

    return;
}

LVec2F Jafg::WTextBox::GetDesiredSizeForString(LString const& String) const noexcept
{
    // const LOrthographicTextShader* Shader{ GEngine->GetShader<LOrthographicTextShader>(Name_ShaderOrthographicText) };

    // if (!Shader) { return LVector2::ZeroVector; }
    //
    // const f32 TextScaleInSpt{ this->TextScale.InSpt(this->GetViewport()) };
    //
    // LVector2 Out;
    // LOrthographicTextShader::LCharacterMap::const_iterator LastIt{ Shader->GetCharacters().end() };
    // for (auto Rune : String)
    // {
    //     if (auto It{ Shader->GetCharacters().find(static_cast<i8>(Rune)) }; It != Shader->GetCharacters().end())
    //     {
    //         // if (LastIt != Shader->GetCharacters().end())
    //         // {
    //         //     Out.X += (static_cast<f32>(LastIt->second.Advance.X) / 64.0f) * TextScaleInSpt;
    //         //     Out.Y  = Maths::Max(Out.Y, static_cast<f32>(LastIt->second.Size.Y) * TextScaleInSpt);
    //         // }
    //         //
    //         // LastIt = It;
    //
    //         Out.X += (static_cast<f32>(It->second.Advance.X) / 64.0f) * TextScaleInSpt;
    //         Out.Y  = Maths::Max(Out.Y, static_cast<f32>(It->second.Size.Y) * TextScaleInSpt);
    //     }
    //
    //     continue;
    // }

    // if (LastIt != Shader->GetCharacters().end())
    // {
    //     Out.X += (static_cast<f32>(LastIt->second.Size.X)) * TextScaleInSpt;
    //     Out.Y  = Maths::Max(Out.Y, static_cast<f32>(LastIt->second.Size.Y) * TextScaleInSpt);
    // }

    // return Out;

    return maths::zero_vector<LVec2F>;
}

i32 Jafg::WTextBox::GoToWidth(const LString& InString, const f32 InWidth) const noexcept
{
    if (InString.empty())
    {
        return 0;
    }

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

    return {};
}
