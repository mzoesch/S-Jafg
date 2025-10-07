// Copyright mzoesch. All rights reserved.

#include "User/Preferences/PreferenceCollection.h"

void Jafg::LPreferenceCollection::AddPreference(TUnique<LPreference>&& InPreference)
{
    jassert( InPreference.get() )

    if (algo::contains(this->Preferences, InPreference->GetName(), [](auto const& E){ return E->GetName(); }))
    {
        panicMsgf( "Cannot add duplicate preference [{}].", InPreference->GetDisplayName())
        return;
    }

    this->Preferences.emplace_back(std::move(InPreference));
    checkSlow( InPreference.get() == nullptr )

    return;
}

Jafg::LPreference* Jafg::LPreferenceCollection::GetPreferenceByIdentifier(const LName InIdentifier)
{
    for (TUnique<LPreference>& Preference : this->Preferences)
    {
        if (Preference->GetName() == InIdentifier)
        {
            return Preference.get();
        }

        if (Preference->GetChildPreferences().size() > 0)
        {
            if ( /* Very cheeky. Not a fan of this code. */
                LPreference* Out =
                    static_cast<LPreferenceCollection*>(Preference.get())->GetPreferenceByIdentifier(InIdentifier);
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

const TArray<TUnique<Jafg::LPreference>>& Jafg::LIntermediatePreferenceCollection::LoadAndGetChildPreferences()
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
    this->Preferences.clear();
    this->Preferences.reserve(this->Preferences.size());
    this->OnLoadDelegate.Invoke(this);

    return true;
}
