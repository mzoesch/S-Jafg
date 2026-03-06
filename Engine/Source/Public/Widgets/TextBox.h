// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/Box.h"
#include "Widgets/TextBoxForward.h"
#include "TextBox.generated.h"

namespace Jafg
{

struct LFactoryTextBox;

DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextBox)
class ENGINE_API WTextBox : public WBox
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_NODE_CONSTRUCTORS(WTextBox)

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;

    FORCEINLINE void UpdateDesiredSize() const override { this->UpdateDesiredSizeForString(this->Content); }
    void UpdateDesiredSizeForString(LString const& String) const noexcept;
    FORCEINLINE f32 GetDesiredWidthForString(LString const& String) const noexcept { return this->GetDesiredSizeForString(String).x; }
    LVec2F GetDesiredSizeForString(LString const& String) const noexcept;
    i32 GoToWidth(LString const& InString, const f32 InWidth) const noexcept;

    //# Called if the text box content changes.
    LTextBoxChangedDelegate OnChanged;

    FORCEINLINE void EmptyContent() noexcept { algo::orphan(&this->Content); this->OnChanged.InvokeIfBound(this->Content); }
    FORCEINLINE void SetContent(LString const& InContent) noexcept { this->Content = InContent; this->OnChanged.InvokeIfBound(this->Content); }
    FORCEINLINE void SetContent(LString&& InContent) noexcept { this->Content = std::move(InContent); this->OnChanged.InvokeIfBound(this->Content); }
    FORCEINLINE const LString& GetContent() const noexcept { return this->Content; }

    FORCEINLINE constexpr void SetTextColor(const LColor& InColor) noexcept { this->TextColor = InColor; }
    FORCEINLINE CONSTEXPR_CHECK void SetTextScale(const LTextScale InScale) noexcept { this->TextScale = InScale; }
    FORCEINLINE constexpr void SetTextAlign(const ETextHAlign::Type InAlign) noexcept { this->TextHAlign = InAlign; }
    FORCEINLINE constexpr void SetTextAlign(const ETextVAlign::Type InAlign) noexcept { this->TextVAlign = InAlign; }
    FORCEINLINE constexpr void SetTextHAlign(const ETextHAlign::Type InAlign) noexcept { this->TextHAlign = InAlign; }
    FORCEINLINE constexpr void SetTextVAlign(const ETextVAlign::Type InAlign) noexcept { this->TextVAlign = InAlign; }
    FORCEINLINE constexpr void SetRespectContentHeight(const bool bInRespect) noexcept { this->bRespectContentHeight = bInRespect; }

    FORCEINLINE constexpr const LColor& GetTextColor() const noexcept { return this->TextColor; }
    FORCEINLINE constexpr LTextScale const& GetTextScale() const noexcept { return this->TextScale; }
    FORCEINLINE constexpr ETextHAlign::Type GetTextAlign() const noexcept { return this->TextHAlign; }
    FORCEINLINE constexpr ETextHAlign::Type GetTextHAlign() const noexcept { return this->TextHAlign; }
    FORCEINLINE constexpr ETextVAlign::Type GetTextVAlign() const noexcept { return this->TextVAlign; }
    FORCEINLINE constexpr bool IsRespectingContentHeight() const noexcept { return this->bRespectContentHeight; }

    FORCEINLINE CONSTEXPR_CHECK void SetBrush(const LTextBoxBrush& InBrush) noexcept
    {
        this->TextColor = InBrush.TextColor;
        this->TextScale = InBrush.TextScale;
        this->TextHAlign = InBrush.TextHAlign;
        this->TextVAlign = InBrush.TextVAlign;

        this->Super::SetBrush(InBrush);
    }

    FORCEINLINE constexpr bool IsLeftAligned() const noexcept { return ETextHAlign::IsLeft(this->TextHAlign);   }
    FORCEINLINE constexpr bool IsCenterAligned() const noexcept { return ETextHAlign::IsCenter(this->TextHAlign); }
    FORCEINLINE constexpr bool IsHCenterAligned() const noexcept { return ETextHAlign::IsCenter(this->TextHAlign); }
    FORCEINLINE constexpr bool IsRightAligned() const noexcept { return ETextHAlign::IsRight(this->TextHAlign);  }

    FORCEINLINE constexpr bool IsTopAligned() const noexcept { return ETextVAlign::IsTop(this->TextVAlign);    }
    FORCEINLINE constexpr bool IsVCenterAligned() const noexcept { return ETextVAlign::IsCenter(this->TextVAlign); }
    FORCEINLINE constexpr bool IsBottomAligned() const noexcept { return ETextVAlign::IsBottom(this->TextVAlign); }

    FORCEINLINE const LVec2F& GetDesiredSizeOfRawText() const noexcept { return this->TextDesiredSize; }

protected:

    // Do not forget to call the #OnChanged delegate.
    FORCEINLINE LString& GetMutableContent() noexcept { return this->Content; }

private:

    // void RegisterShaders();

    LColor TextColor{ Colors::Black };
    LTextScale TextScale;
    ETextHAlign::Type TextHAlign{ ETextHAlign::Left };
    ETextVAlign::Type TextVAlign{ ETextVAlign::Top };

    //#
    //# Whether to respect the content height when calculating the desired size.
    //# Meaning, if we always calculate for the highest character that might occour in the content, even if it
    //# doesn't.
    //#
    bool bRespectContentHeight{ false };

    LString Content;

    mutable LVec2F TextDesiredSize;
};

struct LFactoryTextBox : NODE_FACTORY_PARENT(WTextBox)
{
    NODE_FACTORY_BODY(WTextBox)

    FORCEINLINE decltype(auto) Content(this auto&& Self, const LString&  InContent)
    {
        NODE_FACTORY_SELF().SetContent(InContent);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) Content(this auto&& Self, LString&& InContent)
    {
        NODE_FACTORY_SELF().SetContent(std::move(InContent));
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) TextColor(this auto&& Self, const LColor&  InColor)
    {
        NODE_FACTORY_SELF().SetTextColor(InColor);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) TextScale(this auto&& Self, const LTextScale InScale)
    {
        NODE_FACTORY_SELF().SetTextScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) TextAlign(this auto&& Self, const ETextHAlign::Type InAlign)
    {
        NODE_FACTORY_SELF().SetTextHAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) TextAlign(this auto&& Self, const ETextVAlign::Type InAlign)
    {
        NODE_FACTORY_SELF().SetTextVAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) TextHAlign(this auto&& Self, const ETextHAlign::Type InAlign)
    {
        NODE_FACTORY_SELF().SetTextHAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) TextVAlign(this auto&& Self, const ETextVAlign::Type InAlign)
    {
        NODE_FACTORY_SELF().SetTextVAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) RespectContentHeight(this auto&& Self, const bool bInRespect)
    {
        NODE_FACTORY_SELF().SetRespectContentHeight(bInRespect);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) Brush(this auto&& Self, const LTextBoxBrush& InBrush)
    {
        NODE_FACTORY_SELF().SetBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    FORCEINLINE decltype(auto) Brush(this auto&& Self, LTextBoxBrush&& InBrush)
    {
        NODE_FACTORY_SELF().SetBrush(std::move(InBrush));
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
