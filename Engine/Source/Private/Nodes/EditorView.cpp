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

Jafg::LNodeReply Jafg::WEditorView::OnKeyDownFocused(LNodeKeyEventInfo const& Data, LKeyEvent const& InKeyEvent)
{
    if (InKeyEvent.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
    {
        this->OnSecondaryDown();
    }

    return Super::OnKeyDownFocused(Data, InKeyEvent);
}

Jafg::LNodeReply Jafg::WEditorView::OnKeyDownUnfocused(LNodeKeyEventInfo const& Data, LKeyEvent const& InKeyEvent)
{
    if (InKeyEvent.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
    {
        this->OnSecondaryDown();
    }

    return Super::OnKeyDownUnfocused(Data, InKeyEvent);
}

Jafg::LNodeReply Jafg::WEditorView::OnKeyUpFocused(LNodeKeyEventInfo const& Data, LKeyEvent const& InKeyEvent)
{
    if (InKeyEvent.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
    {
        this->OnSecondaryUp();
    }

    return Super::OnKeyUpFocused(Data, InKeyEvent);
}

Jafg::LNodeReply Jafg::WEditorView::OnKeyUpUnfocused(LNodeKeyEventInfo const& Data, LKeyEvent const& InKeyEvent)
{
    if (InKeyEvent.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton))
    {
        this->OnSecondaryUp();
    }

    return Super::OnKeyUpUnfocused(Data, InKeyEvent);
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
