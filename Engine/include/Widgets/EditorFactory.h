// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/App.h"
#include "User/UserPreferences.h"
#include "Nodes/Node.h"
#include "Nodes/Parent.h"
#include "Nodes/VParent.h"
#include "Nodes/HParent.h"
#include "Nodes/Overlay.h"
#include "Nodes/TabOverlay.h"
#include "Nodes/Region.h"
#include "Nodes/Button.h"
#include "Nodes/Text.h"
#include "Nodes/EditableTextButton.h"
#include "Framework/Frontend.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/Editor.h"
#include "Widgets/TagInspector.h"
#include "Widgets/ColorInspector.h"
#include "Widgets/ClassInspector.h"
#include "Widgets/WorldViewer.h"
#include "Widgets/EditorFinder.h"
#include "Widgets/Input_Vector3.h"
#include "Widgets/AssetInspectors.h"
#include "Engine/Engine.h"

namespace Jafg
{

struct LEditorRowState
{
    constexpr LEditorRowState() noexcept = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LEditorRowState)

    typedef std::variant<TButtonStyle<LRegionBrush>*, TButtonStyle<LBoxBrush>*> StyleVariant;
    typedef std::variant<LRegionBrush*, LBoxBrush*> BrushVariant;

    std::size_t FrameCount{};
    EStyleBits Bit{EStyleBits::Identity};
    TArray<std::tuple<StyleVariant, BrushVariant, TFunction2<bool()>>> Pairs;

    template<typename T>
    static decltype(auto) Lambda(auto&& SharedState) noexcept
    {
        return [SharedState](auto&& F)
        {
            T& Button{*F.GetRawNode().template AsChecked<T>()};
            SharedState->EmplaceNodes(Button, SharedState, [&Button]{ return Button.IsEnabled(); });
        };
    }

    template<typename T>
        requires (!std::is_const_v<T> && (std::is_base_of_v<WTextButton, T> || std::is_base_of_v<WButton, T>))
    void EmplaceNodes(T& Node, auto&& SharedState, auto&& EnabledFunctor) noexcept
    {
        check(GEngine)
        check(!Node.OnBrushChangedEvent.IsValid())

        this->Pairs.emplace_back(&Node.Style, &Node.Brush, std::forward<decltype(EnabledFunctor)>(EnabledFunctor));
        Node.OnBrushChangedEvent = [this, SharedState](auto& Self, Jafg::EStyleBits Bit)
        {
            (void)SharedState;

            if (this->FrameCount < GEngine->FrameCount || Bit != Jafg::EStyleBits::Normal)
            {
                this->FrameCount = GEngine->FrameCount;
                this->Bit = Bit;
                for (auto& [StyleVariant, BrushVariant, Enabled]: this->Pairs)
                {
                    check(!!Enabled)
                    std::visit([&Self, &Enabled, Bit](auto&& StylePtr, auto&& BrushPtr)
                    {
                        if (static_cast<void*>(&Self.Brush) != static_cast<void*>(BrushPtr) && Enabled())
                        {
                            if constexpr ((std::same_as<std::remove_cvref_t<std::decay_t<decltype(StylePtr)>>, Jafg::TButtonStyle<Jafg::LRegionBrush>*>
                                    && std::same_as<std::remove_cvref_t<std::decay_t<decltype(BrushPtr)>>, Jafg::LRegionBrush*>)
                                || (std::same_as<std::remove_cvref_t<std::decay_t<decltype(StylePtr)>>, Jafg::TButtonStyle<Jafg::LBoxBrush>*>
                                    && std::same_as<std::remove_cvref_t<std::decay_t<decltype(BrushPtr)>>, Jafg::LBoxBrush*>))
                            {
                                Jafg::ApplyStyleBit(*StylePtr, *BrushPtr, Bit);
                            }
                            else
                            {
                                std::unreachable();
                            }
                        }
                    }, StyleVariant, BrushVariant);
                }
            }
            else
            {
                Jafg::ApplyStyleBit(Self.Style, Self.Brush, Bit);
            }
        };

        return;
    }
};

template<typename T = LEditorRowState>
inline LFactoryTextButton EditorComponentLabel(LViewport& Viewport, LString Content, auto SharedState)
{
    auto& Prefs{GetSingleton<JUserPreferences>()};
    return NewNode(Viewport).Class<WTextButton>()
        .MinDesiredSize({128_spt, 0})
        .Anchor(EAnchor::VFill)
        .InBrushChained<EStyleBits::ActiveCombi, &LBoxBrush::Tint, &LBoxBrush::Padding>(*Prefs.ForegroundColorVariant, {20_spt, 0, 0, 0})
        .InBrushChained<EStyleBits::InactiveCombi, &LBoxBrush::Tint, &LBoxBrush::Padding>(*Prefs.ForegroundColor, {20_spt, 0, 0, 0})
        .InTextBrushChained<EStyleBits::Disabled, &LTextBoxBrush::Tint>(Colors::Gray)
        .InAllTextBrushes<&LTextBoxBrush::TextVAlign>(ETextVAlign::Center)
        .Content(Content)
        .Delegate(T::template Lambda<WTextButton>(std::move(SharedState)));
}

template<typename T = LEditorRowState>
inline LFactoryButton EditorComponentContentWrapper(LViewport& Viewport, auto SharedState)
{
    auto& Prefs{GetSingleton<JUserPreferences>()};
    return NewNode(Viewport).Class<WButton>()
        .Anchor(EAnchor::HFill)
        .Visibility(ENodeVisibility::Visible)
        .Padding({12_spt, 3})
        .InBrush<EStyleBits::ActiveCombi, &LRegionBrush::Tint>(*Prefs.ForegroundColorVariant)
        .InBrush<EStyleBits::InactiveCombi, &LRegionBrush::Tint>(*Prefs.ForegroundColor)
        .Delegate(T::template Lambda<WButton>(SharedState));
}

inline LFactoryButton EditorResetButton(LViewport& Viewport, bool bEnabled, TFunction2<void(WButton& Self)> OnAction, auto&& Delegate)
{
    auto& Prefs{GetSingleton<JUserPreferences>()};
    return NewNode(Viewport).Class<WButton>()
        .Anchor(EAnchor::VFill)
        .MinDesiredSize({24_spt, 20})
        .InBrush<EStyleBits::ActiveCombi, &LRegionBrush::BorderTint>(*Prefs.ForegroundColorVariant)
        .InBrush<EStyleBits::InactiveCombi, &LRegionBrush::BorderTint>(*Prefs.ForegroundColor)
        .InBrush<EStyleBits::ActiveCombi|EStyleBits::Normal, &LRegionBrush::Tint>(Colors::White)
        .InBrush<EStyleBits::Disabled, &LRegionBrush::Tint>(Colors::Gray)
        .InAllBrushes<&LRegionBrush::Background>(LRegionBrush::Icon("Icons/Jafg.Reset"))
        .Enabled(bEnabled)
        .Delegate(Delegate)
        .OnKeyEventFocused([OnAction=std::move(OnAction)](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event) mutable
        {
            if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::LeftMouseButton)))
            {
                check(!!OnAction)
                OnAction(Self.AsStatic<WButton>());
                return LNodeReply::Handled();
            }
            return LNodeReply::Unhandled();
        });
}

} /* ~Namespace Jafg */
