// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/PreferencesScreen.h"
#include "User/Preferences/PreferenceRegistry.h"
#include "Widgets/ScrollRegion.h"
#include "Widgets/Spacer.h"
#include "Widgets/VRegion.h"

namespace
{

void AddPreference(Jafg::WParentBase* Target, Jafg::LPreference* P)
{
    using namespace Jafg;

    if (P->IsLeaf())
    {
        if (P->Build(Target) == false)
        {
            LOG_WARNING(LogPreferences, "Failed to build preference [{}].", P->GetDisplayName())
        }

        return;
    }

    if (P->Build(Target) == false)
    {
        WTextBlock* Text = nullptr;
        NewNodeCtx(Target, WTextBlock).SaveTo(&Text)
            .Brush(LTextBlockBrush::SubHeader())
            .Content(P->GetDisplayName());
        Target->AddChild(Text);
    }

    Target->AddChild(&NewNodeNoFactoryCtx(Target, WSpacer));

    for (const Smart::TUnique<LPreference>& SubSection : P->LoadAndGetChildPreferences())
    {
        ::AddPreference(Target, const_cast<LPreference*>(&SubSection.GetValue()));
    }

    return;
}

} /* ~Namespace <Anonymous> */

void Jafg::WPreferencesPanel::Construct()
{
    Super::Construct();

    return;
}

bool Jafg::WPreferencesPanel::AddData(const LWidgetNodeData* InData)
{
    Super::AddData(InData);
    if (InData->DerivedClass != WPreferencesPanel::StaticClass()->GetName())
    {
        return false;
    }

    const LPreferencesPanelData* Data = static_cast<const LPreferencesPanelData*>(InData);
    LPreference* P = Data->Preference;
    check( P )

    WParentBase* Root;
    WParentBase* ChildContainer;
    NewNode(WScrollRegion).SaveTo(&Root)
        .Anchor(EAnchor::Fill)
    [
        NewNode(WVRegion).SaveTo(&ChildContainer)
            .Anchor(EAnchor::Fill)
            .Padding(40.0f)
            .VSpace(5.0f)
    ];

    ::AddPreference(ChildContainer, P);

    if (this->GetChildren().GetSize() > 1)
    {
        this->RemoveChild(*this->GetChildren().GetLast());
    }
    this->AddChild(Root);
    MakeDeferredWidgetNodeFinal(Root);

    return true;
}

Jafg::WPreferencesScreen::WPreferencesScreen(const LObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    this->SetHorizontalPreference();
    this->SetAnchor(EAnchor::Fill);
    return;
}

void Jafg::WPreferencesScreen::Construct()
{
    this->SetTabBarDepth(1);

    Super::Construct();

    jassert( this->PanelClass )

    const JPreferenceRegistry* Registry = GetDefault<JPreferenceRegistry>();
    for (const Smart::TUnique<LPreference>& TopPreference : Registry->GetPreferences())
    {
        LPreference* LambdaPreference = const_cast<LPreference*>(TopPreference.GetPointerChecked());

        LTabBarTabDescriptor Descriptor;
        Descriptor.IdentifierField = TopPreference->GetName().ToString();
        Descriptor.DisplayNameField = TopPreference->GetDisplayName();
        Descriptor.PaddingField = LPadding(7.0f, 0.0f, 0.0f, 0.0f);
        Descriptor.PanelWidgetClassField = this->PanelClass;
        Descriptor.CallbackField = [LambdaPreference](WTabBar* TabBar, WNode* Button, WNode* Panel) -> void
        {
            if (Panel)
            {
                LPreferencesPanelData Data;
                Data.DerivedClass = WPreferencesPanel::StaticClass()->GetName();
                Data.Preference = LambdaPreference;
                Panel->AddData(&Data);
            }
            else
            {
                LOG_WARNING(LogPreferences, "Panel is invalid.")
            }

            return;
        };
        this->RegisterTab(std::move(Descriptor));
    }

    return;
}
