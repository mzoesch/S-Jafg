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

    const LTabBarTabData* Data = static_cast<LTabBarTabData*>(InData);
    if (Data->Descriptor->DisplayName.IsEmpty() == false)
    {
        this->GetFactory<WTabBarButton>()
        [
            NewNode(WTextBlock).SaveTo(this->ButtonText)
                .Brush(LTextBlockBrush::Small())
                .Content(Data->Descriptor->DisplayName)
                .Anchor(EAnchor::CenterCenter)
        ];
    }

    this->Context = Data->Context;
    this->Identifier = Data->Descriptor->Identifier;

    return true;
}

void Jafg::WTabBarButton::OnPrimaryPress()
{
    Super::OnPrimaryPress();
    this->Context->OnTabBarButtonPressed(this->Identifier);
    return;
}
