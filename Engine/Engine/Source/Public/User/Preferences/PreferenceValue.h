// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Preferences/Preference.h"

namespace Jafg
{

//#
//# The base class for all preferences that are conceptually a value, that can be
//# changed, and thus reset or restored to their initial or default value.
//#
class LPreferenceValue : public LPreference
{
public:

    FORCEINLINE LPreferenceValue(LName InName) noexcept
        : LPreference(std::move(InName)) { }
    FORCEINLINE LPreferenceValue(LName InName, LBuildPreference&& InBuildDelegate) noexcept
        : LPreference(std::move(InName), std::move(InBuildDelegate)) { }

    FORCEINLINE LPreferenceValue(LName InName, const LString& InDisplayName) noexcept
        : LPreference(std::move(InName), InDisplayName) { }
    FORCEINLINE LPreferenceValue(LName InName, const LString& InDisplayName, LBuildPreference&& InBuildDelegate) noexcept
        : LPreference(std::move(InName), InDisplayName, std::move(InBuildDelegate)) { }

    FORCEINLINE LPreferenceValue(LName InName, LString&& InDisplayName) noexcept
        : LPreference(std::move(InName), std::move(InDisplayName)) { }
    FORCEINLINE LPreferenceValue(LName InName, LString&& InDisplayName, LBuildPreference&& InBuildDelegate) noexcept
        : LPreference(std::move(InName), std::move(InDisplayName), std::move(InBuildDelegate)) { }

    //#
    //# Stores an initial value for the setting.
    //#
    ENGINE_API virtual void StoreInitial() = 0;

    //#
    //# Resets the property to the default.
    //#
    ENGINE_API virtual void ResetToDefault() = 0;

    //#
    //# Restores the setting to the initial value.
    //# This is the value from the state of all preferences before tweaks through either the user interface
    //# or cli were made - in simple terms, the value that should be used currently (for non-immediate changes - so
    //# preferences that are applied immediately and do not require and explicit apply; e.g. because of heavy load
    //# and recalculating after changing said preference would be too expensive).
    //#
    ENGINE_API virtual void ResetToInitial() = 0;
};

} /* ~Namespace Jafg */
