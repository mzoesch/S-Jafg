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
#include "Widgets/TextBlock.h"

void Jafg::JCorePreferencesSubsystem::Initialize(LSubsystemCollection& Collection)
{
    Super::Initialize(Collection);

    JUserPreferences*    UserPreferences = GetMutableDefault<JUserPreferences>();
    JPreferenceRegistry* Registry        = GetMutableDefault<JPreferenceRegistry>();

    {
        Smart::TUnique<LPreferenceCollection> Screen = Smart::MakeUnique<LPreferenceCollection>(new LPreferenceCollection(Name_PrefGameplay, "Gameplay"));
        Registry->AddTopLevelPreference(std::move(Screen));
    }

    {
        Smart::TUnique<LPreferenceCollection> Screen = Smart::MakeUnique<LPreferenceCollection>(new LPreferenceCollection(Name_PrefAudio, "Audio"));

        {
            Smart::TUnique<LPreferenceValue_Scalar> Preference = Smart::MakeUnique<LPreferenceValue_Scalar>(new LPreferenceValue_Scalar(MAKE_DYNAMIC_NAME("MasterVolume"), "Master Volume"));
            Preference->SetDefaultValue(UserPreferences->MasterVolume);
            Preference->SetValueGetter([UserPreferences](void) -> f64 { return UserPreferences->MasterVolume; });
            Preference->SetValueSetter([UserPreferences](const f64 Value) -> void { UserPreferences->MasterVolume = static_cast<float>(Value); });
            if (UserPreferences->MasterVolume.IsMinValueValid()) { Preference->SetMinimum(UserPreferences->MasterVolume.GetMinValue()); }
            if (UserPreferences->MasterVolume.IsMaxValueValid()) { Preference->SetMaximum(UserPreferences->MasterVolume.GetMaxValue()); }
            Preference->SetDisplayFormat(LPreferenceValue_Scalar::Fmt_ZeroToOneAsPercent);
            Screen->AddPreference(std::move(Preference));
        }

        {
            Smart::TUnique<LPreferenceValue_Scalar> Preference = Smart::MakeUnique<LPreferenceValue_Scalar>(new LPreferenceValue_Scalar(MAKE_DYNAMIC_NAME("MusicVolume"), "Music Volume"));
            Preference->SetDefaultValue(UserPreferences->MusicVolume);
            Preference->SetValueGetter([UserPreferences](void) -> f64 { return UserPreferences->MusicVolume; });
            Preference->SetValueSetter([UserPreferences](const f64 Value) -> void { UserPreferences->MusicVolume = static_cast<float>(Value); });
            if (UserPreferences->MusicVolume.IsMinValueValid()) { Preference->SetMinimum(UserPreferences->MusicVolume.GetMinValue()); }
            if (UserPreferences->MusicVolume.IsMaxValueValid()) { Preference->SetMaximum(UserPreferences->MusicVolume.GetMaxValue()); }
            Preference->SetDisplayFormat(LPreferenceValue_Scalar::Fmt_ZeroToOneAsPercent);
            Screen->AddPreference(std::move(Preference));
        }

        {
            Smart::TUnique<LPreferenceValue_Scalar> Preference = Smart::MakeUnique<LPreferenceValue_Scalar>(new LPreferenceValue_Scalar(MAKE_DYNAMIC_NAME("MiscVolume"), "Misc Volume"));
            Preference->SetDefaultValue(UserPreferences->MiscVolume);
            Preference->SetValueGetter([UserPreferences](void) -> f64 { return UserPreferences->MiscVolume; });
            Preference->SetValueSetter([UserPreferences](const f64 Value) -> void { UserPreferences->MiscVolume = static_cast<float>(Value); });
            if (UserPreferences->MiscVolume.IsMinValueValid()) { Preference->SetMinimum(UserPreferences->MiscVolume.GetMinValue()); }
            if (UserPreferences->MiscVolume.IsMaxValueValid()) { Preference->SetMaximum(UserPreferences->MiscVolume.GetMaxValue()); }
            Preference->SetDisplayFormat(LPreferenceValue_Scalar::Fmt_ZeroToOneAsPercent);
            Screen->AddPreference(std::move(Preference));
        }

        {
            Smart::TUnique<LPreferenceValue_Scalar> Preference = Smart::MakeUnique<LPreferenceValue_Scalar>(new LPreferenceValue_Scalar(MAKE_DYNAMIC_NAME("VoiceVolume"), "Voice Volume"));
            Preference->SetDefaultValue(UserPreferences->VoiceVolume);
            Preference->SetValueGetter([UserPreferences](void) -> f64 { return UserPreferences->VoiceVolume; });
            Preference->SetValueSetter([UserPreferences](const f64 Value) -> void { UserPreferences->VoiceVolume = static_cast<float>(Value); });
            if (UserPreferences->VoiceVolume.IsMinValueValid()) { Preference->SetMinimum(UserPreferences->VoiceVolume.GetMinValue()); }
            if (UserPreferences->VoiceVolume.IsMaxValueValid()) { Preference->SetMaximum(UserPreferences->VoiceVolume.GetMaxValue()); }
            Preference->SetDisplayFormat(LPreferenceValue_Scalar::Fmt_ZeroToOneAsPercent);
            Screen->AddPreference(std::move(Preference));
        }

        Registry->AddTopLevelPreference(std::move(Screen));
    }

    {
        Smart::TUnique<LPreferenceCollection> Screen = Smart::MakeUnique<LPreferenceCollection>(new LPreferenceCollection(Name_PrefVideo, "Video"));
        Registry->AddTopLevelPreference(std::move(Screen));
    }

    {
        Smart::TUnique<LPreferenceCollection> Screen = Smart::MakeUnique<LPreferenceCollection>(new LPreferenceCollection(Name_PrefControls, "Controls"));
        Registry->AddTopLevelPreference(std::move(Screen));
    }

    {
        Smart::TUnique<LPreferenceCollection> Screen = Smart::MakeUnique<LPreferenceCollection>(new LPreferenceCollection(Name_PrefKeybindings, "Keybindings"));
        Registry->AddTopLevelPreference(std::move(Screen));
    }

    {
        Smart::TUnique<LPreferenceCollection> Screen = Smart::MakeUnique<LPreferenceCollection>(new LPreferenceCollection(Name_PrefUserInterface, "User Interface"));
        Registry->AddTopLevelPreference(std::move(Screen));
    }

    {
        Smart::TUnique<LIntermediatePreferenceCollection> Screen = Smart::MakeUnique<LIntermediatePreferenceCollection>(new LIntermediatePreferenceCollection(Name_PrefDeveloper, "Developer"));
        Screen->OnBuild([](const LPreference* Self, WParentBase* Target) -> void
        {
            check( Self && Target )

            WHRegion* Container;

            NewNodeCtx(Target, WHRegion).SaveTo(&Container)
                .Anchor(EAnchor::HFill)
            [
                NewNodeCtx(Target, WTextBlock)
                    .Anchor(EAnchor::VCenter | EAnchor::HFill)
                    .Brush(LTextBlockBrush::SubHeader())
                    .Content(Self->GetDisplayName())
                +
                NewNodeCtx(Target, WTextButton)
                    .Anchor(EAnchor::VCenter)
                    .Content("Refresh")
                    .TextBlockBrush(LTextBlockBrush::Body())
                    .OnPrimaryRelease([Target](WButton* Self, LKeyEvent& InKeyEvent) -> void
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

                        Smart::TUnique<LPreference>* P = Registry->GetMutablePreferences().FindRefByPredicate([](const Smart::TUnique<LPreference>& InPreference)
                        {
                            return InPreference->GetName() == Name_PrefDeveloper;
                        });
                        if (P == nullptr || P->IsValid() == false)
                        {
                            return;
                        }

                        LPreferencesPanelData Data;
                        Data.DerivedClass = WPreferencesPanel::StaticClass()->GetName();
                        Data.Preference   = P->GetValuePtr();
                        Panel->AddData(&Data);

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
                LOG_ERROR(LogPreferences, "Failed to load intermediate preference collection [{}] due to engine absence.", InCollection->GetName())
                return;
            }

            LCommandLineInterface* Cli = GEngine->GetCommandLineInterface();
            checkSlow( Cli )

            {
                Smart::TUnique<LPreferenceCollection> Collection = Smart::EmplaceUnique<LPreferenceCollection>(Name_PrefDeveloperTypes, "Cli Types");

                for (const LCliType& Type : Cli->GetTypes())
                {
                    TOptional<LCliObjectHandle> Handle = Cli->GetHandle(Type);
                    if (Handle.IsSet() == false)
                    {
                        LOG_WARNING(LogPreferences, "Encountered invalid type handle.")
                        continue;
                    }

                    Smart::TUnique<LPreferenceValue_CliType> T = Smart::EmplaceUnique<LPreferenceValue_CliType>(
                        MAKE_DYNAMIC_NAME(LString::SprintF("CliType_{}", Type.GetIdentifier())),
                        LString::SprintF("Cli Type {}", Type.GetIdentifier()),
                        Handle.GetValue()
                        );

                    Collection->AddPreference(std::move(T));

                    continue;
                }

                InCollection->AddPreference(std::move(Collection));
            }

            {
                Smart::TUnique<LPreferenceCollection> Collection = Smart::EmplaceUnique<LPreferenceCollection>(Name_PrefDeveloperCmds, "Cli Commands");

                for (const LCliCommand& Type : Cli->GetCommands())
                {
                    TOptional<LCliObjectHandle> Handle = Cli->GetHandle(Type);
                    if (Handle.IsSet() == false)
                    {
                        LOG_WARNING(LogPreferences, "Encountered invalid command handle.")
                        continue;
                    }

                    Smart::TUnique<LPreferenceValue_CliCommand> T = Smart::EmplaceUnique<LPreferenceValue_CliCommand>(
                        MAKE_DYNAMIC_NAME(LString::SprintF("CliCmd_{}", Type.GetIdentifier())),
                        LString::SprintF("Cli Cmd {}", Type.GetIdentifier()),
                        Handle.GetValue()
                        );

                    Collection->AddPreference(std::move(T));

                    continue;
                }

                InCollection->AddPreference(std::move(Collection));
            }

            {
                Smart::TUnique<LPreferenceCollection> Collection = Smart::EmplaceUnique<LPreferenceCollection>(Name_PrefDeveloperVars, "Cli Vars");

                for (const LCliVariable& Type : Cli->GetVariables())
                {
                    TOptional<LCliObjectHandle> Handle = Cli->GetHandle(Type);
                    if (Handle.IsSet() == false)
                    {
                        LOG_WARNING(LogPreferences, "Encountered invalid variable handle.")
                        continue;
                    }

                    Smart::TUnique<LPreferenceValue_CliVariable> T = Smart::EmplaceUnique<LPreferenceValue_CliVariable>(
                        MAKE_DYNAMIC_NAME(LString::SprintF("CliVar_{}", Type.GetIdentifier())),
                        LString::SprintF("Cli Var {}", Type.GetIdentifier()),
                        Handle.GetValue()
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
