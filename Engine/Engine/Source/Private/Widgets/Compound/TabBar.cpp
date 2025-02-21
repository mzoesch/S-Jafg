// Copyright mzoesch. All rights reserved.

#include "Widgets/Compound/TabBar.h"
#include "Widgets/WidgetSwitcher.h"
#include "Widgets/WidgetRegion.h"
#include "Containers/MyStringUtility.h"
#include "Widgets/HBox.h"
#include "Widgets/VBox.h"

Jafg::WTabBar::WTabBar(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SetAnchor(EAnchor::TopLeft);
    return;
}

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
    WWidgetParent::UpdateDesiredSize(); // NOT SUPER!!!

    LVector2 DesiredSize = this->ButtonsContainer->GetDesiredSize();
    DesiredSize.X = Maths::Max(DesiredSize.X, this->Switcher->GetDesiredSize().X);
    DesiredSize.Y = Maths::Max(DesiredSize.Y, this->Switcher->GetDesiredSize().Y);
    DesiredSize += this->GetPadding().GetDesiredSize();

    this->SetDesiredSize(DesiredSize);

    return;
}

void Jafg::WTabBar::RegisterTab(LTabBarTabDescriptor&& InTabDescriptor) // Ok, rvalue is just to do some inline stuff... no need to move.
{
    const LSimpleString* const IdentPtr = &InTabDescriptor.Identifier;
    if (this->TabsInOrder.ContainsByPredicate([IdentPtr](const LAddedTabBarTab& Tab)
    {
        return Tab.Identifier == *IdentPtr;
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
        const LSimpleString* const AddAfterPtr = &InTabDescriptor.AddAfter;
        Index = this->TabsInOrder.FindIndexByPredicate([AddAfterPtr](const LAddedTabBarTab& Tab)
        {
            return Tab.Identifier == *AddAfterPtr;
        });
        check( Index != INDEX_NONE )
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
        // Do not move!!! We discard the tab descriptor just below.
        this->LoadTab(InTabDescriptor, Index);
    }

    InTabDescriptor.Identifier.Empty();
    InTabDescriptor.DisplayName.Empty();
    InTabDescriptor.PanelWidgetClass.Set(nullptr);
    InTabDescriptor.ButtonWidgetClass.Set(nullptr);
    InTabDescriptor.OnButtonPressed.Reset();
    InTabDescriptor.AddAfter.Empty();

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
    const void* PreviouslyFocusedTab = this->CurrentlyFocusedTab;
    if (const LAddedTabBarTab* FocusedTab = this->GetCurrentlyFocusedTab(); FocusedTab)
    {
        if (WTabBarButton* B = DynamicCast<WTabBarButton>(FocusedTab->Button); B)
        {
            B->OnTabBarFocus(false);
        }
        this->CurrentlyFocusedTab = nullptr;
    }

    int32 Idx;
    for (Idx = 0; Idx < this->TabsInOrder.GetSize(); ++Idx)
    {
        if (this->TabsInOrder[Idx].Identifier == Identifier)
        {
            break;
        }
        continue;
    }
    jassert( Idx < this->TabsInOrder.GetSize() )

    LAddedTabBarTab& TabDescriptor = this->TabsInOrder[Idx];

    if (PreviouslyFocusedTab == static_cast<const void*>(TabDescriptor.Identifier.ToPtr()))
    {
        this->Switcher->ResetWidgetIndex();
        return;
    }

    this->CurrentlyFocusedTab = static_cast<const void*>(TabDescriptor.Identifier.ToPtr());

    if (TabDescriptor.SwitcherIndex != INDEX_NONE)
    {
        this->Switcher->SetActiveWidgetIndex(TabDescriptor.SwitcherIndex);
        if (WTabBarButton* B = DynamicCast<WTabBarButton>(TabDescriptor.Button); B)
        {
            B->OnTabBarFocus(true);
        }
    }
    else
    {
        this->Switcher->ResetWidgetIndex();
    }

    return;
}

void Jafg::WTabBar::OnOuterVisibilityChanged(const EWidgetVisibility::Type InOldVisibility, const EWidgetVisibility::Type InNewVisibility)
{
    if (EWidgetVisibility::IsDrawn(InNewVisibility) == false)
    {
        return;
    }

    if (const LAddedTabBarTab* Tab = this->GetCurrentlyFocusedTab(); Tab)
    {
        if (WTabBarButton* B = DynamicCast<WTabBarButton>(Tab->Button); B)
        {
            B->OnTabBarFocus(false);
        }
    }
    this->Switcher->ResetWidgetIndex();

    return;
}

void Jafg::WTabBar::LoadTab(const LTabBarTabDescriptor& Descriptor, const int32 InIndex)
{
    check( !(Descriptor.ButtonWidgetClass && Descriptor.OnButtonPressed) )

    WWidgetNode* Button = Descriptor.ButtonWidgetClass.IsSet()
        ? ConstructDeferredWidgetNode<WWidgetNode>(Descriptor.ButtonWidgetClass)
        : ConstructDeferredWidgetNode<WWidgetNode>(this->DefaultButtonClass);
    this->ButtonsContainer->AddChildAt(InIndex, Button);

    this->TabsInOrder[InIndex].Button = Button;

    LTabBarTabData Data;
    Data.DerivedClass = WTabBarButton::StaticClass()->GetName();
    Data.Context = this;
    Data.Descriptor = &Descriptor;
    Button->AddData(&Data);

    MakeDeferredWidgetNodeFinal(Button);

    WTabBarPanel* Panel = nullptr;
    if (Descriptor.PanelWidgetClass)
    {
        Data.DerivedClass = WTabBarPanel::StaticClass()->GetName();
        Panel = ConstructDeferredWidgetNode(Descriptor.PanelWidgetClass);
        checkSlow( this->TabsInOrder[InIndex].Panel == nullptr )
        this->TabsInOrder[InIndex].Panel = Panel;
        this->Switcher->AddChild(Panel);
        this->TabsInOrder[InIndex].SwitcherIndex = static_cast<int8>(this->Switcher->GetChildren().GetSize() - 1);
        Panel->AddData(&Data);
        MakeDeferredWidgetNodeFinal(Panel);
    }

    Descriptor.Callback.InvokeIfBound(this, Button, Panel);

    return;
}

const Jafg::WTabBar::LAddedTabBarTab* Jafg::WTabBar::GetCurrentlyFocusedTab() const
{
    if (this->CurrentlyFocusedTab)
    {
        if (const int32 Idx= this->TabsInOrder.FindIndexByPredicate([this](const LAddedTabBarTab& Tab)
        {
            return Tab.Identifier.ToPtr() == this->CurrentlyFocusedTab;
        }); Idx != INDEX_NONE)
        {
            return &this->TabsInOrder[Idx];
        }
    }

    return nullptr;
}
