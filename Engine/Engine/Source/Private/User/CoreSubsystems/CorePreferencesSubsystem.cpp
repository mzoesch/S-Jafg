// Copyright mzoesch. All rights reserved.

#include "User/CoreSubsystems/CorePreferencesSubsystem.h"
#include "Cli/CliFrontend.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "User/UserPreferences.h"
#include "User/Frontend/Osd/PreferencesScreen.h"
#include "User/Preferences/PreferenceRegistry.h"
#include "User/Preferences/PreferenceCollection.h"
#include "User/Preferences/PreferenceValue.h"
#include "User/Preferences/CommonPreferenceValues.h"
#include "Widgets/Button.h"
#include "Widgets/HRegion.h"
#include "Widgets/Spacer.h"
#include "Widgets/TextBox.h"
#include "User/Input/InputAction.h"

void Jafg::JCorePreferencesSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    JUserPreferences*    UserPreferences = GetMutableDefault<JUserPreferences>();
    JPreferenceRegistry* Registry        = GetMutableDefault<JPreferenceRegistry>();

    {
        TUnique<LPreferenceCollection> Screen = std::make_unique<LPreferenceCollection>(Name_PrefGameplay, "Gameplay");
        Registry->AddTopLevelPreference(std::move(Screen));
    }

    {
        TUnique<LPreferenceCollection> Screen = std::make_unique<LPreferenceCollection>(Name_PrefAudio, "Audio");

        {
            TUnique<LPreferenceValue_Scalar> Preference = std::make_unique<LPreferenceValue_Scalar>(MAKE_NAME("MasterVolume"), "Master Volume");
            Preference->SetDefaultValue(UserPreferences->MasterVolume);
            Preference->SetValueGetter([UserPreferences](void) -> f64 { return UserPreferences->MasterVolume; });
            Preference->SetValueSetter([UserPreferences](const f64 Value) -> void { UserPreferences->MasterVolume = static_cast<f32>(Value); });
            if (UserPreferences->MasterVolume.IsMinValueValid()) { Preference->SetMinimum(UserPreferences->MasterVolume.GetMinValue()); }
            if (UserPreferences->MasterVolume.IsMaxValueValid()) { Preference->SetMaximum(UserPreferences->MasterVolume.GetMaxValue()); }
            Preference->SetDisplayFormat(LPreferenceValue_Scalar::Fmt_ZeroToOneAsPercent);
            Screen->AddPreference(std::move(Preference));
        }

        {
            TUnique<LPreferenceValue_Scalar> Preference = std::make_unique<LPreferenceValue_Scalar>(MAKE_NAME("MusicVolume"), "Music Volume");
            Preference->SetDefaultValue(UserPreferences->MusicVolume);
            Preference->SetValueGetter([UserPreferences](void) -> f64 { return UserPreferences->MusicVolume; });
            Preference->SetValueSetter([UserPreferences](const f64 Value) -> void { UserPreferences->MusicVolume = static_cast<f32>(Value); });
            if (UserPreferences->MusicVolume.IsMinValueValid()) { Preference->SetMinimum(UserPreferences->MusicVolume.GetMinValue()); }
            if (UserPreferences->MusicVolume.IsMaxValueValid()) { Preference->SetMaximum(UserPreferences->MusicVolume.GetMaxValue()); }
            Preference->SetDisplayFormat(LPreferenceValue_Scalar::Fmt_ZeroToOneAsPercent);
            Screen->AddPreference(std::move(Preference));
        }

        {
            TUnique<LPreferenceValue_Scalar> Preference = std::make_unique<LPreferenceValue_Scalar>(MAKE_NAME("MiscVolume"), "Misc Volume");
            Preference->SetDefaultValue(UserPreferences->MiscVolume);
            Preference->SetValueGetter([UserPreferences](void) -> f64 { return UserPreferences->MiscVolume; });
            Preference->SetValueSetter([UserPreferences](const f64 Value) -> void { UserPreferences->MiscVolume = static_cast<f32>(Value); });
            if (UserPreferences->MiscVolume.IsMinValueValid()) { Preference->SetMinimum(UserPreferences->MiscVolume.GetMinValue()); }
            if (UserPreferences->MiscVolume.IsMaxValueValid()) { Preference->SetMaximum(UserPreferences->MiscVolume.GetMaxValue()); }
            Preference->SetDisplayFormat(LPreferenceValue_Scalar::Fmt_ZeroToOneAsPercent);
            Screen->AddPreference(std::move(Preference));
        }

        {
            TUnique<LPreferenceValue_Scalar> Preference = std::make_unique<LPreferenceValue_Scalar>(MAKE_NAME("VoiceVolume"), "Voice Volume");
            Preference->SetDefaultValue(UserPreferences->VoiceVolume);
            Preference->SetValueGetter([UserPreferences](void) -> f64 { return UserPreferences->VoiceVolume; });
            Preference->SetValueSetter([UserPreferences](const f64 Value) -> void { UserPreferences->VoiceVolume = static_cast<f32>(Value); });
            if (UserPreferences->VoiceVolume.IsMinValueValid()) { Preference->SetMinimum(UserPreferences->VoiceVolume.GetMinValue()); }
            if (UserPreferences->VoiceVolume.IsMaxValueValid()) { Preference->SetMaximum(UserPreferences->VoiceVolume.GetMaxValue()); }
            Preference->SetDisplayFormat(LPreferenceValue_Scalar::Fmt_ZeroToOneAsPercent);
            Screen->AddPreference(std::move(Preference));
        }

        Registry->AddTopLevelPreference(std::move(Screen));
    }

    {
        TUnique<LPreferenceCollection> Screen = std::make_unique<LPreferenceCollection>(Name_PrefVideo, "Video");
        Registry->AddTopLevelPreference(std::move(Screen));
    }

    {
        TUnique<LPreferenceCollection> Screen = std::make_unique<LPreferenceCollection>(Name_PrefControls, "Controls");
        Registry->AddTopLevelPreference(std::move(Screen));
    }

    {
        TUnique<LIntermediatePreferenceCollection> Screen = std::make_unique<LIntermediatePreferenceCollection>(Name_PrefKeybindings, "Keybindings");
        Screen->OnBuild([](const LPreference* Self, WParentBase* Target) -> void
        {
            check( Self && Target )

            WHRegion* Container;

            NewNodeCtx(Target, WHRegion).SaveTo(&Container)
                .Anchor(EAnchor::HFill)
            [
                NewNodeCtx(Target, WTextBox)
                    .Anchor(EAnchor::VCenter | EAnchor::HFill)
                    .Brush(LTextBoxBrush::SubHeader())
                    .Content(Self->GetDisplayName())
                +
                NewNodeCtx(Target, WTextButton)
                    .Anchor(EAnchor::VCenter)
                    .Content("Refresh")
                    .TextBlockBrush(LTextBoxBrush::Body())
                    .OnPrimaryRelease([Target](WButton* Self, const LKeyEvent& InKeyEvent) -> void
                    {
                        check( Self )

                        if (Target->IsGarbage())
                        {
                            return;
                        }

                        WPreferencesPanel* Panel = DynamicCast<WPreferencesPanel>(Target->GetParent());
                        if (Panel == nullptr)
                        {
                            return;
                        }

                        JPreferenceRegistry* Registry = GetMutableDefault<JPreferenceRegistry>();
                        if (Registry == nullptr)
                        {
                            return;
                        }

                        auto* P { algo::find_pointer(Registry->GetMutablePreferences(), Name_PrefDeveloper, [](auto const& E){ return E->GetName(); }) };
                        if (P == nullptr || P->get() == nullptr)
                        {
                            return;
                        }

                        JPreferencesPanelData* Data{ NewObject<JPreferencesPanelData>(Self->GetOuter()) };
                        Data->Preference = P->get();
                        Panel->AddData(*Data);
                        Data->MarkAsGarbage_v2();

                        return;
                    })
            ];

            Target->AddChild(Container);

            return;
        });
        Screen->OnLoad([](LPreferenceCollection* InCollection) -> void
        {
            check( InCollection )

            LOG_VERBOSE(LogPreferences, "Loading intermediate preference collection [{}].", InCollection->GetName())

            if (GEngine == nullptr)
            {
                LOG_WARNING(LogPreferences, "Failed to load intermediate preference collection [{}] due to engine absence.", InCollection->GetName())
                return;
            }

            check( GEngine->GetLocalEgo().IsDecommissioned() == false )
            for (auto& Action : GEngine->GetLocalEgo().GetUserInputRegistry().GetRegisteredActions())
            {
                TUnique<LPreferenceValue_InputAction> T = std::make_unique<LPreferenceValue_InputAction>(Action.GetName());
                InCollection->AddPreference(std::move(T));

                continue;
            }

            return;

        });
        Registry->AddTopLevelPreference(std::move(Screen));
    }

    {
        TUnique<LPreferenceCollection> Screen = std::make_unique<LPreferenceCollection>(Name_PrefUserInterface, "User Interface");
        Registry->AddTopLevelPreference(std::move(Screen));
    }

    {
        TUnique<LIntermediatePreferenceCollection> Screen = std::make_unique<LIntermediatePreferenceCollection>(Name_PrefDeveloper, "Developer");
        Screen->OnBuild([](const LPreference* Self, WParentBase* Target) -> void
        {
            check( Self && Target )

            WHRegion* Container;

            NewNodeCtx(Target, WHRegion).SaveTo(&Container)
                .Anchor(EAnchor::HFill)
            [
                NewNodeCtx(Target, WTextBox)
                    .Anchor(EAnchor::VCenter | EAnchor::HFill)
                    .Brush(LTextBoxBrush::SubHeader())
                    .Content(Self->GetDisplayName())
                +
                NewNodeCtx(Target, WTextButton)
                    .Anchor(EAnchor::VCenter)
                    .Content("Refresh")
                    .TextBlockBrush(LTextBoxBrush::Body())
                    .OnPrimaryRelease([Target](WButton* Self, const LKeyEvent& InKeyEvent) -> void
                    {
                        if (Target->IsGarbage())
                        {
                            return;
                        }

                        WPreferencesPanel* Panel = DynamicCast<WPreferencesPanel>(Target->GetParent());
                        if (Panel == nullptr)
                        {
                            return;
                        }

                        JPreferenceRegistry* Registry = GetMutableDefault<JPreferenceRegistry>();
                        if (Registry == nullptr)
                        {
                            return;
                        }

                        auto* P { algo::find_pointer(Registry->GetMutablePreferences(), Name_PrefDeveloper, [](auto const& E){ return E->GetName(); }) };
                        if (P == nullptr || P->get() == nullptr)
                        {
                            return;
                        }

                        JPreferencesPanelData* Data { NewObject<JPreferencesPanelData>(Self->GetOuter()) };
                        Data->Preference = P->get();
                        Panel->AddData(*Data);
                        Data->MarkAsGarbage_v2();

                        return;
                    })
            ];

            Target->AddChild(Container);

            return;
        });
        Screen->OnLoad([](LPreferenceCollection* InCollection) -> void
        {
            check( InCollection )

            if (GEngine == nullptr)
            {
                LOG_WARNING(LogPreferences, "Failed to load intermediate preference collection [{}] due to engine absence.", InCollection->GetName())
                return;
            }

            LCommandLineInterface& Cli = GEngine->GetCommandLineInterface();
            {
                TUnique<LPreferenceCollection> Collection = std::make_unique<LPreferenceCollection>(Name_PrefDeveloperTypes, "Cli Types");

                for (const LCliType& Type : Cli.GetTypes())
                {
                    TOptional<LCliObjectHandle> Handle = Cli.GetHandle(Type);
                    if (Handle.has_value() == false)
                    {
                        LOG_WARNING(LogPreferences, "Encountered invalid type handle.")
                        continue;
                    }

                    TUnique<LPreferenceValue_CliType> T = std::make_unique<LPreferenceValue_CliType>
                    (
                        MAKE_NAME(Lal::SprintF("CliType_{}", Type.GetIdentifier())),
                        Lal::SprintF("Cli Type {}", Type.GetIdentifier()),
                        Handle.value()
                    );

                    Collection->AddPreference(std::move(T));

                    continue;
                }

                InCollection->AddPreference(std::move(Collection));
            }

            {
                TUnique<LPreferenceCollection> Collection = std::make_unique<LPreferenceCollection>(Name_PrefDeveloperCmds, "Cli Commands");

                for (const LCliCommand& Type : Cli.GetCommands())
                {
                    TOptional<LCliObjectHandle> Handle = Cli.GetHandle(Type);
                    if (Handle.has_value() == false)
                    {
                        LOG_WARNING(LogPreferences, "Encountered invalid command handle.")
                        continue;
                    }

                    TUnique<LPreferenceValue_CliCommand> T = std::make_unique<LPreferenceValue_CliCommand>
                    (
                        MAKE_NAME(Lal::SprintF("CliCmd_{}", Type.GetIdentifier())),
                        Lal::SprintF("Cli Cmd {}", Type.GetIdentifier()),
                        Handle.value()
                    );

                    Collection->AddPreference(std::move(T));

                    continue;
                }

                InCollection->AddPreference(std::move(Collection));
            }

            {
                TUnique<LPreferenceCollection> Collection = std::make_unique<LPreferenceCollection>(Name_PrefDeveloperVars, "Cli Vars");

                for (const LCliVariable& Type : Cli.GetVariables())
                {
                    TOptional<LCliObjectHandle> Handle = Cli.GetHandle(Type);
                    if (Handle.has_value() == false)
                    {
                        LOG_WARNING(LogPreferences, "Encountered invalid variable handle.")
                        continue;
                    }

                    TUnique<LPreferenceValue_CliVariable> T = std::make_unique<LPreferenceValue_CliVariable>
                    (
                        MAKE_NAME(Lal::SprintF("CliVar_{}", Type.GetIdentifier())),
                        Lal::SprintF("Cli Var {}", Type.GetIdentifier()),
                        Handle.value()
                    );

                    Collection->AddPreference(std::move(T));

                    continue;
                }

                InCollection->AddPreference(std::move(Collection));
            }

            return;
        });
        Registry->AddTopLevelPreference(std::move(Screen));
    }

    return;
}
