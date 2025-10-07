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

    void AddTopLevelPreference(TUnique<LPreference>&& InPreference);

    FORCEINLINE virtual       TArray<TUnique<LPreference>>& GetMutablePreferences() { return this->Preferences; }
    FORCEINLINE virtual const TArray<TUnique<LPreference>>& GetPreferences() const { return this->Preferences; }

protected:

    TArray<TUnique<LPreference>> Preferences;
};

} /* ~Namespace Jafg */
