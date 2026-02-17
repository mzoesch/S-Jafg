// Copyright mzoesch. All rights reserved.

#include "Widgets/EditorView.h"
#include "Platform/SurfaceGlfw3.h"
#include "User/LocalEgo.h"
#include "Widgets/Viewport.h"

void Jafg::WEditorView::Tick()
{
    Super::Tick();

    if (this->GetViewport().GetSurface().IsKeyUp(EKeys::RightMouseButton))
    {
        this->OnSecondaryUp();
    }

    return;
}

Jafg::LReply Jafg::WEditorView::OnKeyDown(LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::RightMouseButton)
    {
        this->OnSecondaryDown();
    }

    return Super::OnKeyDown(InViewport, InKeyEvent);
}

Jafg::LReply Jafg::WEditorView::OnKeyDownNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::RightMouseButton)
    {
        this->OnSecondaryDown();
    }

    return Super::OnKeyDownNoFocus(InViewport, InKeyEvent);
}

Jafg::LReply Jafg::WEditorView::OnKeyUp(LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::RightMouseButton)
    {
        this->OnSecondaryUp();
    }

    return Super::OnKeyUp(InViewport, InKeyEvent);
}

Jafg::LReply Jafg::WEditorView::OnKeyUpNoFocus(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::RightMouseButton)
    {
        this->OnSecondaryUp();
    }

    return Super::OnKeyUpNoFocus(InViewport, InKeyEvent);
}

void Jafg::WEditorView::OnSecondaryDown()
{
    this->GetViewport().GetSurface().SetInputMode(EInputMode::Both);
    this->SetShouldTick(true);

    // LUserInput& Input{ this->GetLocalEgo().GetUserInput() };
    // Input.SetReferenceContexts(Input.GetActiveContexts());
    // Input.PopContexts();

    return;
}

void Jafg::WEditorView::OnSecondaryUp()
{
    this->GetViewport().GetSurface().SetInputMode(EInputMode::Both | EInputMode::ShowMouseCursor);
    this->SetShouldTick(false);
    //
    // LUserInput& Input{ this->GetLocalEgo().GetUserInput() };
    // Input.PushContexts();
    // Input.ActivateContexts(Input.GetReferenceContexts());
    // Input.SetReferenceContexts({});

    return;
}
