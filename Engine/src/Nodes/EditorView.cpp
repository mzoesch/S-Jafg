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

Jafg::LNodeReply Jafg::WEditorView::OnKeyEventFocused(LNodeKeyEventInfo const& Info, LKeyEvent const& Event)
{
    if (Event.Is<ERawInputStateBits::Press>(LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton)))
    {
        this->OnSecondaryDown();
    }
    else if (Event.Is<ERawInputStateBits::Release>(LPhysicalKey::FromLogical(ELogicalKey::RightMouseButton)))
    {
        this->OnSecondaryUp();
    }

    return Super::OnKeyEventFocused(Info, Event);
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
