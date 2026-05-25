// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Node.h"
#include "Rhi/Glyph.h"

namespace Jafg
{

class JFontSubsystem;
namespace Detail{ struct TextLocationFn; }

enum struct ETextHAlign : u8 { Left, Center, Right, /* TODO: Justify */ };
enum struct ETextVAlign : u8 { Top, Center, Bottom };

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
    FORCEINLINE constexpr LTextScale(LNodeSize1 InScale) noexcept : Scale{InScale} {}
    FORCEINLINE constexpr LTextScale(LTextScale const& Other) noexcept { this->Scale = Other.Scale; }
    FORCEINLINE constexpr LTextScale& operator=(ETextScale InScale) noexcept { this->Scale = InScale; return *this; }
    FORCEINLINE constexpr LTextScale& operator=(LNodeSize1 InScale) noexcept { this->Scale = InScale; return *this; }
    FORCEINLINE constexpr LTextScale& operator=(LTextScale const& Other) noexcept { this->Scale = Other.Scale; return *this; }

    FORCEINLINE constexpr auto const& GetScale() const noexcept { return this->Scale; }

    inline f32 InStaticPoints(LViewport const& Viewport) const noexcept
    {
        if (std::holds_alternative<LNodeSize1>(this->Scale))
        {
            return std::get<LNodeSize1>(this->Scale).InStaticPoints(Viewport);
        }
        return LTextScale::InStaticPointsImpl(Viewport, std::get<ETextScale>(this->Scale));
    }

    FORCEINLINE constexpr bool operator==(LTextScale const& Other) const noexcept
    {
        if (this->Scale.index() != Other.Scale.index()) { return false; }
        if (std::holds_alternative<LNodeSize1>(this->Scale))
        {
            return std::get<LNodeSize1>(this->Scale) == std::get<LNodeSize1>(Other.Scale);
        }
        return std::get<ETextScale>(this->Scale) == std::get<ETextScale>(Other.Scale);
    }

private:

    ENGINE_API static f32 InStaticPointsImpl(LViewport const& Viewport, ETextScale TextScale) noexcept;
    std::variant<ETextScale, LNodeSize1> Scale;
};

struct LTextBrushBase
{
    LTextScale TextScale{ ETextScale::Body };
    //# Vertical text tightening of the ascender and descender in percent.
    f32 Tightening{ 1.0f };

    ETextHAlign TextHAlign{ ETextHAlign::Left };
    ETextVAlign TextVAlign{ ETextVAlign::Top };

    constexpr bool IsTextLeftAligned() const noexcept { return this->TextHAlign == ETextHAlign::Left; }
    constexpr bool IsTextHCenterAligned() const noexcept { return this->TextHAlign == ETextHAlign::Center; }
    constexpr bool IsTextRightAligned() const noexcept { return this->TextHAlign == ETextHAlign::Right;  }
    constexpr bool IsTextTopAligned() const noexcept { return this->TextVAlign == ETextVAlign::Top; }
    constexpr bool IsTextVCenterAligned() const noexcept { return this->TextVAlign == ETextVAlign::Center; }
    constexpr bool IsTextBottomAligned() const noexcept { return this->TextVAlign == ETextVAlign::Bottom; }
};

struct LRenderData final
{
    constexpr void Dirty() noexcept { bDirty = true; }

    bool bDirty{ true };
    f32 FontSize{};
    LGlyphCollection Collection;
    LVec2F DesiredSize{ maths::zero_vector<LVec2F>};

    ENGINE_API void Update(LViewport const& Viewport, JFontSubsystem const& Subsystem, LTextBrushBase const& Brush, LStringView Text);
};

//# A building block that combines many aspects that are need for mutable text containers. Completely optional.
template<typename TNode>
struct TMutableTextContainer
{
    friend Detail::TextLocationFn;

    constexpr TMutableTextContainer(TNode& Owner) noexcept : Owner{Owner} {}
    virtual ~TMutableTextContainer() = default;

    inline constexpr LVec2F GetTextDesiredSize() const noexcept { return this->RenderData.DesiredSize; }

    TEvent<TMutableTextContainer<TNode>, void(LString const& NewContent)> OnChanged;

    FORCEINLINE void EmptyContent() { algo::swap_default(&this->Content); this->OnChangedImpl(); }
    FORCEINLINE void SetContent(LString InContent) { this->Content = std::move(InContent); this->OnChangedImpl(); }
    FORCEINLINE constexpr LString const& GetContent() const noexcept { return this->Content; }

    //# Offset is only for drawing and does not affect the desired size in any way.
    mutable LVec2F TextDrawOffset{ maths::zero_vector<LVec2F> };
    //# Reduction of the playroom. For rendering only. Does not affect the desired size in any way.
    mutable LVec2F TextPlayroomReduction{ maths::zero_vector<LVec2F> };

protected:

    NODISCARD FORCEINLINE LRenderData& GetTextRenderData() const noexcept { return this->RenderData; }

    template<auto Proj>
    NODISCARD FORCEINLINE bool IsTextRenderDataDirty() const noexcept
    {
        if (!this->LastTextScale.has_value() || *this->LastTextScale != (this->Owner.*Proj).TextScale)
        {
            this->LastTextScale = (this->Owner.*Proj).TextScale;
            this->RenderData.bDirty = true;
        }
        return this->IsTextRenderDataDirtyNoCheck();
    }
    NODISCARD FORCEINLINE bool IsTextRenderDataDirtyNoCheck() const noexcept { return this->RenderData.bDirty; }

    FORCEINLINE std::pair<LString&, algo::raii_leave> GetMutableTextContent() noexcept
    {
        return {this->Content, algo::raii_leave{std::bind(&TMutableTextContainer::OnChangedImpl, this)}};
    }

    //# Called if the text content was changed.
    virtual void OnChangedImpl() noexcept
    {
        this->RenderData.Dirty();
        this->OnChanged.InvokeIfBound(this->Content);
    }

    TNode& Owner;
    LString Content;
    mutable std::optional<LTextScale> LastTextScale;
    mutable LRenderData RenderData;
};

template<typename TNode, auto BrushProj>
struct TFactoryMutableTextContainer : NODE_FACTORY_PARENT(TNode)
{
    NODE_FACTORY_BODY(TNode)

    constexpr decltype(auto) Content(this auto&& Self, LString Content) noexcept
    {
        NODE_FACTORY_SELF().SetContent(std::move(Content));
        return NODE_FACTORY_RESULT();
    }

    constexpr decltype(auto) TextScale(this auto&& Self, LTextScale Scale) noexcept
    {
        check(!Self._IsDecommissioned())
        (DETAIL_JAFG_NODE_FACTORY_SELF().*BrushProj).TextScale = Scale;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextTightening(this auto&& Self, f32 Tightening) noexcept
    {
        check(!Self._IsDecommissioned())
        (DETAIL_JAFG_NODE_FACTORY_SELF().*BrushProj).Tightening = Tightening;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextAlign(this auto&& Self, ETextHAlign Align) noexcept
    {
        check(!Self._IsDecommissioned())
        (DETAIL_JAFG_NODE_FACTORY_SELF().*BrushProj).TextHAlign = Align;
        return NODE_FACTORY_RESULT();
    }
    constexpr decltype(auto) TextAlign(this auto&& Self, ETextVAlign Align) noexcept
    {
        check(!Self._IsDecommissioned())
        (DETAIL_JAFG_NODE_FACTORY_SELF().*BrushProj).TextVAlign = Align;
        return NODE_FACTORY_RESULT();
    }
};

namespace Detail
{
struct TextLocationFn
{
    template<typename T, auto Proj = &T::TextBrush> requires std::is_base_of_v<WNode, T>
    NODISCARD FORCEINLINE LVec2F GetRelativeTopLeft(LVec2F Max, TMutableTextContainer<T> const& Who) const noexcept
    {
        LVec2F PlayRoom{Max - Who.RenderData.DesiredSize - Who.TextDrawOffset - Who.TextPlayroomReduction};
        return Who.TextDrawOffset + LVec2F{
            (Who.Owner.*Proj).IsTextLeftAligned() ? 0.0f : ((Who.Owner.*Proj).IsTextHCenterAligned() ? PlayRoom.x * 0.5f : PlayRoom.x),
            (Who.Owner.*Proj).IsTextTopAligned()  ? 0.0f : ((Who.Owner.*Proj).IsTextVCenterAligned() ? PlayRoom.y * 0.5f : PlayRoom.y)
            };
    }
};
} /* ~Namespace Detail */
inline constexpr Detail::TextLocationFn TextLocation{};

} /* ~Namespace Jafg */
