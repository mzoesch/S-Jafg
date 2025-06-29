// Copyright mzoesch. All rights reserved.

#include "Widgets/Compound/TabBar.h"
#include "Widgets/Switcher.h"
#include "Widgets/Region.h"
#include "Containers/MyStringUtility.h"
#include "Widgets/HRegion.h"
#include "Widgets/Spacer.h"
#include "Widgets/VRegion.h"

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
        this->ButtonsContainerClass.Set<WVRegion>();
    }
    if (this->SwitcherClass == nullptr)
    {
        this->SwitcherClass.Set<WSwitcher>();
    }
    if (this->DefaultButtonClass == nullptr)
    {
        this->DefaultButtonClass.Set<WRegion>();
    }

    checkSlow( this->ButtonsContainerClass )
    checkSlow( this->ButtonsContainerClass.IsValidType() )
    checkSlow( this->SwitcherClass )
    checkSlow( this->SwitcherClass.IsValidType() )
    checkSlow( this->DefaultButtonClass )
    checkSlow( this->DefaultButtonClass.IsValidType() )

    WParentBase* Container = this;
    if (this->WrapperClass)
    {
        Container = ConstructDeferredWidgetNode<WParentBase>(this->GetOuter(), this->WrapperClass);
        this->AddChild(Container);
        MakeDeferredWidgetNodeFinal(Container);
    }
    else if (this->bIsVertical.IsValid())
    {
        if (this->bIsVertical.GetValue() == true)
        {
            Container = ConstructDeferredWidgetNode<WVRegion>(this->GetOuter());
            Container->SetAnchor(EAnchor::Fill);
            this->AddChild(Container);
            MakeDeferredWidgetNodeFinal(Container);
        }
        else
        {
            Container = ConstructDeferredWidgetNode<WHRegion>(this->GetOuter());
            this->AddChild(Container);
            Container->SetAnchor(EAnchor::Fill);
            MakeDeferredWidgetNodeFinal(Container);
        }
    }

    this->ButtonsContainer = ConstructDeferredWidgetNode<WParentBase>(this->GetOuter(), this->ButtonsContainerClass);
    this->Switcher = ConstructDeferredWidgetNode<WSwitcher>(this->GetOuter(), this->SwitcherClass);
    Container->AddChild(this->ButtonsContainer);
    Container->AddChild(this->Switcher);
    MakeDeferredWidgetNodeFinal(this->ButtonsContainer);
    MakeDeferredWidgetNodeFinal(this->Switcher);

    for (LTabBarTabDescriptor& DeferredTab : this->DeferredTabs)
    {
        this->RegisterTab(std::move(DeferredTab));
    }
    this->DeferredTabs.Empty();

    this->ResetToDefault();

    return;
}

void Jafg::WTabBar::RegisterTab(LTabBarTabDescriptor&& InTabDescriptor) // Ok, rvalue is just to do some inline stuff... no need to move.
{
    const LString* const IdentPtr = &InTabDescriptor.IdentifierField;
    if (this->TabsInOrder.ContainsByPredicate([IdentPtr](const LAddedTabBarTab& Tab)
    {
        return Tab.Identifier == *IdentPtr;
    }))
    {
        panicMsgf("Tab with identifier [{}] already exists.", InTabDescriptor.IdentifierField)
    }

    if (this->ButtonsContainer == nullptr)
    {
        this->DeferredTabs.Emplace(std::move(InTabDescriptor));
        return;
    }

    i32 Index;
    if (InTabDescriptor.AddAfterField.IsEmpty())
    {
        this->TabsInOrder.Add(LAddedTabBarTab({.Identifier = InTabDescriptor.IdentifierField}));
        Index = this->TabsInOrder.GetSize() - 1;
    }
    else
    {
        const LString* const AddAfterPtr = &InTabDescriptor.AddAfterField;
        Index = this->TabsInOrder.FindByPredicate([AddAfterPtr](const LAddedTabBarTab& Tab)
        {
            return Tab.Identifier == *AddAfterPtr;
        });
        check( Index != INDEX_NONE )
        ++Index;
        LAddedTabBarTab AddedTab;
        AddedTab.Identifier = InTabDescriptor.IdentifierField;
        this->TabsInOrder.AddAt(Index, std::move(AddedTab));
    }

    if (InTabDescriptor.DisplayNameField.IsEmpty())
    {
        const LString S = Strings::AddSpacesToCamelCase(InTabDescriptor.IdentifierField);
        InTabDescriptor.DisplayNameField = S.ToPtr();
    }

    this->LoadTab(std::move(InTabDescriptor), Index);

    return;
}

void Jafg::WTabBar::AppendVSpace(const f32 InVSpace)
{
    WSpacer* Spacer = ConstructDeferredWidgetNode<WSpacer>(this->GetOuter());
    Spacer->SetHeight(InVSpace);
    this->ButtonsContainer->AddChild(Spacer);
    MakeDeferredWidgetNodeFinal(Spacer);

    return;
}

void Jafg::WTabBar::AppendHSpace(const f32 InHSpace)
{
    WSpacer* Spacer = ConstructDeferredWidgetNode<WSpacer>(this->GetOuter());
    Spacer->SetWidth(InHSpace);
    this->ButtonsContainer->AddChild(Spacer);
    MakeDeferredWidgetNodeFinal(Spacer);

    return;
}

void Jafg::WTabBar::AppendStretch(const LAnchor& InStretch)
{
    WSpacer* Spacer = ConstructDeferredWidgetNode<WSpacer>(this->GetOuter());
    Spacer->SetAnchor(InStretch);
    this->ButtonsContainer->AddChild(Spacer);
    MakeDeferredWidgetNodeFinal(Spacer);

    return;
}

bool Jafg::WTabBar::UnregisterTab(const LString& Identifier)
{
    return false;
}

void Jafg::WTabBar::ResetToDefault()
{
    if (this->TabsInOrder.IsValidIndex(this->DefaultIndex))
    {
        this->ActivateTab(this->TabsInOrder[this->DefaultIndex].Identifier);
    }
    else
    {
        if (this->DefaultIndex != INDEX_NONE)
        {
            LOG_ERROR(LogWidgets, "The default index [{}] is out of bounds.", this->DefaultIndex)
        }

        if (this->bAllowNone == false)
        {
            LOG_ERROR(LogWidgets, "The default index is set to none, but none is allowed.")
            return;
        }

        this->ActivateTab("");
    }

    return;
}

void Jafg::WTabBar::ActivateTab(const LString& Identifier)
{
    i32 Idx;
    for (Idx = 0; Idx < this->TabsInOrder.GetSize(); ++Idx)
    {
        if (this->TabsInOrder[Idx].Identifier == Identifier)
        {
            break;
        }
        continue;
    }
    jassert( Idx < this->TabsInOrder.GetSize() || Identifier.IsEmpty() )

    if (Identifier.IsEmpty() && this->bAllowNone == false)
    {
        LOG_ERROR(LogWidgets, "The tab bar is not allowed to be none. Discarding change request.")
        return;
    }

    const LAddedTabBarTab* TabDescriptor = this->TabsInOrder.IsValidIndex(Idx) ? &this->TabsInOrder[Idx] : nullptr;

    if (const LAddedTabBarTab* FocusedTab = this->GetCurrentlyFocusedTab(); FocusedTab)
    {
        if (TabDescriptor && this->CurrentlyFocusedTab == static_cast<const void*>(TabDescriptor->Button))
        {
            if (this->bAllowNone)
            {
                this->ActivateTab("");
                return;
            }

            return;
        }

        check( TabDescriptor || this->bAllowNone )

        if (WTabBarButton* B = DynamicCast<WTabBarButton>(FocusedTab->Button); B)
        {
            B->OnTabBarFocus(false);
        }
        this->CurrentlyFocusedTab = nullptr;
    }

    if (Identifier.IsEmpty())
    {
        check( this->bAllowNone )
        this->Switcher->ResetWidgetIndex();
        return;
    }

    check( TabDescriptor )
    check( TabDescriptor->Button )

    this->CurrentlyFocusedTab = static_cast<const void*>(TabDescriptor->Button);

    if (TabDescriptor->SwitcherIndex != INDEX_NONE)
    {
        this->Switcher->SetActiveWidgetIndex(TabDescriptor->SwitcherIndex);
        if (WTabBarButton* B = DynamicCast<WTabBarButton>(TabDescriptor->Button); B)
        {
            B->OnTabBarFocus(true);
        }
    }
    else
    {
        check( this->bAllowNone )
        this->Switcher->ResetWidgetIndex();
    }

    return;
}

void Jafg::WTabBar::OnTabBarButtonReleased(const LString& Identifier)
{
    this->ActivateTab(Identifier);
    return;
}

void Jafg::WTabBar::LoadTab(LTabBarTabDescriptor&& InTabDescriptor, const i32 InIndex)
{
    check( this->ButtonsContainer )
    check( !(InTabDescriptor.ButtonWidgetClassField && InTabDescriptor.OnButtonReleaseField.IsValid()) )

    WNode* Button = InTabDescriptor.ButtonWidgetClassField.IsSet()
        ? ConstructDeferredWidgetNode<WNode>(this->GetOuter(), InTabDescriptor.ButtonWidgetClassField)
        : ConstructDeferredWidgetNode<WNode>(this->GetOuter(), this->DefaultButtonClass);

    i32 Iterator { 0 };
    i32 Where { 0 };
    for (const LWidgetSlot* Slot : this->ButtonsContainer->GetChildren())
    {
        if (Iterator == InIndex)
        {
            break;
        }

        check( Slot && Slot->Content )
        if (Slot->Content->IsA<WSpacer>() == false)
        {
            ++Iterator;
        }

        ++Where;

        continue;
    }
    check( Iterator == InIndex )
    while (this->ButtonsContainer->GetChildren().IsValidIndex(Where))
    {
        const LWidgetSlot* Slot = this->ButtonsContainer->GetChildren()[Where];
        check( Slot && Slot->Content )
        if (Slot->Content->IsA<WSpacer>() == false)
        {
            break;
        }

        ++Where;

        continue;
    }
    this->ButtonsContainer->AddChildAt(Where, Button);

    this->TabsInOrder[InIndex].Button = Button;

    LTabBarTabData Data;
    Data.DerivedClass = WTabBarButton::StaticClass()->GetName();
    Data.Context = this;
    Data.Descriptor = &InTabDescriptor;
    Button->AddData(&Data);

    MakeDeferredWidgetNodeFinal(Button);

    WNode* Panel { nullptr };
    if (InTabDescriptor.PanelWidgetClassField)
    {
        Data.DerivedClass = WTabBarPanel::StaticClass()->GetName();
        Panel = ConstructDeferredWidgetNode(this->GetOuter(), InTabDescriptor.PanelWidgetClassField);
        checkSlow( this->TabsInOrder[InIndex].Panel == nullptr )
        this->TabsInOrder[InIndex].Panel = Panel;
        this->Switcher->AddChild(Panel);
        this->TabsInOrder[InIndex].SwitcherIndex = static_cast<i8>(this->Switcher->GetChildren().GetSize() - 1);
        Panel->AddData(&Data);
        MakeDeferredWidgetNodeFinal(Panel);
    }

    InTabDescriptor.CallbackField.InvokeIfBound(this, Button, Panel);

    for (LTabBarTabDescriptor& Siblings : InTabDescriptor.Siblings)
    {
        this->RegisterTab(std::move(Siblings));
    }
    InTabDescriptor.Siblings.Empty();

    return;
}

const Jafg::WTabBar::LAddedTabBarTab* Jafg::WTabBar::GetCurrentlyFocusedTab() const
{
    if (this->CurrentlyFocusedTab)
    {
        if (const i32 Idx= this->TabsInOrder.FindByPredicate([this](const LAddedTabBarTab& Tab)
        {
            return Tab.Button == this->CurrentlyFocusedTab;
        }); Idx != INDEX_NONE)
        {
            return &this->TabsInOrder[Idx];
        }
    }

    return nullptr;
}
