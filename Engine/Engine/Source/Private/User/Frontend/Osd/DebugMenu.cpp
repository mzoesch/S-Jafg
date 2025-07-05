// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/DebugMenu.h"
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
            .Tint(LColor::DarkerGray)
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

    LViewport* Viewport { this->GetViewport() };
    if (ensure(Viewport != nullptr) == false)
    {
        return;
    }
    LSurface* Surface { Viewport->GetCachedContext() };
    if (ensure(Surface != nullptr) == false)
    {
        return;
    }

    if (EWidgetVisibility::IsDrawn(InNewVisibility))
    {
        Surface->SetInputMode(EInputMode::Both, true);

        ConstructWidgetNode<WEditorView>(this->GetOuter())->AddToViewport(Viewport);
    }
    else
    {
        Surface->SetInputMode(EInputMode::Both, false);

        while (WEditorView* View { Viewport->GetTopLevelWidgetByClass<WEditorView>() })
        {
            View->RemoveFromParent();
            continue;
        }
    }

    return;
}
