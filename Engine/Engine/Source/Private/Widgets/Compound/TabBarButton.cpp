// Copyright mzoesch. All rights reserved.

#include "Widgets/Compound/TabBarButton.h"
#include "Widgets/TextBlock.h"
#include "Widgets/Compound/TabBar.h"

bool Jafg::WTabBarButton::AddData(LWidgetNodeData* InData)
{
    Super::AddData(InData);

    if (InData->DerivedClass != WTabBarButton::StaticClass()->GetName())
    {
        return false;
    }

    LTabBarTabData* Data = static_cast<LTabBarTabData*>(InData);
    if (Data->Descriptor->DisplayName.IsEmpty() == false)
    {
        this->GetFactory<WTabBarButton>()
        [
            NewNode(WTextBlock).SaveTo(&this->ButtonText)
                .Brush(LTextBlockBrush::SubHeader())
                .Content(Data->Descriptor->DisplayName)
                .Anchor(EAnchor::CenterCenter)
                .Padding(Data->Descriptor->Padding)
        ];
    }

    this->Context = Data->Context;
    this->Identifier = Data->Descriptor->Identifier;

    if (Data->Descriptor->OnButtonRelease)
    {
        // ??? Why can't we move directly into the member variable?
        // ??? this->OnButtonRelease.operator=(std::move(Data->Descriptor->OnButtonRelease));
        LOnTabBarButtonRelease Del = std::move(Data->Descriptor->OnButtonRelease);
        this->OnButtonRelease = std::move(Del);
        checkSlow( Data->Descriptor->OnButtonRelease.IsBound() == false )
    }

    return true;
}

void Jafg::WTabBarButton::OnPrimaryRelease()
{
    Super::OnPrimaryRelease();

    if (this->OnButtonRelease && this->OnButtonRelease(*this->Context, this->Identifier))
    {
        return;
    }

    this->Context->OnTabBarButtonReleased(this->Identifier);

    return;
}

void Jafg::WTabBarButton::OnTabBarFocus(const bool bInFocus)
{
    if (bInFocus)
    {
        this->SetLetUiReactToEvents(false);
        this->SetBrush(this->HoverBrush);
    }
    else
    {
        this->SetLetUiReactToEvents(true);
        this->SetBrush(this->NormalBrush);
    }

    return;
}
