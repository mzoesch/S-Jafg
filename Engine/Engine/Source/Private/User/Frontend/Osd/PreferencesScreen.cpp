// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/PreferencesScreen.h"
#include "User/Preferences/PreferenceRegistry.h"
#include "Widgets/Spacer.h"
#include "Widgets/VBox.h"

namespace
{

void AddPreference(Jafg::WWidgetParentBase* Target, const Jafg::LPreference* P)
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

    WTextBlock* Text = nullptr;
    NewNode(WTextBlock).SaveTo(Text)
        .Brush(LTextBlockBrush::SubHeader())
        .Content(P->GetDisplayName());
        // .Padding({10.0f, 5.0f});
    Target->AddChild(Text);

    WSpacer* Spacer; NewNode(WSpacer).SaveTo(Spacer).SetHeight(10.0f);
    Target->AddChild(Spacer);

    for (const Smart::TUnique<LPreference>& SubSection : P->GetChildPreferences())
    {
        ::AddPreference(Target, SubSection);
        WSpacer* SubSectionSpacer; NewNode(WSpacer).SaveTo(SubSectionSpacer).SetHeight(5.0f);
        Target->AddChild(SubSectionSpacer);
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

    const LPreferencesPanelData* Data = static_cast<LPreferencesPanelData*>(InData);
    const LPreference* P = Data->Preference;
    check( P )

    WVBox* Root; NewNode(WVBox).SaveTo(Root).Anchor(EAnchor::Fill).Padding(40.0f);
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
    for (const Smart::TUnique<LPreference>& TopPreference : Registry->GetPreferences() )
    {
        const LPreference* LambdaPreference = TopPreference.GetValuePtr();

        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = TopPreference->GetName().ToString();
        Descriptor.DisplayName = TopPreference->GetDisplayName();
        Descriptor.Padding = LPadding(7.0f, 0.0f, 0.0f, 0.0f);
        Descriptor.PanelWidgetClass = this->PanelClass;
        Descriptor.Callback = [LambdaPreference](WTabBar* TabBar, WWidgetNode* Button, WWidgetNode* Panel) -> void
        {
            if (Panel)
            {
                LPreferencesPanelData Data;
                Data.DerivedClass = WPreferencesPanel::StaticClass()->GetName();
                Data.Preference = LambdaPreference;
                Panel->AddData(&Data);
            }
        };
        this->RegisterTab(std::move(Descriptor));
    }

    return;
}
