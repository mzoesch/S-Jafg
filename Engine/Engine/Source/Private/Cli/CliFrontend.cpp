// Copyright mzoesch. All rights reserved.

#include "Cli/CliFrontend.h"

#include "Engine/Engine.h"
#include "Widgets/HRegion.h"
#include "Widgets/ParentBase.h"
#include "Widgets/TextBlock.h"

void Jafg::LPreferenceValue_CliType::StoreInitial()
{
}

void Jafg::LPreferenceValue_CliType::ResetToDefault()
{
}

void Jafg::LPreferenceValue_CliType::ResetToInitial()
{
}

void Jafg::LPreferenceValue_CliType::BuildDefault(const LPreference* Self, WParentBase* Target)
{
    if (GEngine == nullptr)
    {
        LOG_WARNING(LogPreferences, "Engine is invalid.")
        return;
    }

    const LPreferenceValue_CliType* This = static_cast<const LPreferenceValue_CliType*>(Self);


    const LCommandLineInterface* Cli = GEngine->GetCommandLineInterface();
    const LCliType* Type = Cli->GetType(This->Type);
    if (Type == nullptr)
    {
        LOG_ERROR(LogPreferences, "CliType is invalid.")
        return;
    }

    WParentBase* Container;

    const u8 ColorSpace = static_cast<u8>(20 * (Target->GetChildren().GetSize() % 2 == 0 ? 1.8 : 1));


    NewNodeCtx(Target, WHRegion).SaveTo(&Container)
        .Anchor(EAnchor::HFill)
        .Padding({15.0f, 10.0f})
        .Tint({ColorSpace, ColorSpace, ColorSpace, 192})
    [
        NewNodeCtx(Target, WTextBlock)
            .Anchor(EAnchor::VCenter)
            .Brush(LTextBlockBrush::Body())
            .MinDesiredSize({100.0f, 0.0f})
            .Content(Type->GetIdentifier())
        +
        NewNodeCtx(Target, WTextBlock)
            .Anchor(EAnchor::VCenter | EAnchor::HFill)
            .Brush(LTextBlockBrush::Body())
            .Content(Type->GetHelp())
        +
        NewNodeCtx(Target, WTextBlock)
            .Anchor(EAnchor::VCenter)
            .Brush(LTextBlockBrush::Body())
            .Align(ETextAlign::Right)
            .Content(Type->GetDefault())
    ];

    Target->AddChild(Container);

    return;
}

void Jafg::LPreferenceValue_CliCommand::StoreInitial()
{
}

void Jafg::LPreferenceValue_CliCommand::ResetToDefault()
{
}

void Jafg::LPreferenceValue_CliCommand::ResetToInitial()
{
}

void Jafg::LPreferenceValue_CliVariable::StoreInitial()
{
}

void Jafg::LPreferenceValue_CliVariable::ResetToDefault()
{
}

void Jafg::LPreferenceValue_CliVariable::ResetToInitial()
{
}
