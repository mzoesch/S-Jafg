// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Widgets/Compound/TabBar.h"
#include "Widgets/WidgetSwitcher.h"
#include "Widgets/WidgetRegion.h"
#include "Containers/MyStringUtility.h"
#include "Widgets/VBox.h"

void Jafg::WTabBar::Construct()
{
    Super::Construct();

    if (this->ButtonsContainerClass == nullptr)
    {
        this->ButtonsContainerClass.Set<WVBox>();
    }
    if (this->SwitcherClass == nullptr)
    {
        this->SwitcherClass.Set<WWidgetSwitcher>();
    }
    if (this->DefaultButtonClass == nullptr)
    {
        this->DefaultButtonClass.Set<WWidgetRegion>();
    }

    checkSlow( this->ButtonsContainerClass )
    checkSlow( this->ButtonsContainerClass.IsValidType() )
    checkSlow( this->SwitcherClass )
    checkSlow( this->SwitcherClass.IsValidType() )
    checkSlow( this->DefaultButtonClass )
    checkSlow( this->DefaultButtonClass.IsValidType() )

    this->ButtonsContainer = ConstructDeferredWidgetNode<WWidgetParentBase>(this->ButtonsContainerClass);
    this->Switcher = ConstructDeferredWidgetNode<WWidgetSwitcher>(this->SwitcherClass);
    this->AddChild(this->ButtonsContainer);
    this->AddChild(this->Switcher);
    MakeDeferredWidgetNodeFinal(this->ButtonsContainer);
    MakeDeferredWidgetNodeFinal(this->Switcher);

    for (int32 Index = 0; Index < this->TabsInOrder.GetSize(); ++Index)
    {
        this->LoadTab(Index);
    }

    return;
}

void Jafg::WTabBar::UpdateDesiredSize() const
{
    Super::UpdateDesiredSize();

    LVector2 DesiredSize = this->ButtonsContainer->GetDesiredSize();
    DesiredSize.X = Maths::Max(DesiredSize.X, this->Switcher->GetDesiredSize().X);
    DesiredSize.Y = Maths::Max(DesiredSize.Y, this->Switcher->GetDesiredSize().Y);
    DesiredSize += this->GetPadding().GetDesiredSize();

    this->SetDesiredSize(DesiredSize);

    return;
}

void Jafg::WTabBar::RegisterTab(LTabBarTabDescriptor&& InTabDescriptor)
{
    if (this->TabsInOrder.ContainsByPredicate([InTabDescriptor](const LTabBarTabDescriptor& Tab)
    {
        return Tab.Identifier == InTabDescriptor.Identifier;
    }))
    {
        panicMsgf("Tab with identifier [{}] already exists.", InTabDescriptor.Identifier)
    }

    int32 Index;
    if (InTabDescriptor.AddAfter.IsEmpty())
    {
        this->TabsInOrder.Emplace(std::move(InTabDescriptor));
        Index = this->TabsInOrder.GetSize() - 1;
    }
    else
    {
        Index = this->TabsInOrder.FindIndexByPredicate([InTabDescriptor](const LTabBarTabDescriptor& Tab)
        {
            return Tab.Identifier == InTabDescriptor.AddAfter;
        });
        jassert( Index != INDEX_NONE )
        this->TabsInOrder.AddAt(Index + 1, std::move(InTabDescriptor));
    }

    LTabBarTabDescriptor* TabDescriptor = &this->TabsInOrder[Index];
    if (TabDescriptor->DisplayName.IsEmpty())
    {
        const LSimpleString S = Strings::AddSpacesToCamelCase(TabDescriptor->Identifier);
        TabDescriptor->DisplayName = S.ToPtr();
    }

    if (this->ButtonsContainer)
    {
        this->LoadTab(Index);
    }

    return;
}

bool Jafg::WTabBar::UnregisterTab(const LSimpleString& Identifier)
{
    return false;
}

bool Jafg::WTabBar::UnregisterTabChecked(const LSimpleString& Identifier)
{
    const bool bOut = this->UnregisterTab(Identifier);
    check( bOut )
    return bOut;
}

void Jafg::WTabBar::LoadTab(const int32 InIndex)
{
    checkSlow( this->TabsInOrder.IsValidIndex(InIndex) )
    checkSlow( this->TabsInOrder[InIndex].DisplayName.IsEmpty() == false )

    LTabBarTabDescriptor* TabDescriptor = &this->TabsInOrder[InIndex];

    WWidgetNode* Button = TabDescriptor->ButtonWidgetClass.IsSet()
        ? ConstructDeferredWidgetNode<WWidgetNode>(TabDescriptor->ButtonWidgetClass)
        : ConstructDeferredWidgetNode<WWidgetNode>(this->DefaultButtonClass);
    this->ButtonsContainer->AddChildAt(InIndex, Button);

    LTabBarTabData Data;
    Data.DerivedClass = WTabBarButton::StaticClass()->GetName();
    Data.Descriptor = TabDescriptor;
    Button->AddData(&Data);

    MakeDeferredWidgetNodeFinal(Button);

    return;
}
