// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Box.h"
#include "Framework/FontSubsystemForward.h"
#include "TextBox.generated.h"

namespace Jafg
{

class JFontSubsystem;
struct LFactoryTextBox;

enum struct ETextHAlign : u8
{
    Left, Center, Right, // TODO: Justify
};

enum struct ETextVAlign : u8
{
    Top, Center, Bottom
};

enum struct ETextScale : u8
{
    Header,
    SubHeader,
    Body,
    Compact,
    Small,
};

struct LTextScale final
{
    inline static constexpr LTextScale Header() noexcept { return LTextScale{ETextScale::Header}; }
    inline static constexpr LTextScale SubHeader() noexcept { return LTextScale{ETextScale::SubHeader}; }
    inline static constexpr LTextScale Body() noexcept { return LTextScale{ETextScale::Body}; }
    inline static constexpr LTextScale Compact() noexcept { return LTextScale{ETextScale::Compact}; }
    inline static constexpr LTextScale Small() noexcept { return LTextScale{ETextScale::Small}; }

    FORCEINLINE constexpr LTextScale() noexcept = delete;
    FORCEINLINE constexpr LTextScale(ETextScale InScale) noexcept : Scale(InScale) {}
    FORCEINLINE constexpr LTextScale(LWidgetSize1 InScale) noexcept : Scale(InScale) {}
    FORCEINLINE constexpr LTextScale(LTextScale const& Other) noexcept { this->Scale = Other.Scale; }
    FORCEINLINE constexpr LTextScale& operator=(ETextScale InScale) noexcept { this->Scale = InScale; return *this; }
    FORCEINLINE constexpr LTextScale& operator=(LWidgetSize1 InScale) noexcept { this->Scale = InScale; return *this; }
    FORCEINLINE constexpr LTextScale& operator=(LTextScale const& Other) noexcept { this->Scale = Other.Scale; return *this; }

    FORCEINLINE constexpr auto const& GetScale() const noexcept { return this->Scale; }

    inline f32 InSpt(LViewport const& Viewport) const noexcept
    {
        if (std::holds_alternative<LWidgetSize1>(this->Scale))
        {
            return ::Jafg::InSpt(Viewport, std::get<LWidgetSize1>(this->Scale));
        }
        return LTextScale::InSptImpl(Viewport, std::get<ETextScale>(this->Scale));
    }

private:

    ENGINE_API static f32 InSptImpl(LViewport const& Viewport, ETextScale TextScale) noexcept;
    std::variant<ETextScale, LWidgetSize1> Scale;
};

struct LTextBrush
{
    LTextScale TextScale{ ETextScale::Body };

    LColor Tint{ Colors::White };

    bool bSkipBrushDraw{ false };

    f32 OutlineThickness{};
    LColor OutlineTint{ Colors::White };

    ETextHAlign TextHAlign{ ETextHAlign::Left };
    ETextVAlign TextVAlign{ ETextVAlign::Top };
};

//# A box that can display text.
DECLARE_JAFG_WIDGET_WITH_FACTORY(LFactoryTextBox)
class ENGINE_API WTextBox : public WBox
{
    GENERATED_CLASS_BODY()

    // TODO:
    //      Multiple fonts (not in one box but in general - for multiple fonts in one textbox, make that an extra class).
    //      New lines.
    //      Text wrapping (cutoff or with ...)
    //      Spacing between glyphs and lines.

protected:

    DEFAULT_NODE_CONSTRUCTORS_BODY(WTextBox)
    {
        this->SetTint(Colors::Black);
        this->SetPadding({4,2});
    }

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;

    //# Called if the text box content changes.
    EVENT_DECL(OnChanged, void(LString const& NewContent))
    FORCEINLINE void EmptyContent() { algo::orphan(&this->Content); this->OnChanged.InvokeIfBound(this->Content); }
    FORCEINLINE void SetContent(LString const& InContent) { this->Content = InContent; this->RenderData.Dirty(); this->OnChanged.InvokeIfBound(this->Content); }
    FORCEINLINE void SetContent(LString&& InContent) { this->Content = std::move(InContent); this->RenderData.Dirty(); this->OnChanged.InvokeIfBound(this->Content); }
    FORCEINLINE constexpr LString const& GetContent() const noexcept { return this->Content; }

    constexpr void SetTextBrush(LTextBrush const& InBrush) noexcept { this->TextBrush = InBrush; this->RenderData.Dirty(); }
    constexpr LTextBrush& GetMutableTextBrush() noexcept { this->RenderData.Dirty(); return this->TextBrush; }
    constexpr LTextBrush const& GetTextBrush() const noexcept { return this->TextBrush; }

    constexpr void SetTextTint(LColor const& InTint) noexcept { this->TextBrush.Tint = InTint; }
    constexpr void SetSkipTextBrushDraw(bool bInSkip) noexcept { this->TextBrush.bSkipBrushDraw = bInSkip; }
    constexpr void SetTextOutlineThickness(f32 InThickness) noexcept { this->TextBrush.OutlineThickness = InThickness; }
    constexpr void SetTextOutlineTint(LColor const& InTint) noexcept { this->TextBrush.OutlineTint = InTint; }
    constexpr void SetTextScale(LTextScale InScale) noexcept { this->TextBrush.TextScale = InScale; this->RenderData.Dirty(); }
    constexpr void SetTextAlign(ETextHAlign InAlign) noexcept { this->TextBrush.TextHAlign = InAlign; }
    constexpr void SetTextAlign(ETextVAlign InAlign) noexcept { this->TextBrush.TextVAlign = InAlign; }
    constexpr void SetTextHAlign(ETextHAlign InAlign) noexcept { this->TextBrush.TextHAlign = InAlign; }
    constexpr void SetTextVAlign(ETextVAlign InAlign) noexcept { this->TextBrush.TextVAlign = InAlign; }

    constexpr LColor const& GetTextTint() const noexcept { return this->TextBrush.Tint; }
    constexpr bool GetSkipTextBrushDraw() const noexcept { return this->TextBrush.bSkipBrushDraw; }
    constexpr f32 GetTextOutlineThickness() const noexcept { return this->TextBrush.OutlineThickness; }
    constexpr LColor const& GetTextOutlineTint() const noexcept { return this->TextBrush.OutlineTint; }
    constexpr LTextScale GetTextScale() const noexcept { return this->TextBrush.TextScale; }
    constexpr ETextHAlign GetTextHAlign() const noexcept { return this->TextBrush.TextHAlign; }
    constexpr ETextVAlign GetTextVAlign() const noexcept { return this->TextBrush.TextVAlign; }

    constexpr bool IsTextLeftAligned() const noexcept { return this->TextBrush.TextHAlign == ETextHAlign::Left; }
    constexpr bool IsTextHCenterAligned() const noexcept { return this->TextBrush.TextHAlign == ETextHAlign::Center; }
    constexpr bool IsTextRightAligned() const noexcept { return this->TextBrush.TextHAlign == ETextHAlign::Right;  }
    constexpr bool IsTextTopAligned() const noexcept { return this->TextBrush.TextVAlign == ETextVAlign::Top; }
    constexpr bool IsTextVCenterAligned() const noexcept { return this->TextBrush.TextVAlign == ETextVAlign::Center; }
    constexpr bool IsTextBottomAligned() const noexcept { return this->TextBrush.TextVAlign == ETextVAlign::Bottom; }

    //# Offset is only for drawing and does not affect the desired size in any way.
    constexpr void SetTextDrawOffset(LVec2F InOffset) const noexcept { this->DrawOffset = InOffset; }
    constexpr LVec2F GetTextDrawOffset() const noexcept { return this->DrawOffset; }

protected:

    //# @note Do not forget to call the #InvokeOnChanged delegate.
    FORCEINLINE LString& GetMutableContent() noexcept { this->RenderData.Dirty(); return this->Content; }
    inline bool InvokeOnChanged() { this->RenderData.Dirty(); return this->OnChanged.InvokeIfBound(this->Content); }

private:

    void UpdateRenderData(JFontSubsystem const& FontSubsystem, f32 TargetFontSize) const;

    LString Content;
    LTextBrush TextBrush;
    mutable LVec2F DrawOffset{};
    mutable LVec2F TextDesiredSize{};

    mutable struct
    {
        constexpr void Dirty() noexcept { bDirty = true; }

        bool bDirty{ true };
        f32 FontSize{};
        TArray<LGlyphInfo> Glyphes;
        LVec2F DesiredSize;
    } RenderData;
};

struct LFactoryTextBox : NODE_FACTORY_PARENT(WTextBox)
{
    NODE_FACTORY_BODY(WTextBox)

    constexpr decltype(auto) Content(this auto&& Self, LString const& InContent) noexcept
    {
        NODE_FACTORY_SELF().SetContent(InContent);
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) Content(this auto&& Self, LString&& InContent) noexcept
    {
        NODE_FACTORY_SELF().SetContent(std::move(InContent));
        return NODE_FACTORY_RESULT();
    }

    constexpr decltype(auto) TextBrush(this auto&& Self, LTextBrush const& InBrush) noexcept
    {
        NODE_FACTORY_SELF().SetTextBrush(InBrush);
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetTextTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) SkipTextBrushDraw(this auto&& Self, bool bInSkip) noexcept
    {
        NODE_FACTORY_SELF().SetSkipTextBrushDraw(bInSkip);
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextOutlineThickness(this auto&& Self, f32 InThickness) noexcept
    {
        NODE_FACTORY_SELF().SetTextOutlineThickness(InThickness);
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextOutlineTint(this auto&& Self, LColor const& InTint) noexcept
    {
        NODE_FACTORY_SELF().SetTextOutlineTint(InTint);
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextScale(this auto&& Self, LTextScale InScale) noexcept
    {
        NODE_FACTORY_SELF().SetTextScale(InScale);
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextAlign(this auto&& Self, ETextHAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetTextHAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextAlign(this auto&& Self, ETextVAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetTextVAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextHAlign(this auto&& Self, ETextHAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetTextHAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextVAlign(this auto&& Self, ETextVAlign InAlign) noexcept
    {
        NODE_FACTORY_SELF().SetTextVAlign(InAlign);
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
