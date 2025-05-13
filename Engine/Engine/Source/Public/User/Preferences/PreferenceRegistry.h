// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"
#include "User/Preferences/Preference.h"
#include "PreferenceRegistry.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(EClassFlags::Singleton)
class ENGINE_API JPreferenceRegistry : public JObjectBase
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JPreferenceRegistry)

public:

    template <typename TPref> FORCEINLINE
    void AddTopLevelPreference(Smart::TUnique<TPref>&& InPreference);
    void AddTopLevelPreference(Smart::TUnique<LPreference>&& InPreference);

    FORCEINLINE virtual       TArray<Smart::TUnique<LPreference>>& GetMutablePreferences() { return this->Preferences; }
    FORCEINLINE virtual const TArray<Smart::TUnique<LPreference>>& GetPreferences() const { return this->Preferences; }

protected:

    TArray<Smart::TUnique<LPreference>> Preferences;
};

template <typename TPref>
FORCEINLINE void JPreferenceRegistry::AddTopLevelPreference(Smart::TUnique<TPref>&& InPreference)
{
    this->AddTopLevelPreference(Smart::UpcastUnique<LPreference>(std::forward<Smart::TUnique<TPref>>(InPreference)));
    check( InPreference.IsValid() == false )
    return;
}

} /* ~Namespace Jafg */
