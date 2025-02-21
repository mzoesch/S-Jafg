// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Preferences/Preference.h"

namespace Jafg
{

//#
//# A nestable collection of preferences.
//#
class LPreferenceCollection : public LPreference
{
public:

    LPreferenceCollection(const LName InName, const LString& InDisplayName) : LPreference(InName, InDisplayName) { }
    LPreferenceCollection(const LName InName, LString&& InDisplayName) : LPreference(InName, std::move(InDisplayName)) { }

    FORCEINLINE virtual bool IsLeaf() const override { return false; }

    template <typename TPref>
    FORCEINLINE void AddPreference(Smart::TUnique<TPref>&& InPreference);
    ENGINE_API  void AddPreference(Smart::TUnique<LPreference>&& InPreference);

    ENGINE_API  auto GetPreferenceByIdentifier(const LName InIdentifier) -> LPreference*;
    FORCEINLINE auto GetPreferenceByIdentifier(const LName InIdentifier) const -> const LPreference*;
    FORCEINLINE auto GetPreferenceByIdentifierChecked(const LName InIdentifier) -> LPreference*;
    FORCEINLINE auto GetPreferenceByIdentifierChecked(const LName InIdentifier) const -> const LPreference*;
    ENGINE_API  auto GetPreferenceByIdentifier(const LString& InIdentifier) -> LPreference*;
    FORCEINLINE auto GetPreferenceByIdentifier(const LString& InIdentifier) const -> const LPreference*;
    FORCEINLINE auto GetPreferenceByIdentifierChecked(const LString& InIdentifier) -> LPreference*;
    FORCEINLINE auto GetPreferenceByIdentifierChecked(const LString& InIdentifier) const -> const LPreference*;
    FORCEINLINE auto GetChildPreferences(void) const -> const TdhArray<Smart::TUnique<LPreference>>&  override { return this->Preferences; }

private:

    TdhArray<Smart::TUnique<LPreference>> Preferences;
};

template<typename TPref>
FORCEINLINE void LPreferenceCollection::AddPreference(Smart::TUnique<TPref>&& InPreference)
{
    this->AddPreference(Smart::UpcastUnique<LPreference>(std::forward<Smart::TUnique<TPref>>(InPreference)));
    check( InPreference.IsValid() == false )
    return;
}

FORCEINLINE auto LPreferenceCollection::GetPreferenceByIdentifier(const LName InIdentifier) const -> const LPreference*
{
    return const_cast<LPreferenceCollection*>(this)->GetPreferenceByIdentifier(InIdentifier);
}

FORCEINLINE LPreference* LPreferenceCollection::GetPreferenceByIdentifierChecked(const LName InIdentifier)
{
    LPreference* const Preference = this->GetPreferenceByIdentifier(InIdentifier);
    check(Preference)
    return Preference;
}

FORCEINLINE const LPreference* LPreferenceCollection::GetPreferenceByIdentifierChecked(const LName InIdentifier) const
{
    const LPreference* const Preference = this->GetPreferenceByIdentifier(InIdentifier);
    check(Preference)
    return Preference;
}

FORCEINLINE auto LPreferenceCollection::GetPreferenceByIdentifier(const LString& InIdentifier) const -> const LPreference*
{
    return const_cast<LPreferenceCollection*>(this)->GetPreferenceByIdentifier(InIdentifier);
}

FORCEINLINE LPreference* LPreferenceCollection::GetPreferenceByIdentifierChecked(const LString& InIdentifier)
{
    LPreference* const Preference = this->GetPreferenceByIdentifier(InIdentifier);
    check(Preference)
    return Preference;
}

FORCEINLINE const LPreference* LPreferenceCollection::GetPreferenceByIdentifierChecked(const LString& InIdentifier) const
{
    const LPreference* const Preference = this->GetPreferenceByIdentifier(InIdentifier);
    check(Preference)
    return Preference;
}

} /* ~Namespace Jafg */
