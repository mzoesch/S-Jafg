// Copyright mzoesch. All rights reserved.

#include "Widgets/TextBox.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "Rhi/OrthographicBoxShader.h"
#include "Rhi/OrthographicTextShader.h"
#include "User/UserPreferences.h"

namespace
{

f32 TextBoxInSptImpl(Jafg::ETextScale::Type TextScale, Jafg::EApplicationScale::Type Scale) noexcept
{
    auto* const Prefs{ Jafg::GetDefault<Jafg::JUserPreferences>() };

    switch (Scale)
    {
    case Jafg::EApplicationScale::Single:
    {
        switch (TextScale)
        {
        case Jafg::ETextScale::Header:    return Prefs->HeaderFontSizeSingle;
        case Jafg::ETextScale::SubHeader: return Prefs->SubHeaderFontSizeSingle;
        case Jafg::ETextScale::Body:      return Prefs->BodyFontSizeSingle;
        case Jafg::ETextScale::Compact:   return Prefs->CompactFontSizeSingle;
        case Jafg::ETextScale::Small:     return Prefs->SmallFontSizeSingle;
        case Jafg::ETextScale::Tiny:      return Prefs->TinyFontSizeSingle;
        default: break;
        }
    }
    case Jafg::EApplicationScale::Double:
    {
        switch (TextScale)
        {
        case Jafg::ETextScale::Header:    return Prefs->HeaderFontSizeDouble;
        case Jafg::ETextScale::SubHeader: return Prefs->SubHeaderFontSizeDouble;
        case Jafg::ETextScale::Body:      return Prefs->BodyFontSizeDouble;
        case Jafg::ETextScale::Compact:   return Prefs->CompactFontSizeDouble;
        case Jafg::ETextScale::Small:     return Prefs->SmallFontSizeDouble;
        case Jafg::ETextScale::Tiny:      return Prefs->TinyFontSizeDouble;
        default: break;
        }
    }
    case Jafg::EApplicationScale::Triple:
    {
        switch (TextScale)
        {
        case Jafg::ETextScale::Header:    return Prefs->HeaderFontSizeTriple;
        case Jafg::ETextScale::SubHeader: return Prefs->SubHeaderFontSizeTriple;
        case Jafg::ETextScale::Body:      return Prefs->BodyFontSizeTriple;
        case Jafg::ETextScale::Compact:   return Prefs->CompactFontSizeTriple;
        case Jafg::ETextScale::Small:     return Prefs->SmallFontSizeTriple;
        case Jafg::ETextScale::Tiny:      return Prefs->TinyFontSizeTriple;
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

    EApplicationScale::Type Scale{ Viewport.GetMaxAllowApplicationScale() };

    if (const EApplicationScale::Type UserMaxScale{ GetDefault<JUserPreferences>()->ApplicationScaleMode }; UserMaxScale != EApplicationScale::Auto)
    {
        Scale = Maths::Min(Scale, UserMaxScale);
    }

    return ::TextBoxInSptImpl(this->GetPredefinedScale(), Scale);
}

f32 Jafg::LTextScale::InSpt(WNode const& Node) const noexcept
{
    return this->InSpt(Node.GetViewport());
}

void Jafg::WTextBox::BeginLifeCDR()
{
    Super::BeginLifeCDR();

    if (GEngine)
    {
        this->RegisterShaders();
    }
    else
    {
        Tasks::Make(ENamedThreads::Master, ETaskTime::BeforeEngineInitButAfterAlloc, [this](void) -> void
        {
            this->RegisterShaders();

            return;
        });
    }

    return;
}

void Jafg::WTextBox::Draw(LViewport& Context) const
{
    Super::Draw(Context);

    GEngine->GetShaderChecked<LOrthographicTextShader>(Name_ShaderOrthographicText)->Draw
    (
        Context,
        this->GetAnchoredSize_v2(),
        this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
        this->GetPadding(),
        this->TextDesiredSize,
        this->TextHAlign,
        this->TextVAlign,
        this->TextColor,
        this->TextScale.InSpt(Context),
        this->Content
    );

    return;
}

void Jafg::WTextBox::UpdateDesiredSize() const
{
    this->UpdateDesiredSizeForString(this->Content);
}

void Jafg::WTextBox::UpdateDesiredSizeForString(const LString& InString) const
{
    LOrthographicTextShader const* Shader{ GEngine->GetShaderChecked<LOrthographicTextShader>(Name_ShaderOrthographicText) };
    const f32 TextScaleInSpt{ this->TextScale.InSpt(this->GetViewport()) };

    LVector2 DesiredSize;
    for (const u8 Rune : InString)
    {
        const Character& Ch { Shader->GetCharacters().at(static_cast<i8>(Rune)) };
        DesiredSize.X += static_cast<f32>(Ch.Advance.X) * TextScaleInSpt / 64.0f;
        DesiredSize.Y = Maths::Max(DesiredSize.Y, static_cast<f32>(Ch.Size.y) * TextScaleInSpt);
    }

    if (this->bRespectContentHeight == false)
    {
        DesiredSize.Y = Shader->GetApproximateHeight(TextScaleInSpt);
    }

    this->TextDesiredSize = DesiredSize;

    DesiredSize += this->GetPadding().GetDesiredSizeInSpt(this->GetViewport());
    this->SetDesiredSizeInSpt(DesiredSize);

    return;
}

f32 Jafg::WTextBox::GetDesiredWidth(const LString& InString) const
{
    if (InString.empty())
    {
        return 0.0f;
    }

    const LOrthographicTextShader* Shader { GEngine->GetShaderChecked<LOrthographicTextShader>(Name_ShaderOrthographicText) };

    f32 Out { 0.0f };
    for (auto const Rune : InString)
    {
        if (auto It { Shader->GetCharacters().find(static_cast<i8>(Rune)) }; It != Shader->GetCharacters().end())
        {
            Out += static_cast<f32>(It->second.Advance.X) * this->TextScale.InSpt(*this) / 64.0f;
        }

        continue;
    }

    return Out;
}

i32 Jafg::WTextBox::GoToWidth(const LString& InString, const f32 InWidth) const
{
    if (InString.empty())
    {
        return 0;
    }

    const LOrthographicTextShader* Shader{ GEngine->GetShaderChecked<LOrthographicTextShader>(Name_ShaderOrthographicText) };

    f32 Width { 0.0f };
    i32 Index { 0 };
    for (auto const Rune : InString)
    {
        ++Index;

        if (auto It { Shader->GetCharacters().find(static_cast<i8>(Rune)) }; It != Shader->GetCharacters().end())
        {
            // TODO: Improve this algorithm to better reflect the actual width of one single character instead of the advance.

            const f32 OldWidth { Width };
            Width += static_cast<f32>(It->second.Advance.X) * this->TextScale.InSpt(this->GetViewport()) / 64.0f;

            if (Width >= InWidth)
            {
                const f32 A
                {
                    Maths::Absolute
                    (
                        OldWidth - InWidth
                        /* Super sketchy solution. This calculation should just not be based of the advance of the character. */
                        + (15.0f * this->TextScale.InSpt(this->GetViewport()))
                    )
                };

                if (Maths::IsNearlyEqual(A, Maths::Min(A, Maths::Absolute(Width - InWidth))))
                {
                    return --Index;
                }

                return Index;
            }
        }

        continue;
    }

    return Index;
}

void Jafg::WTextBox::RegisterShaders()
{
    check( GEngine )

    if (GEngine->IsShaderValid(Name_ShaderOrthographicText) == false)
    {
        (new LOrthographicTextShader)->MakeChecked(Name_ShaderOrthographicText);
    }

    return;
}
