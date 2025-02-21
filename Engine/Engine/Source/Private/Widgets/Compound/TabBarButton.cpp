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
            NewNode(WTextBlock).SaveTo(this->ButtonText)
                .Brush(LTextBlockBrush::SubHeader())
                .Content(Data->Descriptor->DisplayName)
                .Anchor(EAnchor::CenterCenter)
                .Padding(Data->Descriptor->Padding)
        ];
    }

    this->Context = Data->Context;
    this->Identifier = Data->Descriptor->Identifier;

    if (Data->Descriptor->OnButtonPressed)
    {
        // ??? Why can't we move directly into the member variable?
        // ??? this->OnButtonPressed.operator=(std::move(Data->Descriptor->OnButtonPressed));
        LOnTabBarButtonPressed Del = std::move(Data->Descriptor->OnButtonPressed);
        this->OnButtonPressed = std::move(Del);
        checkSlow( Data->Descriptor->OnButtonPressed.IsBound() == false )
    }

    return true;
}

void Jafg::WTabBarButton::OnPrimaryPress()
{
    Super::OnPrimaryPress();

    if (this->OnButtonPressed && this->OnButtonPressed(*this->Context, this->Identifier))
    {
        return;
    }

    this->Context->OnTabBarButtonPressed(this->Identifier);

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
