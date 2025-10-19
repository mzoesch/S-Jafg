// Copyright mzoesch. All rights reserved.

#include "Widgets/EditorView.h"
#include "Platform/SurfaceGlfw3.h"
#include "User/LocalEgo.h"
#include "Widgets/Viewport.h"

Jafg::WEditorView::WEditorView(LCxxObjectInitializer const& CxxObjectInitializer)
    : Super(CxxObjectInitializer)
{
    this->SetAnchor(EAnchor::Fill);
    this->SetVisibility(EWidgetVisibility::Visible);

    return;
}

void Jafg::WEditorView::Tick()
{
    Super::Tick();

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

    if (Surface->IsKeyUp(EKeys::RightMouseButton))
    {
        this->OnSecondaryUp();
    }

    return;
}

Jafg::LReply Jafg::WEditorView::OnKeyDown(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
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

Jafg::LReply Jafg::WEditorView::OnKeyUp(const LViewport& InViewport, const LKeyEvent& InKeyEvent)
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
    LViewport* Viewport{ this->GetViewport() };
    if (ensure(Viewport != nullptr) == false)
    {
        return;
    }
    LSurface* Surface{ Viewport->GetCachedContext() };
    if (ensure(Surface != nullptr) == false)
    {
        return;
    }

    Surface->SetInputMode(EInputMode::Both, false);
    this->SetShouldTick(true);

    LUserInput& Input{ this->GetLocalEgo().GetUserInput() };
    Input.SetReferenceContexts(Input.GetActiveContexts());
    Input.PopContexts();

    return;
}

void Jafg::WEditorView::OnSecondaryUp()
{
    LViewport* Viewport{ this->GetViewport() };
    if (ensure(Viewport != nullptr) == false)
    {
        return;
    }
    LSurface* Surface{ Viewport->GetCachedContext() };
    if (ensure(Surface != nullptr) == false)
    {
        return;
    }

    Surface->SetInputMode(EInputMode::Both, true);
    this->SetShouldTick(false);

    LUserInput& Input{ this->GetLocalEgo().GetUserInput() };
    Input.PushContexts();
    Input.ActivateContexts(Input.GetReferenceContexts());
    Input.SetReferenceContexts({});

    return;
}
