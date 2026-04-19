// Copyright mzoesch. All rights reserved.

#include "Nodes/EditorView.h"
#include "Platform/SurfaceGlfw3.h"
#include "User/LocalEgo.h"
#include "Nodes/Viewport.h"

void Jafg::WEditorView::Tick()
{
    Super::Tick();

    if (this->GetViewport().GetSurface()
        .HasConsumableKeyState(LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton), ERawInputStateBits::Release))
    {
        this->OnSecondaryUp();
    }

    return;
}

Jafg::LReply Jafg::WEditorView::OnKeyDown(LNodeKeyEventData const& Data, LKeyEvent const& InKeyEvent)
{
    if (InKeyEvent.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
    {
        this->OnSecondaryDown();
    }

    return Super::OnKeyDown(Data, InKeyEvent);
}

Jafg::LReply Jafg::WEditorView::OnKeyDownNoFocus(LNodeKeyEventData const& Data, LKeyEvent const& InKeyEvent)
{
    if (InKeyEvent.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
    {
        this->OnSecondaryDown();
    }

    return Super::OnKeyDownNoFocus(Data, InKeyEvent);
}

Jafg::LReply Jafg::WEditorView::OnKeyUp(LNodeKeyEventData const& Data, LKeyEvent const& InKeyEvent)
{
    if (InKeyEvent.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
    {
        this->OnSecondaryUp();
    }

    return Super::OnKeyUp(Data, InKeyEvent);
}

Jafg::LReply Jafg::WEditorView::OnKeyUpNoFocus(LNodeKeyEventData const& Data, LKeyEvent const& InKeyEvent)
{
    if (InKeyEvent.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
    {
        this->OnSecondaryUp();
    }

    return Super::OnKeyUpNoFocus(Data, InKeyEvent);
}

void Jafg::WEditorView::OnSecondaryDown()
{
    this->GetViewport().GetSurface().SetInputMode(EInputModeBits::HideMouseCursor);
    this->SetShouldTick(true);

    // LUserInput& Input{ this->GetLocalEgo().GetUserInput() };
    // Input.SetReferenceContexts(Input.GetActiveContexts());
    // Input.PopContexts();

    return;
}

void Jafg::WEditorView::OnSecondaryUp()
{
    this->GetViewport().GetSurface().SetInputMode(EInputModeBits::ShowMouseCursor);
    this->SetShouldTick(false);
    //
    // LUserInput& Input{ this->GetLocalEgo().GetUserInput() };
    // Input.PushContexts();
    // Input.ActivateContexts(Input.GetReferenceContexts());
    // Input.SetReferenceContexts({});

    return;
}
