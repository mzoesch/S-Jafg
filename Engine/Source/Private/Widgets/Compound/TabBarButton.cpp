// Copyright mzoesch. All rights reserved.

#include "Widgets/Compound/TabBarButton.h"
#include "Widgets/TextBox.h"
#include "Widgets/Compound/TabBar.h"

bool Jafg::WTabBarButton::AddData(JNodeData& Data)
{
    const bool bSuper{ Super::AddData(Data) };

    JTabBarData* TbData{ Data.As<JTabBarData>() };
    if (TbData == nullptr)
    {
        return bSuper;
    }

    if (TbData->Descriptor->DisplayNameField.empty() == false)
    {
        BeginStyling(*this).Root<WTextBox>().SaveTo(&this->ButtonText)
            .TextBrush({ETextScale::SubHeader})
            .Content(TbData->Descriptor->DisplayNameField)
            .Anchor(EAnchor::CenterCenter)
            .Padding(TbData->Descriptor->PaddingField)
            ;
    }

    this->TabBar = TbData->TabBar;
    this->Identifier = TbData->Descriptor->IdentifierField;

    if (TbData->Descriptor->OnButtonReleaseField.IsValid())
    {
        this->OnButtonRelease = TbData->Descriptor->OnButtonReleaseField;
    }

    return true;
}

void Jafg::WTabBarButton::OnPrimaryRelease()
{
    Super::OnPrimaryRelease();

    if (this->OnButtonRelease.IsValid() && this->OnButtonRelease(*this->TabBar, this->Identifier))
    {
        return;
    }

    this->TabBar->OnTabBarButtonReleased(this->Identifier);

    return;
}

void Jafg::WTabBarButton::OnTabBarFocus(const bool bInFocus)
{
    if (bInFocus)
    {
        // this->SetLetUiReactToEvents(false);
        this->SetBrush(this->Style.HoverBrush);
    }
    else
    {
        // this->SetLetUiReactToEvents(true);
        this->SetBrush(this->Style.NormalBrush);
    }

    return;
}
