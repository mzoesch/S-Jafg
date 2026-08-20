// Copyright mzoesch. All rights reserved.

#include "Widgets/Input_List.h"
#include "Nodes/DismissibleFloatingWidget.h"
#include "Nodes/TextButton.h"
#include "User/UserPreferences.h"

void Jafg::WInput_List::Construct()
{
    Super::Construct();

    if (!this->Icon.get())
    {
        this->Icon = LTexture2::FromAsset("Icons/Jafg.ArrowDown");
        this->IconBrush.bAlwaysPad = true;
        this->IconBrush.Alignment = LIconBrush::Align::Right;
    }
}

Jafg::LNodeReply Jafg::WInput_List::OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    if (auto Reply{Super::OnKeyEventFocused(Info, Event)}; Reply.IsHandled())
    {
        return Reply;
    }

    if (Event.Is<ERawInputStateBits::Press>(ELogicalKey::LeftMouseButton))
    {
        ConstructDeferredWidget(Jafg::TNodeStaticInit<WDismissibleFloatingWidget>{Info.Viewport}).Style()
            .Decorate(false)
            .CreateResizeUi(false)
            .InitialWindowPosition(Info.CursorLocation ? *Info.CursorLocation : maths::zero_vector<LVec2F>)
            .InitialWindowSize({100_spt, 20})
            .Content([this](WFloatingWidget& FloatingWidget, WParent& Container)
            {
                auto& Prefs{GetSingleton<JUserPreferences>()};

                WParent* Target;
                Container.AddChild(NewNode(Container.GetViewport()).Class<WVRegion>().SaveTo(&Target)
                    .Padding({5_pt, 2})
                    .Tint(*Prefs.ForegroundColor)
                    .OutlineThickness(1)
                    .OutlineTint(Colors::White)
                    .Unique());

                for (auto& Item: this->Items)
                {
                    Target->AddChild(NewNode(Target->GetViewport()).Class<WTextButton>()
                        .Anchor(EAnchor::HFill)
                        .InBrush<EStyleBits::Normal|EStyleBits::Disabled, &LBoxBrush::bSkipBrushDraw>(true)
                        .InBrush<EStyleBits::Hover, &LBoxBrush::Tint>(*Prefs.PrimaryColor)
                        .InBrush<EStyleBits::Press, &LBoxBrush::Tint>(*Prefs.PrimaryColorVariant)
                        .InAllBrushes<&LBoxBrush::Padding>({5_spt, 0})
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

        return LNodeReply::Handled();
    }

    return LNodeReply::Unhandled();
}
