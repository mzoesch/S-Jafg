// Copyright mzoesch. All rights reserved.

#include "Widgets/Compound/TabBar.h"
#include "Widgets/WidgetSwitcher.h"
#include "Widgets/WidgetRegion.h"
#include "Containers/MyStringUtility.h"
#include "Widgets/HBox.h"
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

    WWidgetParentBase* Container = this;
    if (this->WrapperClass)
    {
        Container = ConstructDeferredWidgetNode<WWidgetParentBase>(this->WrapperClass);
        this->AddChild(Container);
        MakeDeferredWidgetNodeFinal(Container);
    }
    else if (this->bIsVertical.IsSet())
    {
        if (this->bIsVertical.GetValue() == true)
        {
            Container = ConstructDeferredWidgetNode<WVBox>();
            Container->SetAnchor(EAnchor::Fill);
            this->AddChild(Container);
            MakeDeferredWidgetNodeFinal(Container);
        }
        else
        {
            Container = ConstructDeferredWidgetNode<WHBox>();
            this->AddChild(Container);
            Container->SetAnchor(EAnchor::Fill);
            MakeDeferredWidgetNodeFinal(Container);
        }
    }

    this->ButtonsContainer = ConstructDeferredWidgetNode<WWidgetParentBase>(this->ButtonsContainerClass);
    this->Switcher = ConstructDeferredWidgetNode<WWidgetSwitcher>(this->SwitcherClass);
    Container->AddChild(this->ButtonsContainer);
    Container->AddChild(this->Switcher);
    MakeDeferredWidgetNodeFinal(this->ButtonsContainer);
    MakeDeferredWidgetNodeFinal(this->Switcher);

    for (LTabBarTabDescriptor& DeferredTab : this->DeferredTabs)
    {
        this->RegisterTab(std::move(DeferredTab));
    }
    this->DeferredTabs.Empty();

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
    if (this->TabsInOrder.ContainsByPredicate([InTabDescriptor](const LAddedTabBarTab& Tab)
    {
        return Tab.Identifier == InTabDescriptor.Identifier;
    }))
    {
        panicMsgf("Tab with identifier [{}] already exists.", InTabDescriptor.Identifier)
    }

    int32 Index;
    if (InTabDescriptor.AddAfter.IsEmpty())
    {
        this->TabsInOrder.Add(LAddedTabBarTab({.Identifier = InTabDescriptor.Identifier}));
        Index = this->TabsInOrder.GetSize() - 1;
    }
    else
    {
        Index = this->TabsInOrder.FindIndexByPredicate([InTabDescriptor](const LAddedTabBarTab& Tab)
        {
            return Tab.Identifier == InTabDescriptor.AddAfter;
        });
        jassert( Index != INDEX_NONE )
        ++Index;
        LAddedTabBarTab AddedTab;
        AddedTab.Identifier = InTabDescriptor.Identifier;
        this->TabsInOrder.AddAt(Index, std::move(AddedTab));
    }

    if (InTabDescriptor.DisplayName.IsEmpty())
    {
        const LSimpleString S = Strings::AddSpacesToCamelCase(InTabDescriptor.Identifier);
        InTabDescriptor.DisplayName = S.ToPtr();
    }

    if (this->ButtonsContainer)
    {
        this->LoadTab(InTabDescriptor, Index);
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

void Jafg::WTabBar::OnTabBarButtonPressed(const LSimpleString& Identifier)
{
    int32 idx;
    for (idx = 0; idx < this->TabsInOrder.GetSize(); ++idx)
    {
        if (this->TabsInOrder[idx].Identifier == Identifier)
        {
            break;
        }
        continue;
    }
    jassert( idx < this->TabsInOrder.GetSize() )

    LAddedTabBarTab& TabDescriptor = this->TabsInOrder[idx];
    LOG_WARNING(LogTemporal, "Tab [{}] was pressed.", TabDescriptor.Identifier)

    if (TabDescriptor.SwitcherIndex != INDEX_NONE)
    {
        this->Switcher->SetActiveWidgetIndex(TabDescriptor.SwitcherIndex);
    }
    else
    {
        this->Switcher->ResetWidgetIndex();
    }

    return;
}

void Jafg::WTabBar::LoadTab(const LTabBarTabDescriptor& Descriptor, const int32 InIndex)
{
    WWidgetNode* Button = Descriptor.ButtonWidgetClass.IsSet()
        ? ConstructDeferredWidgetNode<WWidgetNode>(Descriptor.ButtonWidgetClass)
        : ConstructDeferredWidgetNode<WWidgetNode>(this->DefaultButtonClass);
    this->ButtonsContainer->AddChildAt(InIndex, Button);

    LTabBarTabData Data;
    Data.DerivedClass = WTabBarButton::StaticClass()->GetName();
    Data.Context = this;
    Data.Descriptor = &Descriptor;
    Button->AddData(&Data);

    MakeDeferredWidgetNodeFinal(Button);

    if (Descriptor.PanelWidgetClass)
    {
        WTabBarBase* Panel = ConstructDeferredWidgetNode(Descriptor.PanelWidgetClass);
        checkSlow( this->TabsInOrder[InIndex].Panel == nullptr )
        this->TabsInOrder[InIndex].Panel = Panel;
        this->Switcher->AddChild(Panel);
        this->TabsInOrder[InIndex].SwitcherIndex = static_cast<int8>(this->Switcher->GetChildren().GetSize() - 1);
        MakeDeferredWidgetNodeFinal(Panel);
    }

    return;
}
