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
    FORCEINLINE constexpr LTextScale() noexcept = delete;
    FORCEINLINE constexpr LTextScale(ETextScale InScale) noexcept : Scale{InScale} {}
    FORCEINLINE constexpr LTextScale(LWidgetSize1 InScale) noexcept : Scale{InScale} {}
    FORCEINLINE constexpr LTextScale(LTextScale const& Other) noexcept { this->Scale = Other.Scale; }
    FORCEINLINE constexpr LTextScale& operator=(ETextScale InScale) noexcept { this->Scale = InScale; return *this; }
    FORCEINLINE constexpr LTextScale& operator=(LWidgetSize1 InScale) noexcept { this->Scale = InScale; return *this; }
    FORCEINLINE constexpr LTextScale& operator=(LTextScale const& Other) noexcept { this->Scale = Other.Scale; return *this; }

    FORCEINLINE constexpr auto const& GetScale() const noexcept { return this->Scale; }

    inline f32 InSpt(LViewport const& Viewport) const noexcept
    {
        if (std::holds_alternative<LWidgetSize1>(this->Scale))
        {
            return Jafg::InSpt(Viewport, std::get<LWidgetSize1>(this->Scale));
        }
        return LTextScale::InSptImpl(Viewport, std::get<ETextScale>(this->Scale));
    }

    FORCEINLINE constexpr bool operator==(LTextScale const& Other) const noexcept
    {
        if (this->Scale.index() != Other.Scale.index()) { return false; }
        if (std::holds_alternative<LWidgetSize1>(this->Scale))
        {
            return std::get<LWidgetSize1>(this->Scale) == std::get<LWidgetSize1>(Other.Scale);
        }
        return std::get<ETextScale>(this->Scale) == std::get<ETextScale>(Other.Scale);
    }

private:

    ENGINE_API static f32 InSptImpl(LViewport const& Viewport, ETextScale TextScale) noexcept;
    std::variant<ETextScale, LWidgetSize1> Scale;
};

struct LTextBrush
{
    LTextScale TextScale{ ETextScale::Body };

    LColor Tint{ Colors::White };

    bool bSkipBrushDraw{};

    //# Vertical text tightening of the ascender and descender in percent.
    f32 Tightening{ 1.0f };

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
        this->Brush.Tint = Colors::Black;
        this->Brush.Padding = {4_spt, 0};
    }

public:

    virtual void Draw(LNodeRenderInfo const& Info) const override;
    virtual void UpdateDesiredSize() const override;

    //# Called if the text box content changes.
    EVENT_DECL(OnChanged, void(LString const& NewContent))
    FORCEINLINE void EmptyContent() { algo::swap_default(&this->Content); this->RenderData.Dirty(); this->OnChanged.InvokeIfBound(this->Content); }
    FORCEINLINE void SetContent(LString InContent) { this->Content = std::move(InContent); this->RenderData.Dirty(); this->OnChanged.InvokeIfBound(this->Content); }
    FORCEINLINE constexpr LString const& GetContent() const noexcept { return this->Content; }

    LTextBrush TextBrush;
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
    mutable TOptional<LTextScale> LastTextScale;
    mutable LVec2F DrawOffset{};
    mutable LVec2F TextDesiredSize{};

    mutable struct
    {
        constexpr void Dirty() noexcept { bDirty = true; }

        bool bDirty{ true };
        f32 FontSize{};
        LGetGlyphInfosResult Result;
        LVec2F DesiredSize;
    } RenderData;
};

struct LFactoryTextBox : NODE_FACTORY_PARENT(WTextBox)
{
    NODE_FACTORY_BODY(WTextBox)

    constexpr decltype(auto) Content(this auto&& Self, LString Content) noexcept
    {
        NODE_FACTORY_SELF().SetContent(std::move(Content));
        return NODE_FACTORY_RESULT();
    }

    constexpr decltype(auto) TextScale(this auto&& Self, LTextScale Scale) noexcept
    {
        NODE_FACTORY_SELF().TextBrush.TextScale = Scale;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextBrush(this auto&& Self, LTextBrush const& Brush) noexcept
    {
        NODE_FACTORY_SELF().TextBrush = Brush;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextTint(this auto&& Self, LColor const& Tint) noexcept
    {
        NODE_FACTORY_SELF().TextBrush.Tint = Tint;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) SkipTextBrushDraw(this auto&& Self, bool bSkip) noexcept
    {
        NODE_FACTORY_SELF().TextBrush.bSkipBrushDraw = bSkip;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextTightening(this auto&& Self, f32 Tightening) noexcept
    {
        NODE_FACTORY_SELF().TextBrush.Tightening = Tightening;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextOutlineThickness(this auto&& Self, f32 Thickness) noexcept
    {
        NODE_FACTORY_SELF().TextBrush.OutlineThickness = Thickness;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextOutlineTint(this auto&& Self, LColor const& Tint) noexcept
    {
        NODE_FACTORY_SELF().TextBrush.OutlineTint = Tint;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextAlign(this auto&& Self, ETextHAlign Align) noexcept
    {
        NODE_FACTORY_SELF().TextBrush.TextHAlign = Align;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextAlign(this auto&& Self, ETextVAlign Align) noexcept
    {
        NODE_FACTORY_SELF().TextBrush.TextVAlign = Align;
        return NODE_FACTORY_RESULT();
    }
};

} /* ~Namespace Jafg */
