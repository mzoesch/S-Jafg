// Copyright mzoesch. All rights reserved.

#include "User/Preferences/PreferenceRegistry.h"

void Jafg::JPreferenceRegistry::AddTopLevelPreference(TUnique<LPreference>&& InPreference)
{
    jassert(InPreference.get())

    if (algo::contains(this->Preferences, InPreference->GetName(), [](auto const& E){ return E->GetName(); }))
    {
        panicMsgf("Cannot add duplicate preference [{}::{}].", InPreference->GetName().ToString(), InPreference->GetDisplayName())
    }

    this->Preferences.emplace_back(std::move(InPreference));
    check(InPreference.get() == nullptr)

    return;
}
