// Copyright mzoesch. All rights reserved.

#pragma once

#include "Nodes/Node.h"

namespace Jafg
{

enum struct ENodePrimitiveControlflow{ Stacked, Horizontal, Vertical, };

NLOHMANN_JSON_SERIALIZE_ENUM(ENodePrimitiveControlflow, {
    {ENodePrimitiveControlflow::Stacked, "Stacked"},
    {ENodePrimitiveControlflow::Horizontal, "Horizontal"},
    {ENodePrimitiveControlflow::Vertical, "Vertical"},
    })

namespace Detail
{

struct StackedControlFlowFn
{
    FORCEINLINE LVec2F UpdateDesiredSize(WParent const& Self) const noexcept
    {
        LVec2F DesiredSize{maths::zero_vector<LVec2F>};
        for (auto& Child : Self.GetChildren())
        {
            LVec2F ChildDesiredSize{Child->GetDesiredSize_v2()};
            DesiredSize.x = maths::max(DesiredSize.x, ChildDesiredSize.x);
            DesiredSize.y = maths::max(DesiredSize.y, ChildDesiredSize.y);
        }
        DesiredSize += Self.Padding.GetDesiredSize().InStaticPoints(Self.GetViewport());
        return DesiredSize;
    }

    FORCEINLINE LVec2F GetAnchoredSizeForChild(WParent const& Self, WNode const& Target) const noexcept
    {
        check(Target.TransformsWidgetLayout())
        return{
            maths::max(
                Target.GetDesiredSize_v2().x,
                Target.Anchor.MaxX * (Self.GetAnchoredSize_v2().x - Self.Padding.GetDesiredSizeX().InStaticPoints(Self.GetViewport()))
                ),
            maths::max(
                Target.GetDesiredSize_v2().y,
                Target.Anchor.MaxY * (Self.GetAnchoredSize_v2().y - Self.Padding.GetDesiredSizeY().InStaticPoints(Self.GetViewport()))
                )
            };
    }

    FORCEINLINE LVec2F GetAnchoredTopLeftFromMostOuterForChild(WParent const& Self, WNode const& Target) const noexcept
    {
        check(Target.TransformsWidgetLayout())
        LVec2F Out{
            Self.Padding.GetLeftOffset().InStaticPoints(Self.GetViewport())
            + Target.Anchor.MinX *
            (
                Self.GetAnchoredSize_v2().x
                - Self.Padding.GetDesiredSizeX().InStaticPoints(Self.GetViewport())
                - Target.GetAnchoredSize_v2().x
            ),
            Self.Padding.GetTopOffset().InStaticPoints(Self.GetViewport())
            + Target.Anchor.MinY *
            (
                Self.GetAnchoredSize_v2().y
                - Self.Padding.GetDesiredSizeY().InStaticPoints(Self.GetViewport())
                - Target.GetAnchoredSize_v2().y
            ),
            };
        Out += Self.GetAnchoredTopLeftFromMostOuter();
        return Out;
    }
};

} /* ~Namespace Detail */
inline constexpr Detail::StackedControlFlowFn StackedControlFlow{};

namespace Detail
{

struct HorizontalControlFlowFn
{
    FORCEINLINE LVec2F UpdateDesiredSize(WParent const& Self, LNodeSize1 Space) const noexcept
    {
        LVec2F DesiredSize{maths::zero_vector<LVec2F>};
        auto TransformerChildCount{0uz};
        for (auto& Child : Self.GetChildren())
        {
            check(Child.get())
            if (Child->TransformsWidgetLayout())
            {
                DesiredSize.x += Child->GetDesiredSize_v2().x;
                DesiredSize.y  = maths::max(DesiredSize.y, Child->GetDesiredSize_v2().y);
                ++TransformerChildCount;
            }
        }
        if (TransformerChildCount > 0)
        {
            DesiredSize.x += Space.InStaticPoints(Self.GetViewport()) * (TransformerChildCount + 1);
        }
        DesiredSize += Self.Padding.GetDesiredSize().InStaticPoints(Self.GetViewport());
        return DesiredSize;
    }

    FORCEINLINE LVec2F GetAnchoredSizeForChild(WParent const& Self, WNode const& Target, LNodeSize1 Space) const noexcept
    {
        check(Target.TransformsWidgetLayout())
        if (!Target.Anchor.IsStretchedHorizontal())
        {
            return StackedControlFlow.GetAnchoredSizeForChild(Self, Target);
        }

        f32 TotalDesiredSize{};
        f32 TotalFreeUsage{}; /* In percent */

        for (auto& Child : Self.GetChildren())
        {
            check(Child.get())
            TotalDesiredSize += Child->GetDesiredSize_v2().x;
            TotalFreeUsage   += Child->Anchor.MaxX;
        }

        const f32 FreeSpace{
            (Self.GetAnchoredSize_v2().x - Self.Padding.GetDesiredSizeX().InStaticPoints(Self.GetViewport()))
            - TotalDesiredSize
            - Space.InStaticPoints(Self.GetViewport()) * (Self.GetChildren().empty() ? 0 : Self.GetChildren().size() + 1)
            };
        const f32 InverseFreeUsage{ 1.0f / TotalFreeUsage };

        return{
            Target.GetDesiredSize_v2().x
          + Target.Anchor.MaxX * InverseFreeUsage * FreeSpace
          , maths::max(
              Target.GetDesiredSize_v2().y,
              Target.Anchor.MaxY * (Self.GetAnchoredSize_v2().y - Self.Padding.GetDesiredSizeY().InStaticPoints(Self.GetViewport()))
              )
          };
    }

    FORCEINLINE LVec2F GetAnchoredTopLeftFromMostOuterForChild(WParent const& Self, WNode const& Target, LNodeSize1 Space) const noexcept
    {
        check(Target.TransformsWidgetLayout())

        auto SpaceSpt{Space.InStaticPoints(Self.GetViewport())};

        f32 Offset{SpaceSpt};
        for (auto& Child : Self.GetChildren())
        {
            check(Child.get())
            if (&*Child == &Target)
            {
                break;
            }
            Offset += Child->GetAnchoredSize_v2().x;
            Offset += SpaceSpt;
            continue;
        }

        LVec2F Out{
            Self.Padding.GetLeftOffset().InStaticPoints(Self.GetViewport())
            + Offset,
            Self.Padding.GetTopOffset().InStaticPoints(Self.GetViewport())
            + Target.Anchor.MinY *
            (
                Self.GetAnchoredSize_v2().y
                - Self.Padding.GetDesiredSizeY().InStaticPoints(Self.GetViewport())
                - Target.GetAnchoredSize_v2().y
            ),
            };
        Out += Self.GetAnchoredTopLeftFromMostOuter();
        return Out;
    }
};

struct VerticalControlFlowFn
{
    FORCEINLINE LVec2F UpdateDesiredSize(WParent const& Self, LNodeSize1 Space) const noexcept
    {
        LVec2F DesiredSize{ maths::zero_vector<LVec2F> };
        auto TransformerChildCount{0uz};
        for (auto& Child : Self.GetChildren())
        {
            check(Child.get())
            if (Child->TransformsWidgetLayout())
            {
                DesiredSize.x  = maths::max(DesiredSize.x, Child->GetDesiredSize_v2().x);
                DesiredSize.y += Child->GetDesiredSize_v2().y;
                ++TransformerChildCount;
            }
        }
        if (TransformerChildCount > 0)
        {
            DesiredSize.y += Space.InStaticPoints(Self.GetViewport()) * (TransformerChildCount + 1);
        }
        DesiredSize += Self.Padding.GetDesiredSize().InStaticPoints(Self.GetViewport());
        return DesiredSize;
    }

    FORCEINLINE LVec2F GetAnchoredSizeForChild(WParent const& Self, WNode const& Target, LNodeSize1 Space) const noexcept
    {
        check(Target.TransformsWidgetLayout())
        if (!Target.Anchor.IsStretchedVertical())
        {
            return StackedControlFlow.GetAnchoredSizeForChild(Self, Target);
        }

        f32 TotalDesiredSize{};
        f32 TotalFreeUsage{}; /* In percent */

        for (auto& Child : Self.GetChildren())
        {
            check(Child.get())
            TotalDesiredSize += Child->GetDesiredSize_v2().y;
            TotalFreeUsage   += Child->Anchor.MaxY;
        }

        const f32 FreeSpace{
            (Self.GetAnchoredSize_v2().y - Self.Padding.GetDesiredSizeY().InStaticPoints(Self.GetViewport()))
            - TotalDesiredSize
            - Space.InStaticPoints(Self.GetViewport()) * (Self.GetChildren().empty() ? 0 : Self.GetChildren().size() + 1)
            };
        const f32 InverseFreeUsage{ 1.0f / TotalFreeUsage };

        return{
            maths::max(
                Target.GetDesiredSize_v2().x,
                Target.Anchor.MaxX * (Self.GetAnchoredSize_v2().x - Self.Padding.GetDesiredSizeX().InStaticPoints(Self.GetViewport()))
                )
            , Target.GetDesiredSize_v2().y
            + Target.Anchor.MaxY * InverseFreeUsage * FreeSpace
        };
    }

    FORCEINLINE LVec2F GetAnchoredTopLeftFromMostOuterForChild(WParent const& Self, WNode const& Target, LNodeSize1 Space) const noexcept
    {
        check(Target.TransformsWidgetLayout())

        auto SpaceSpt{Space.InStaticPoints(Self.GetViewport())};

        f32 Offset{SpaceSpt};
        for (auto& Child : Self.GetChildren())
        {
            check(Child.get())
            if (&*Child == &Target)
            {
                break;
            }
            Offset += Child->GetAnchoredSize_v2().y;
            Offset += SpaceSpt;
            continue;
        }

        LVec2F Out{
            Self.Padding.GetLeftOffset().InStaticPoints(Self.GetViewport())
            + Target.Anchor.MinX *
            (
                Self.GetAnchoredSize_v2().x
                - Self.Padding.GetDesiredSizeX().InStaticPoints(Self.GetViewport())
                - Target.GetAnchoredSize_v2().x
            ),
            Self.Padding.GetTopOffset().InStaticPoints(Self.GetViewport())
            + Offset,
            };
        Out += Self.GetAnchoredTopLeftFromMostOuter();
        return Out;
    }
};

} /* ~Namespace Detail */
inline constexpr Detail::HorizontalControlFlowFn HorizontalControlFlow{};
inline constexpr Detail::VerticalControlFlowFn VerticalControlFlow{};

template<ENodePrimitiveControlflow Cf>
struct StaticControlFlowOrchestration
{
    FORCEINLINE LVec2F UpdateDesiredSize(WParent const& Self, LNodeSize1 Space = {}) const noexcept
    {
        if constexpr (Cf == ENodePrimitiveControlflow::Stacked)
        {
            return StackedControlFlow.UpdateDesiredSize(Self);
        }
        else if constexpr (Cf == ENodePrimitiveControlflow::Horizontal)
        {
            return HorizontalControlFlow.UpdateDesiredSize(Self, Space);
        }
        else if constexpr (Cf == ENodePrimitiveControlflow::Vertical)
        {
            return VerticalControlFlow.UpdateDesiredSize(Self, Space);
        }
        else
        {
            std::unreachable();
        }
    }

    FORCEINLINE LVec2F GetAnchoredSizeForChild(WParent const& Self, WNode const& Target, LNodeSize1 Space = {}) const noexcept
    {
        if constexpr (Cf == ENodePrimitiveControlflow::Stacked)
        {
            return StackedControlFlow.GetAnchoredSizeForChild(Self, Target);
        }
        else if constexpr (Cf == ENodePrimitiveControlflow::Horizontal)
        {
            return HorizontalControlFlow.GetAnchoredSizeForChild(Self, Target, Space);
        }
        else if constexpr (Cf == ENodePrimitiveControlflow::Vertical)
        {
            return VerticalControlFlow.GetAnchoredSizeForChild(Self, Target, Space);
        }
        else
        {
            std::unreachable();
        }
    }

    FORCEINLINE LVec2F GetAnchoredTopLeftFromMostOuterForChild(WParent const& Self, WNode const& Target, LNodeSize1 Space = {}) const noexcept
    {
        if constexpr (Cf == ENodePrimitiveControlflow::Stacked)
        {
            return StackedControlFlow.GetAnchoredTopLeftFromMostOuterForChild(Self, Target);
        }
        else if constexpr (Cf == ENodePrimitiveControlflow::Horizontal)
        {
            return HorizontalControlFlow.GetAnchoredTopLeftFromMostOuterForChild(Self, Target, Space);
        }
        else if constexpr (Cf == ENodePrimitiveControlflow::Vertical)
        {
            return VerticalControlFlow.GetAnchoredTopLeftFromMostOuterForChild(Self, Target, Space);
        }
        else
        {
            std::unreachable();
        }
    }
};

namespace Detail
{

struct DynamicControlFlowOrchestrationFn
{
    FORCEINLINE LVec2F UpdateDesiredSize(ENodePrimitiveControlflow Cf, WParent const& Self, LNodeSize1 Space = {}) const noexcept
    {
        switch (Cf)
        {
        case ENodePrimitiveControlflow::Stacked: return StaticControlFlowOrchestration<ENodePrimitiveControlflow::Stacked>().UpdateDesiredSize(Self, Space);
        case ENodePrimitiveControlflow::Horizontal: return StaticControlFlowOrchestration<ENodePrimitiveControlflow::Horizontal>().UpdateDesiredSize(Self, Space);
        case ENodePrimitiveControlflow::Vertical: return StaticControlFlowOrchestration<ENodePrimitiveControlflow::Vertical>().UpdateDesiredSize(Self, Space);
        default: std::unreachable();
        }
    }

    FORCEINLINE LVec2F GetAnchoredSizeForChild(ENodePrimitiveControlflow Cf, WParent const& Self, WNode const& Target, LNodeSize1 Space = {}) const noexcept
    {
        switch (Cf)
        {
        case ENodePrimitiveControlflow::Stacked: return StaticControlFlowOrchestration<ENodePrimitiveControlflow::Stacked>().GetAnchoredSizeForChild(Self, Target, Space);
        case ENodePrimitiveControlflow::Horizontal: return StaticControlFlowOrchestration<ENodePrimitiveControlflow::Horizontal>().GetAnchoredSizeForChild(Self, Target, Space);
        case ENodePrimitiveControlflow::Vertical: return StaticControlFlowOrchestration<ENodePrimitiveControlflow::Vertical>().GetAnchoredSizeForChild(Self, Target, Space);
        default: std::unreachable();
        }
    }

    FORCEINLINE LVec2F GetAnchoredTopLeftFromMostOuterForChild(ENodePrimitiveControlflow Cf, WParent const& Self, WNode const& Target, LNodeSize1 Space = {}) const noexcept
    {
        switch (Cf)
        {
        case ENodePrimitiveControlflow::Stacked: return StaticControlFlowOrchestration<ENodePrimitiveControlflow::Stacked>().GetAnchoredTopLeftFromMostOuterForChild(Self, Target, Space);
        case ENodePrimitiveControlflow::Horizontal: return StaticControlFlowOrchestration<ENodePrimitiveControlflow::Horizontal>().GetAnchoredTopLeftFromMostOuterForChild(Self, Target, Space);
        case ENodePrimitiveControlflow::Vertical: return StaticControlFlowOrchestration<ENodePrimitiveControlflow::Vertical>().GetAnchoredTopLeftFromMostOuterForChild(Self, Target, Space);
        default: std::unreachable();
        }
    }
};

} /* ~Namespace Detail */
inline constexpr Detail::DynamicControlFlowOrchestrationFn DynamicControlFlowOrchestration{};

} /* ~Namespace Jafg */
