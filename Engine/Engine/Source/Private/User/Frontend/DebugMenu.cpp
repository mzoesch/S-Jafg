// Copyright mzoesch. All rights reserved.

#include "User/Frontend/DebugMenu.h"
#include "Platform/SurfaceGlfw3.h"
#include "Widgets/EditorView.h"
#include "Widgets/Region.h"
#include "Widgets/VRegion.h"
#include "Widgets/TextBox.h"
#include "Widgets/Viewport.h"
#include "Widgets/Blueprint/Input_Vector3.h"

void Jafg::WDebugMenu::Construct()
{
    Super::Construct();

    LTextBoxBrush Brush { LTextBoxBrush::Body() };
    Brush.Tint = { 0, 0, 0, 128 };

    MakeRootNode(WRegion)
        .Anchor(EAnchor::Fill)
    [
        NewNode(WVRegion)
            .Anchor(EAnchor::TopRight)
            .Type(ERegionBrush::Box)
            .Tint(Lal::LColor::DarkerGray)
        [
            NewNode(WInput_Vector3)
                .HSpace(1.0f)
                .DisplayName("Sun Direction")
        ]
    ]
    FinishWidgetStyling()

    return;
}

void Jafg::WDebugMenu::Tick()
{
    Super::Tick();


    return;
}

void Jafg::WDebugMenu::OnVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility)
{
    Super::OnVisibilityChanged(InOldVisibility, InNewVisibility);

    LSurface& Surface { this->GetViewport().GetSurface() };

    if (EWidgetVisibility::IsDrawn(InNewVisibility))
    {
        Surface.SetInputMode(EInputMode::Both | EInputMode::ShowMouseCursor);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        ConstructWidgetNode<WEditorView>(this->GetOuter())->AddToViewport(&this->GetViewport());
#pragma clang diagnostic pop
    }
    else
    {
        Surface.SetInputMode(EInputMode::Both);

        while (WEditorView* View { this->GetViewport().GetTopLevelWidgetByClass<WEditorView>() })
        {
            View->RemoveFromParent();
            continue;
        }
    }

    return;
}
