// Copyright mzoesch. All rights reserved.

#include "Widgets/UserWidget.h"
#include "Widgets/Viewport.h"
#include "Widgets/Parent.h"

Jafg::WUserWidget::WUserWidget(LCxxObjectInitializer const& CxxObjectInitializer) : Super(CxxObjectInitializer)
{
    this->SetAnchor(EAnchor::Fill);
    this->SetShouldTick(false);

    return;
}

void Jafg::WUserWidget::OnGarbage(ECxxRecordTearDownReason::Type Reason)
{
    Super::OnGarbage(Reason);
    this->Root = nullptr;

    if (this->AttachedViewport)
    {
        this->AttachedViewport->RemoveWidget(this);
        this->AttachedViewport = nullptr;
    }

    return;
}

void Jafg::WUserWidget::Destruct()
{
    Super::Destruct();

    // check( this->GetParent() == nullptr )
    check( this->GetChildren().size() == 0 )
    // check( this->AttachedViewport == nullptr )
    this->Root = nullptr;

    return;
}

Jafg::LViewport* Jafg::WUserWidget::GetMostOuterViewport()
{
    if (this->Slot.Parent)
    {
        check( this->AttachedViewport == nullptr )
        return Super::GetMostOuterViewport();
    }

    return this->AttachedViewport;
}

void Jafg::WUserWidget::AddToViewport(LViewport* InViewport)
{
    check( this->AttachedViewport == nullptr )
    this->AttachedViewport = InViewport;
    this->RecacheViewport();
    this->AttachedViewport->AddWidget(this);

    return;
}

void Jafg::WUserWidget::AddToViewportAt(const i32 InIndex, LViewport* InViewport)
{
    check( this->AttachedViewport == nullptr )

    this->AttachedViewport = InViewport;
    this->AttachedViewport->AddWidgetAt(InIndex, this);

    return;
}

void Jafg::WUserWidget::RemoveFromParent(const bool bDestroy /* = true */)
{
    Super::RemoveFromParent(bDestroy);

    if (this->AttachedViewport)
    {
        this->AttachedViewport->RemoveWidget(this);
        this->AttachedViewport = nullptr;
    }

    return;
}

void Jafg::WUserWidget::RemoveChild(WNode* InChild)
{
    check( this->Root )
    check( this->GetChildren().size() == 1 )

    Super::RemoveChild(InChild);

    check( this->GetChildren().empty() )
    this->Root = nullptr;

    return;
}

Jafg::LWidgetSlot* Jafg::WUserWidget::AddChild(WNode* InChild)
{
    check( this->Root == nullptr && this->GetChildren().size() == 0 )
    LWidgetSlot* Out = Super::AddChild(InChild);
    check( Out )
    this->Root = Out;
    return Out;
}

Jafg::LWidgetSlot* Jafg::WUserWidget::AddChildAt(const i32 InIndex, WNode* InChild)
{
    check( this->Root == nullptr && this->GetChildren().size() == 0 )
    LWidgetSlot* Out = Super::AddChildAt(InIndex, InChild);
    check( Out )
    this->Root = Out;
    return Out;
}

Jafg::WParentBase* Jafg::WUserWidget::ReplaceRootImpl(WParentBase* InRoot)
{
    if (this->IsRootValid())
    {
        this->RemoveChild(this->Root);
        check( this->Root == nullptr )
    }

    const LWidgetSlot* Out = this->AddChild(InRoot);
    check( this->Root != nullptr )
    check( this->GetChildren().size() == 1 )

    this->RecacheViewport();
    check( this->CachedViewport )

    /* Typesafe this is. Look at function parameters. */
    return static_cast<WParentBase*>(Out->Content);
}
