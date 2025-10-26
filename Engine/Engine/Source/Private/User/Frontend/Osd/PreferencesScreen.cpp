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
        WTextBox* Text = nullptr;
        NewNodeCtx(Target, WTextBox).SaveTo(&Text)
            .Brush(LTextBoxBrush::SubHeader())
            .Content(P->GetDisplayName());
        Target->AddChild(Text);
    }

    Target->AddChild(NewNodeNoFactoryCtx(Target, WSpacer));

    for (const TUnique<LPreference>& SubSection : P->LoadAndGetChildPreferences())
    {
        ::AddPreference(Target, SubSection.get());
    }

    return;
}

} /* ~Namespace <Anonymous> */

void Jafg::WPreferencesPanel::Construct()
{
    Super::Construct();

    return;
}

bool Jafg::WPreferencesPanel::AddData(JNodeData& Data)
{
    const bool bSuper{ Super::AddData(Data) };
    JPreferencesPanelData* PpData{ Data.As<JPreferencesPanelData>() };
    if (PpData == nullptr)
    {
        return false;
    }

    LPreference* P = PpData->Preference;
    check( P )

    WParentBase* Root;
    WParentBase* ChildContainer;
    NewNode(WScrollRegion).SaveTo(&Root)
        .Anchor(EAnchor::Fill)
    [
        NewNode(WVRegion).SaveTo(&ChildContainer)
            .Anchor(EAnchor::Fill)
            .Padding(40.0f)
            .VSpace(5_pt)
    ];

    ::AddPreference(ChildContainer, P);

    if (this->GetChildren().size() > 1)
    {
        this->RemoveChild(this->GetChildren().back());
    }
    this->AddChild(Root);
    MakeDeferredWidgetNodeFinal(Root);

    return true;
}

Jafg::WPreferencesScreen::WPreferencesScreen(LCxxObjectInitializer const& CxxObjectInitializer)
    : Super(CxxObjectInitializer)
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
    for (const TUnique<LPreference>& TopPreference : Registry->GetPreferences())
    {
        check( TopPreference.get() )
        LPreference* LambdaPreference = TopPreference.get();

        LTabBarTabDescriptor Descriptor;
        Descriptor.IdentifierField = TopPreference->GetName().ToString();
        Descriptor.DisplayNameField = TopPreference->GetDisplayName();
        Descriptor.PaddingField = LPadding(7.0f, 0.0f, 0.0f, 0.0f);
        Descriptor.PanelWidgetClassField = this->PanelClass;
        Descriptor.CallbackField = [this, LambdaPreference](WTabBar* TabBar, WNode* Button, WNode* Panel) -> void
        {
            if (Panel)
            {
                JPreferencesPanelData* Data{ NewObject<JPreferencesPanelData>(this->GetOuter()) };
                Data->Preference = LambdaPreference;
                Panel->AddData(*Data);

                Data->MarkAsGarbage_v2();
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
