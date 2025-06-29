// Copyright mzoesch. All rights reserved.

#include "Widgets/Compound/TabBarButton.h"
#include "Widgets/TextBlock.h"
#include "Widgets/Compound/TabBar.h"

bool Jafg::WTabBarButton::AddData(const LWidgetNodeData* InData)
{
    Super::AddData(InData);

    if (InData->DerivedClass != WTabBarButton::StaticClass()->GetName())
    {
        return false;
    }

    const LTabBarTabData* Data = static_cast<const LTabBarTabData*>(InData);
    if (Data->Descriptor->DisplayNameField.IsEmpty() == false)
    {
        this->GetFactory<WTabBarButton>()
        [
            NewNode(WTextBlock).SaveTo(&this->ButtonText)
                .Brush(LTextBlockBrush::SubHeader())
                .Content(Data->Descriptor->DisplayNameField)
                .Anchor(EAnchor::CenterCenter)
                .Padding(Data->Descriptor->PaddingField)
        ];
    }

    this->Context = Data->Context;
    this->Identifier = Data->Descriptor->IdentifierField;

    if (Data->Descriptor->OnButtonReleaseField.IsValid())
    {
        this->OnButtonRelease = Data->Descriptor->OnButtonReleaseField;
    }

    return true;
}

void Jafg::WTabBarButton::OnPrimaryRelease()
{
    Super::OnPrimaryRelease();

    if (this->OnButtonRelease.IsValid() && this->OnButtonRelease(*this->Context, this->Identifier))
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
        this->SetBrush(this->Style.HoverBrush);
    }
    else
    {
        this->SetLetUiReactToEvents(true);
        this->SetBrush(this->Style.NormalBrush);
    }

    return;
}
