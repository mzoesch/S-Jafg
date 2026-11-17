// Copyright mzoesch. All rights reserved.

#include "Widgets/Input_List.h"
#include "Nodes/DismissibleFloatingWidget.h"
#include "Nodes/TextButton.h"
#include "User/UserPreferences.h"

void Jafg::WInput_List::CtorLogic()
{
    auto& Prefs{GetSingleton<JUserPreferences>()};

    this->SetVisibility(ENodeVisibility::DerivedHitTestInvisible);

    Prefs.InputPaletteSolid->ApplyOn(this->Style);
    Prefs.InputPaletteSolid->ApplyOn(this->TextStyle);

    this->Style.SetEverywhere<&LBoxBrush::Padding>(Prefs.InputPadding);

    this->RightIcon = LTexture2::FromTextureView(*JafgInfo, "Icons/ArrowDown");
    this->RightIconStyle.ChainEverywhere<&LIconBrush::bAlwaysPad, &LIconBrush::Alignment>(true, LIconBrush::Align::Right);
    this->RightIconStyle.Set<ETextButtonIconStyleBits::Disabled, &LIconBrush::Tint>(Colors::Gray);
}

Jafg::LNodeReply Jafg::WInput_List::OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    auto Reply{Super::OnKeyEventFocused(Info, Event)};

    if (Event.Is<ERawInputStateBits::Press>(ELogicalKey::LeftMouseButton))
    {
        if (this->DeferredWidget.IsValidFast())
        {
            this->DeferredWidget->RemoveFromParent2();
            this->DeferredWidget.Reset();
        }
        else
        {
            ConstructDeferredWidget(Jafg::TNodeStaticInit<WDismissibleFloatingWidget>{Info.Viewport}).Style().SaveTo(&this->DeferredWidget)
                .Decorate(false)
                .CreateResizeUi(false)
                .InitialWindowPosition(
                    this->GetAnchoredAndTranslatedTopLeftFromMostOuter(this->GetTranslationFromMostOuter())
                    + LVec2F{0,this->GetAnchoredSize_v2().y}
                    )
                .InitialWindowSize({100_spt, 20})
                .Content([this](WFloatingWidget& FloatingWidget, WParent& Container)
                {
                    auto& Prefs{GetSingleton<JUserPreferences>()};

                    WParent* Target;
                    Container.AddChild(NewNode(Container.GetViewport()).Class<WVRegion>().SaveTo(&Target)
                        .Padding(1_spt)
                        .Tint(*Prefs.ForegroundColor)
                        .OutlineThickness(1)
                        .OutlineTint(Colors::White)
                        .Unique());

                    for (auto& Item: this->Items)
                    {
                        Target->AddChild(NewNode(Target->GetViewport()).Class<WTextButton>()
                            .Anchor(EAnchor::HFill)
                            .Palette(*Prefs.PrimaryPaletteSolid)
                            .InBrush<EStyleBits::InactiveCombi, &LBoxBrush::bSkipBrushDraw>(true)
                            .InAllBrushes<&LBoxBrush::Padding>(Prefs.InputPadding)
                            .Content(Item)
                            .OnKeyEventFocused([this,Item,FloatingWidget=&FloatingWidget](WNode& Self, LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
                            {
                                if (Info.CursorLocation && Self.AabbTest({.Translation=Info.Translation}, *Info.CursorLocation))
                                {
                                    if (Event.Is<ERawInputStateBits::Release>(ELogicalKey::LeftMouseButton))
                                    {
                                        this->SetContent(Item);
                                        (void)this->OnValueChanged.InvokeIfBound(this->GetContent());
                                        FloatingWidget->DestroyFloatingWidgetControlled();
                                        return LNodeReply::Handled();
                                    }
                                }
                                return LNodeReply::Unhandled();
                            })
                            .Unique());
                    }
                });
        }

        if (Reply.IsHandled())
        {
            return Reply;
        }
        return LNodeReply::Handled();
    }

    if (Reply.IsHandled())
    {
        return Reply;
    }
    return LNodeReply::Unhandled();
}
