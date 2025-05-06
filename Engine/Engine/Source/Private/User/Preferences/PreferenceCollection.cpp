// Copyright mzoesch. All rights reserved.

#include "User/Preferences/PreferenceCollection.h"

void Jafg::LPreferenceCollection::AddPreference(Smart::TUnique<LPreference>&& InPreference)
{
    jassert( InPreference.GetValuePtr() )

    if (this->Preferences.ContainsByPredicate([&InPreference](const Smart::TUnique<LPreference>& Preference)
    {
        return Preference->GetName() == InPreference->GetName();
    }))
    {
        panicMsgf( "Cannot add duplicate preference [{}].", InPreference->GetDisplayName())
        return;
    }

    this->Preferences.Add(std::move(InPreference));

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
                    static_cast<LPreferenceCollection*>(Preference.GetValuePtr())->GetPreferenceByIdentifier(InIdentifier);
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

const Jafg::TArray<Jafg::Smart::TUnique<Jafg::LPreference>>& Jafg::LIntermediatePreferenceCollection::LoadAndGetChildPreferences()
{
    if (this->Refresh() == false)
    {
        LOG_WARNING(LogPreferences, "Failed to load preferences for collection [{}].", this->GetDisplayName())
    }

    return LPreferenceCollection::LoadAndGetChildPreferences();
}

bool Jafg::LIntermediatePreferenceCollection::Refresh()
{
    if (this->OnLoad.IsBound() == false)
    {
        return false;
    }

    this->Preferences.Reset(this->Preferences.GetSize());
    this->OnLoad.Invoke(this);

    return true;
}
