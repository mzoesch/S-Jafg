// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/PreferencesScreen.h"

void Jafg::WPreferencesPanel::Construct()
{
    Super::Construct();
    return;
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

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Gameplay";
        Descriptor.PanelWidgetClass = this->PanelClass;
        this->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Audio";
        Descriptor.PanelWidgetClass = this->PanelClass;
        this->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Video";
        Descriptor.PanelWidgetClass = this->PanelClass;
        this->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Controls";
        Descriptor.PanelWidgetClass = this->PanelClass;
        this->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Keybindings";
        Descriptor.PanelWidgetClass = this->PanelClass;
        this->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "User Interface";
        Descriptor.PanelWidgetClass = this->PanelClass;
        this->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Developer";
        Descriptor.PanelWidgetClass = this->PanelClass;
        this->RegisterTab(std::move(Descriptor));
    }

    return;
}
