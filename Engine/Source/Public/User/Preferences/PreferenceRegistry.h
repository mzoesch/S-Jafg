// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/CxxClass.h"
#include "User/Preferences/Preference.h"
#include "PreferenceRegistry.generated.h"

namespace Jafg
{

DECLARE_JAFG_CLASS(ECxxClassFlags::Singleton)
class JPreferenceRegistry : public JCxxClass
{
    GENERATED_CLASS_BODY()

protected:

    DEFAULT_OBJECT_CONSTRUCTOR(JPreferenceRegistry)

public:

    void AddTopLevelPreference(TUnique<LPreference>&& InPreference);

    FORCEINLINE virtual       TArray<TUnique<LPreference>>& GetMutablePreferences() { return this->Preferences; }
    FORCEINLINE virtual const TArray<TUnique<LPreference>>& GetPreferences() const { return this->Preferences; }

protected:

    TCdrEmptyArray<TArray<TUnique<LPreference>>> Preferences;
};

} /* ~Namespace Jafg */
