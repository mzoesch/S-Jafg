// Copyright mzoesch. All rights reserved.

#include "User/Preferences/PreferenceRegistry.h"

void Jafg::JPreferenceRegistry::AddTopLevelPreference(Smart::TUnique<LPreference>&& InPreference)
{
    jassert( InPreference.IsValid() )

    if (this->Preferences.ContainsByPredicate([&InPreference](const Smart::TUnique<LPreference>& Preference)
    {
        return Preference->GetName() == InPreference->GetName();
    }))
    {
        panicMsgf( "Cannot add duplicate preference [{}::{}].", InPreference->GetName().ToString(), InPreference->GetDisplayName())
        return;
    }

    this->Preferences.Add(std::move(InPreference));
    checkSlow( InPreference.IsValid() == false )

    return;
}
