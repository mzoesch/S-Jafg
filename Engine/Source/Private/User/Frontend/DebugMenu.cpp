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

    LTextBoxBrush Brush{LTextBoxBrush::Body()};
    Brush.Tint = { 0, 0, 0, 128 };

    BeginStyling(*this).Root<WRegion>()
        .Anchor(EAnchor::Fill)
    [
        NewNode(this->GetViewport()).Class<WVRegion>()
            .Anchor(EAnchor::TopRight)
            .Type(ERegionBrush::Box)
            .Tint(Colors::DarkerGray)
        [
            NewNode(this->GetViewport()).Class<WInput_Vector3>()
                .HSpace(1.0f)
                .DisplayName("Sun Direction")
        ]
    ];

    return;
}

void Jafg::WDebugMenu::OnVisibilityChanged(ENodeVisibility OldVisibility, ENodeVisibility NewVisibility)
{
    Super::OnVisibilityChanged(OldVisibility, NewVisibility);

    LSurface& Surface{this->GetViewport().GetSurface()};

    if (NodeVisibility::IsDrawn(NewVisibility))
    {
        Surface.SetInputMode(EInputMode::Both | EInputMode::ShowMouseCursor);
        check(this->GetViewport().GetTopLevelWidgetByClass<WEditorView>() == nullptr)
        ConstructWidget(TWidgetStaticInit<WEditorView>{.Outer=this->GetViewport()});
    }
    else
    {
        Surface.SetInputMode(EInputMode::Both);
        while (WEditorView* View{this->GetViewport().GetTopLevelWidgetByClass<WEditorView>()})
        {
            View->RemoveFromParent();
        }
    }

    return;
}
