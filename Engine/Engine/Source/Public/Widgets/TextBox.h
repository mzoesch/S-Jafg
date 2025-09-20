// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Box.h"
#include "Widgets/TextBoxForward.h"
#include "TextBox.generated.h"

namespace Jafg
{

template <typename TNode>
class TWidgetFactoryTextBox : public TWidgetFactoryBox<TNode>
{
public:

    GENERATED_FACTORY_BODY(TWidgetFactoryBox)

    FORCEINLINE TFactoryRetTy& Content(const LString&  InContent) { this->This()->SetContent(InContent); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Content(LString&& InContent) { this->This()->SetContent(std::move(InContent)); return this->Self(); }

    FORCEINLINE TFactoryRetTy& TextColor(const Lal::LColor&  InColor) { this->This()->SetTextColor(InColor); return this->Self(); }
    FORCEINLINE TFactoryRetTy& TextScale(const f32 InScale) { this->This()->SetTextScale(InScale); return this->Self(); }
    FORCEINLINE TFactoryRetTy& TextAlign(const ETextHAlign::Type InAlign) { this->This()->SetTextHAlign(InAlign); return this->Self(); }
    FORCEINLINE TFactoryRetTy& TextAlign(const ETextVAlign::Type InAlign) { this->This()->SetTextVAlign(InAlign); return this->Self(); }
    FORCEINLINE TFactoryRetTy& TextHAlign(const ETextHAlign::Type InAlign) { this->This()->SetTextHAlign(InAlign); return this->Self(); }
    FORCEINLINE TFactoryRetTy& TextVAlign(const ETextVAlign::Type InAlign) { this->This()->SetTextVAlign(InAlign); return this->Self(); }
    FORCEINLINE TFactoryRetTy& RespectContentHeight(const bool bInRespect) { this->This()->SetRespectContentHeight(bInRespect); return this->Self(); }

    FORCEINLINE TFactoryRetTy& Brush(const LTextBoxBrush& InBrush) { this->This()->SetBrush(InBrush); return this->Self(); }
    FORCEINLINE TFactoryRetTy& Brush(LTextBoxBrush&& InBrush) { this->This()->SetBrush(std::move(InBrush)); return this->Self(); }
};

DECLARE_JAFG_WIDGET_WITH_FACTORY(TWidgetFactoryTextBox)
class ENGINE_API WTextBox : public WBox
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(WTextBox)

public:

    virtual void BeginLifeDefault() override;
    virtual void Draw(LViewport& Context) const override;

    virtual void UpdateDesiredSize() const override;
    void UpdateDesiredSizeForString(const LString& InString) const;
    f32 GetDesiredWidth(const LString& InString) const;
    i32 GoToWidth(const LString& InString, const f32 InWidth) const;

    //# Called if the text box content changes.
    LTextBoxChangedDelegate OnChanged;

    FORCEINLINE void EmptyContent() noexcept { this->Content.Empty(); this->OnChanged.InvokeIfBound(this->Content); }
    FORCEINLINE void SetContent(const LString& InContent) noexcept { this->Content = InContent; this->OnChanged.InvokeIfBound(this->Content); }
    FORCEINLINE void SetContent(LString&& InContent) noexcept { this->Content = std::move(InContent); this->OnChanged.InvokeIfBound(this->Content); }
    FORCEINLINE const LString& GetContent() const noexcept { return this->Content; }

    FORCEINLINE constexpr void SetTextColor(const Lal::LColor& InColor) noexcept { this->TextColor = InColor; }
    FORCEINLINE constexpr void SetTextScale(const f32 InScale) noexcept { this->TextScale = InScale; }
    FORCEINLINE constexpr void SetTextAlign(const ETextHAlign::Type InAlign) noexcept { this->TextHAlign = InAlign; }
    FORCEINLINE constexpr void SetTextAlign(const ETextVAlign::Type InAlign) noexcept { this->TextVAlign = InAlign; }
    FORCEINLINE constexpr void SetTextHAlign(const ETextHAlign::Type InAlign) noexcept { this->TextHAlign = InAlign; }
    FORCEINLINE constexpr void SetTextVAlign(const ETextVAlign::Type InAlign) noexcept { this->TextVAlign = InAlign; }
    FORCEINLINE constexpr void SetRespectContentHeight(const bool bInRespect) noexcept { this->bRespectContentHeight = bInRespect; }

    FORCEINLINE constexpr const Lal::LColor& GetTextColor() const noexcept { return this->TextColor; }
    FORCEINLINE constexpr f32               GetTextScale() const noexcept { return this->TextScale; }
    FORCEINLINE constexpr ETextHAlign::Type GetTextAlign() const noexcept { return this->TextHAlign; }
    FORCEINLINE constexpr ETextHAlign::Type GetTextHAlign() const noexcept { return this->TextHAlign; }
    FORCEINLINE constexpr ETextVAlign::Type GetTextVAlign() const noexcept { return this->TextVAlign; }
    FORCEINLINE constexpr bool IsRespectingContentHeight() const noexcept { return this->bRespectContentHeight; }

    FORCEINLINE constexpr void SetBrush(const LTextBoxBrush& InBrush) noexcept;
    FORCEINLINE constexpr void SetBrush(LTextBoxBrush&& InBrush) noexcept;
    //# WARNING: This returns a new brush and not a reference.
    FORCEINLINE constexpr LTextBoxBrush GetBrush() const noexcept;

    FORCEINLINE constexpr bool IsLeftAligned() const noexcept { return ETextHAlign::IsLeft(this->TextHAlign);   }
    FORCEINLINE constexpr bool IsCenterAligned() const noexcept { return ETextHAlign::IsCenter(this->TextHAlign); }
    FORCEINLINE constexpr bool IsHCenterAligned() const noexcept { return ETextHAlign::IsCenter(this->TextHAlign); }
    FORCEINLINE constexpr bool IsRightAligned() const noexcept { return ETextHAlign::IsRight(this->TextHAlign);  }

    FORCEINLINE constexpr bool IsTopAligned() const noexcept { return ETextVAlign::IsTop(this->TextVAlign);    }
    FORCEINLINE constexpr bool IsVCenterAligned() const noexcept { return ETextVAlign::IsCenter(this->TextVAlign); }
    FORCEINLINE constexpr bool IsBottomAligned() const noexcept { return ETextVAlign::IsBottom(this->TextVAlign); }

    FORCEINLINE const LVector2& GetDesiredSizeOfRawText() const noexcept { return this->TextDesiredSize; }

protected:

    // Do not forget to call the #OnChanged delegate.
    FORCEINLINE LString& GetMutableContent() noexcept { return this->Content; }

private:

    void RegisterShaders();

    Lal::LColor TextColor { Lal::LColor::Black };
    f32 TextScale { 1.0f };
    ETextHAlign::Type TextHAlign { ETextHAlign::Left };
    ETextVAlign::Type TextVAlign { ETextVAlign::Top };

    //#
    //# Whether to respect the content height when calculating the desired size.
    //# Meaning, if we always calculate for the highest character that might occour in the content, even if it
    //# doesn't.
    //#
    bool bRespectContentHeight { false };

    LString Content;

    mutable LVector2 TextDesiredSize;
};

FORCEINLINE constexpr void WTextBox::SetBrush(const LTextBoxBrush& InBrush) noexcept
{
    this->TextColor = InBrush.TextColor;
    this->TextScale = InBrush.TextScale;
    this->TextHAlign = InBrush.TextHAlign;
    this->TextVAlign = InBrush.TextVAlign;

    this->Super::SetBrush(InBrush);

    return;
}

FORCEINLINE constexpr void WTextBox::SetBrush(LTextBoxBrush&& InBrush) noexcept
{
    this->TextColor = std::move(InBrush.TextColor);
    this->TextScale = std::move(InBrush.TextScale);
    this->TextHAlign = std::move(InBrush.TextHAlign);
    this->TextVAlign = std::move(InBrush.TextVAlign);

    this->Super::SetBrush(std::move(InBrush));

    return;
}

FORCEINLINE constexpr LTextBoxBrush WTextBox::GetBrush() const noexcept
{
    static_assert(sizeof(LTextBoxBrush) == 88, "This method needs to be updated because LTextBoxBrush has changed.");

    LTextBoxBrush Out;

    Out.Type = this->Super::GetBrush().Type;
    Out.Tint = this->Super::GetBrush().Tint;
    Out.Image = this->Super::GetBrush().Image;
    Out.ImageTint = this->Super::GetBrush().ImageTint;
    Out.ImageScale = this->Super::GetBrush().ImageScale;
    Out.ImageBehavior = this->Super::GetBrush().ImageBehavior;
    Out.ImageOobm = this->Super::GetBrush().ImageOobm;
    Out.ImagePadding = this->Super::GetBrush().ImagePadding;
    Out.Radii = this->Super::GetBrush().Radii;
    Out.OutlineThickness = this->Super::GetBrush().OutlineThickness;
    Out.OutlineTint = this->Super::GetBrush().OutlineTint;

    Out.Padding = this->Super::GetBrush().Padding;

    Out.TextColor = this->TextColor;
    Out.TextScale = this->TextScale;
    Out.TextHAlign = this->TextHAlign;
    Out.TextVAlign = this->TextVAlign;

    return Out;
}

} /* ~Namespace Jafg */
