// Copyright mzoesch. All rights reserved.

#include "Widgets/TextBox.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "Rhi/OrthographicBoxShader.h"
#include "User/UserPreferences.h"
#include "Rhi/OrthographicTextShader.h"

namespace
{

FORCEINLINE Jafg::LTextBoxBrush GetDefaultTextBoxBrush() noexcept
{
    Jafg::LTextBoxBrush Brush;
    Brush.Tint = Lal::LColor::Transparent;
    Brush.Padding = {4.5f};
    return Brush;
}

} /* ~Namespace <Anonymous> */

Jafg::LTextBoxBrush Jafg::LTextBoxBrush::Header()
{
    return ::GetDefaultTextBoxBrush().TextScaleRet(GetDefault<JUserPreferences>()->HeaderFontSize);
}

Jafg::LTextBoxBrush Jafg::LTextBoxBrush::SubHeader()
{
    return ::GetDefaultTextBoxBrush().TextScaleRet(GetDefault<JUserPreferences>()->SubHeaderFontSize);
}

Jafg::LTextBoxBrush Jafg::LTextBoxBrush::Body()
{
    return ::GetDefaultTextBoxBrush().TextScaleRet(GetDefault<JUserPreferences>()->BodyFontSize);
}

Jafg::LTextBoxBrush Jafg::LTextBoxBrush::Compact()
{
    return ::GetDefaultTextBoxBrush().TextScaleRet(GetDefault<JUserPreferences>()->CompactFontSize);
}

Jafg::LTextBoxBrush Jafg::LTextBoxBrush::Small()
{
    return ::GetDefaultTextBoxBrush().TextScaleRet(GetDefault<JUserPreferences>()->SmallFontSize);
}

Jafg::LTextBoxBrush Jafg::LTextBoxBrush::Tiny()
{
    return ::GetDefaultTextBoxBrush().TextScaleRet(GetDefault<JUserPreferences>()->TinyFontSize);
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
        this->GetAnchoredSize(),
        this->GetAnchoredAndTranslatedTopLeftFromMostOuter(Context),
        this->GetPadding(),
        this->TextDesiredSize,
        this->TextHAlign,
        this->TextVAlign,
        this->TextColor,
        this->TextScale,
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

    LVector2 DesiredSize;
    for (const u8 Rune : InString)
    {
        const Character& Ch { Shader->GetCharacters().at(static_cast<i8>(Rune)) };
        DesiredSize.X += static_cast<f32>(Ch.Advance.X) * this->TextScale / 64.0f;
        DesiredSize.Y = Maths::Max(DesiredSize.Y, static_cast<f32>(Ch.Size.y) * this->TextScale);
    }

    if (this->bRespectContentHeight == false)
    {
        DesiredSize.Y = Shader->GetApproximateHeight(this->TextScale);
    }

    this->TextDesiredSize = DesiredSize;

    DesiredSize += this->GetPadding().GetDesiredSize();
    this->SetDesiredSize(DesiredSize);

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
            Out += static_cast<f32>(It->second.Advance.X) * this->TextScale / 64.0f;
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
            Width += static_cast<f32>(It->second.Advance.X) * this->TextScale / 64.0f;

            if (Width >= InWidth)
            {
                const f32 A
                {
                    Maths::Absolute
                    (
                        OldWidth - InWidth
                        /* Super sketchy solution. This calculation should just not be based of the advance of the character. */
                        + (15.0f * this->TextScale)
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
