// Copyright mzoesch. All rights reserved.

#include "Widgets/UserWidget.h"
#include "Widgets/Viewport.h"
#include "Widgets/Parent.h"

void Jafg::WUserWidget::Destruct()
{
    if (this->IsTopLevel())
    {
        this->GetViewport().RemoveWidget(this);
        this->bIsTopLevel = false;
    }

    Super::Destruct();
    check(this->GetChildren().size() == 0)
    return;
}

void Jafg::WUserWidget::SetRoot(WNode* InRoot)
{
    check(this->GetRoot() == nullptr)
    check(this->GetChildren().size() == 0)
    Super::AddChild(InRoot);
    check(this->GetRoot() == InRoot)
    check(this->GetChildren().size() == 1)

    return;
}

void Jafg::WUserWidget::RemoveFromParent(const bool bDestroy /* = true */)
{
    if (this->IsTopLevel())
    {
        this->GetViewport().RemoveWidget(this);
        this->bIsTopLevel = false;
    }

    Super::RemoveFromParent(bDestroy);

    return;
}
