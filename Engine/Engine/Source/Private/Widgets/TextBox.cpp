// Copyright mzoesch. All rights reserved.

#include "Widgets/TextBox.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "Rhi/OrthographicBoxShader.h"
#include "Rhi/OrthographicTextShader.h"
#include "User/UserPreferences.h"

f32 Jafg::LTextScale::InSpt() const noexcept
{
    if (this->IsCustom())
    {
        return this->GetCustomScale();
    }

    JUserPreferences const* Prefs { GetDefault<JUserPreferences>() };

    switch (this->GetPredefinedScale())
    {
    case ETextScale::Header:    return 0.4  * Prefs->ApplicationScale;
    case ETextScale::SubHeader: return 0.25 * Prefs->ApplicationScale;
    case ETextScale::Body:      return 0.2  * Prefs->ApplicationScale;
    case ETextScale::Compact:   return 0.2  * Prefs->ApplicationScale;
    case ETextScale::Small:     return 0.2  * Prefs->ApplicationScale;
    case ETextScale::Tiny:      return 0.18 * Prefs->ApplicationScale;
    // TODO: Make for each application scale an other font size. HeaderFontSize_Single, HeaderFontSize_Double, ...
    // case ETextScale::Header:    return Prefs->HeaderFontSize * Prefs->ApplicationScale;
    // case ETextScale::SubHeader: return Prefs->SubHeaderFontSize * Prefs->ApplicationScale;
    // case ETextScale::Body:      return Prefs->BodyFontSize * Prefs->ApplicationScale;
    // case ETextScale::Compact:   return Prefs->CompactFontSize * Prefs->ApplicationScale;
    // case ETextScale::Small:     return Prefs->SmallFontSize * Prefs->ApplicationScale;
    // case ETextScale::Tiny:      return Prefs->TinyFontSize * Prefs->ApplicationScale;
    }

    unreachable()
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
        this->TextScale.InSpt(),
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
    const LOrthographicTextShader* Shader { GEngine->GetShaderChecked<LOrthographicTextShader>(Name_ShaderOrthographicText) };

    f32 TextScaleInSpt{ this->TextScale.InSpt() };

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

    DesiredSize += this->GetPadding().GetDesiredSizeInSpt();
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
            Out += static_cast<f32>(It->second.Advance.X) * this->TextScale.InSpt() / 64.0f;
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

    const LOrthographicTextShader* Shader { GEngine->GetShaderChecked<LOrthographicTextShader>(Name_ShaderOrthographicText) };

    f32 Width { 0.0f };
    i32 Index { 0 };
    for (auto const Rune : InString)
    {
        ++Index;

        if (auto It { Shader->GetCharacters().find(static_cast<i8>(Rune)) }; It != Shader->GetCharacters().end())
        {
            // TODO: Improve this algorithm to better reflect the actual width of one single character instead of the advance.

            const f32 OldWidth { Width };
            Width += static_cast<f32>(It->second.Advance.X) * this->TextScale.InSpt() / 64.0f;

            if (Width >= InWidth)
            {
                const f32 A
                {
                    Maths::Absolute
                    (
                        OldWidth - InWidth
                        /* Super sketchy solution. This calculation should just not be based of the advance of the character. */
                        + (15.0f * this->TextScale.InSpt())
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
