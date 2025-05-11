// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/PreferencesScreen.h"
#include "User/Preferences/PreferenceRegistry.h"
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

bool Jafg::WPreferencesPanel::AddData(LWidgetNodeData* InData)
{
    Super::AddData(InData);
    if (InData->DerivedClass != WPreferencesPanel::StaticClass()->GetName())
    {
        return false;
    }

    LPreferencesPanelData* Data = static_cast<LPreferencesPanelData*>(InData);
    LPreference* P = Data->Preference;
    check( P )

    WVRegion* Root;
    NewNode(WVRegion).SaveTo(&Root)
        .Anchor(EAnchor::Fill)
        .Padding(40.0f)
        .VSpace(5.0f);

    ::AddPreference(Root, P);

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
        LPreference* LambdaPreference = const_cast<LPreference*>(TopPreference.GetValuePtr());

        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = TopPreference->GetName().ToString();
        Descriptor.DisplayName = TopPreference->GetDisplayName();
        Descriptor.Padding = LPadding(7.0f, 0.0f, 0.0f, 0.0f);
        Descriptor.PanelWidgetClass = this->PanelClass;
        Descriptor.Callback = [LambdaPreference](WTabBar* TabBar, WNode* Button, WNode* Panel) -> void
        {
            if (Panel)
            {
                LPreferencesPanelData Data;
                Data.DerivedClass = WPreferencesPanel::StaticClass()->GetName();
                Data.Preference = LambdaPreference;
                Panel->AddData(&Data);
            }

            return;
        };
        this->RegisterTab(std::move(Descriptor));
    }

    return;
}
