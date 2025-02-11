// Copyright mzoesch. All rights reserved.

#include "User/Frontend/Osd/PreferencesScreen.h"

Jafg::WPreferencesScreen::WPreferencesScreen(const LObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
    this->SetHorizontalPreference();
    this->SetAnchor(EAnchor::Fill);
    return;
}

void Jafg::WPreferencesScreen::Construct()
{
    this->SetTabBarDepth(1);

    Super::Construct();

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Gameplay";
        this->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Audio";
        this->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Video";
        this->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Controls";
        this->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Keybindings";
        this->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "User Interface";
        this->RegisterTab(std::move(Descriptor));
    }

    {
        LTabBarTabDescriptor Descriptor;
        Descriptor.Identifier = "Developer";
        this->RegisterTab(std::move(Descriptor));
    }

    return;
}
