// Copyright mzoesch. All rights reserved.

#include "User/CoreSubsystems/CorePreferencesSubsystem.h"
#include "Core/CoreNames.h"
#include "User/UserPreferences.h"
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

            NewNodeCtx(Target, WTest).SaveTo(&Container)
                .Tint(LColor{0x00u, 0x7Fu, 0x00u, 0x7Fu})
                .Anchor(EAnchor::HFill)
            [
                NewNodeCtx(Target, WTextBlock)
                    .Brush(LTextBlockBrush::SubHeader())
                    .Content(Self->GetDisplayName())
                +
                NewNodeCtx(Target, WRegion)
                    .Anchor(EAnchor::HFill)
                    .MinDesiredSize(LVector2{5})
                    .Tint({0x00u, 0xFFu, 0x00u, 0x7Fu})
                +
                NewNodeCtx(Target, WTextButton)
                    .NormalBrush(LRegionBrush({.Tint = LColor{0x00u, 0xFFu, 0xFFu, 0x0Fu}}))
                    .Content("Refresh")
                    .TextBlockBrush(LTextBlockBrush::Body())
            ];

            Target->AddChild(Container);

            return;
        });
        Screen->OnLoad([](LPreferenceCollection* InCollection) -> void
        {
            check( InCollection )

            LOG_WARNING(LogTemporal, "Loading ....")

            return;
        });
        Registry->AddTopLevelPreference(std::move(Screen));
    }

    return;
}
