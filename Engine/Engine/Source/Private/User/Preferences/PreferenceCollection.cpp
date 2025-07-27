// Copyright mzoesch. All rights reserved.

#include "User/Preferences/PreferenceCollection.h"

void Jafg::LPreferenceCollection::AddPreference(Smart::TUnique<LPreference>&& InPreference)
{
    jassert( InPreference.IsValid() )

    if (this->Preferences.ContainsByPredicate([&InPreference](const Smart::TUnique<LPreference>& Preference)
    {
        return Preference->GetName() == InPreference->GetName();
    }))
    {
        panicMsgf( "Cannot add duplicate preference [{}].", InPreference->GetDisplayName())
        return;
    }

    this->Preferences.Add(std::move(InPreference));
    checkSlow( InPreference.IsValid() == false )

    return;
}

Jafg::LPreference* Jafg::LPreferenceCollection::GetPreferenceByIdentifier(const LName InIdentifier)
{
    for (Smart::TUnique<LPreference>& Preference : this->Preferences)
    {
        if (Preference->GetName() == InIdentifier)
        {
            return Preference;
        }

        if (Preference->GetChildPreferences().GetSize() > 0)
        {
            if ( /* Very cheeky. Not a fan of this code. */
                LPreference* Out =
                    static_cast<LPreferenceCollection*>(Preference.GetPointerChecked())->GetPreferenceByIdentifier(InIdentifier);
                Out
            )
            {
                return Out;
            }
        }

        continue;
    }

    return nullptr;
}

Jafg::LPreference* Jafg::LPreferenceCollection::GetPreferenceByIdentifier(const LString& InIdentifier)
{
    return this->GetPreferenceByIdentifier(GET_NAME(InIdentifier));
}

const TArray<Smart::TUnique<Jafg::LPreference>>& Jafg::LIntermediatePreferenceCollection::LoadAndGetChildPreferences()
{
    if (this->Refresh() == false)
    {
        LOG_WARNING(LogPreferences, "Failed to load preferences for collection [{}].", this->GetDisplayName())
    }

    return LPreferenceCollection::LoadAndGetChildPreferences();
}

bool Jafg::LIntermediatePreferenceCollection::Refresh()
{
    if (this->OnLoadDelegate.IsBound() == false)
    {
        return false;
    }

    LOG_VERBOSE(LogPreferences, "Loading intermediate preference collection [{}].", this->GetName())
    this->Preferences.Reset(this->Preferences.GetSize());
    this->OnLoadDelegate.Invoke(this);

    return true;
}
