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
    Brush.Tint = Jafg::LColor::Transparent;
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

void Jafg::WTextBox::BeginLifeDefault()
{
    Super::BeginLifeDefault();

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
    if (InString.IsEmpty())
    {
        return 0.0f;
    }

    const LOrthographicTextShader* Shader { GEngine->GetShaderChecked<LOrthographicTextShader>(Name_ShaderOrthographicText) };

    f32 Out { 0.0f };
    for (const LString::T Rune : InString)
    {
        if (auto It { Shader->GetCharacters().find(static_cast<i8>(Rune)) }; It != Shader->GetCharacters().end())
        {
            Out += static_cast<f32>(It->second.Advance.X) * this->TextScale / 64.0f;
        }

        continue;
    }

    return Out;
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
