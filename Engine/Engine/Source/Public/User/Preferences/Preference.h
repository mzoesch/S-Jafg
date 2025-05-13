// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/Name.h"

namespace Jafg
{

class LPreference;
class LPreferenceCollection;
class LPreferenceValue;
class WParentBase;

MAKE_DELEGATE_SIGNATURE(LBuildPreference, void, const LPreference* Self, WParentBase* Target)

//#
//# A preference adjustable by the user, that comes with default user interface and cli support.
//#
class LPreference
{
    friend LPreferenceCollection;

public:

    LPreference(const LName InName, const LString& InDisplayName) : Name(InName), DisplayName(InDisplayName) { }
    LPreference(const LName InName, LString&& InDisplayName) : Name(InName), DisplayName(std::move(InDisplayName)) { }
    LPreference(const LName InName, const LString& InDisplayName, LBuildPreference&& InBuildDelegate) : Name(InName), DisplayName(InDisplayName), OnBuildDelegate(std::move(InBuildDelegate)) { }
    LPreference(const LName InName, LString&& InDisplayName, LBuildPreference&& InBuildDelegate) : Name(InName), DisplayName(std::move(InDisplayName)), OnBuildDelegate(std::move(InBuildDelegate)) { }
    PROHIBIT_COPY(LPreference)
    DEFAULT_MOVE(LPreference)
    virtual ~LPreference(void) = default;

    FORCEINLINE bool operator==(const LPreference& InOther) const { return this->Name == InOther.Name; }
    FORCEINLINE bool operator!=(const LPreference& InOther) const { return this->Name != InOther.Name; }
    FORCEINLINE bool operator==(const LName InOther) const { return this->Name == InOther; }
    FORCEINLINE bool operator!=(const LName InOther) const { return this->Name != InOther; }

    FORCEINLINE const LName& GetName(void) const { return this->Name; }
    FORCEINLINE const LString& GetDisplayName(void) const { return this->DisplayName; }

    FORCEINLINE virtual const TArray<Smart::TUnique<LPreference>>& GetChildPreferences() const;
    FORCEINLINE virtual const TArray<Smart::TUnique<LPreference>>& LoadAndGetChildPreferences();

    FORCEINLINE virtual bool IsLeaf() const { return true; }
    FORCEINLINE bool IsBuildable() const { return this->OnBuildDelegate.IsBound(); }
    FORCEINLINE void OnBuild(LBuildPreference&& InBuildPreference) { this->OnBuildDelegate = std::move(InBuildPreference); }
    FORCEINLINE bool Build(WParentBase* Target) const { return this->OnBuildDelegate.InvokeIfBound(this, Target); }

private:

    LName Name;
    LString DisplayName;
    LBuildPreference OnBuildDelegate;
};

FORCEINLINE const TArray<Smart::TUnique<LPreference>>& LPreference::GetChildPreferences() const
{
    static const TArray<Smart::TUnique<LPreference>> _; return _;
}

FORCEINLINE const TArray<Smart::TUnique<LPreference>>& LPreference::LoadAndGetChildPreferences()
{
    return this->GetChildPreferences();
}

} /* ~Namespace Jafg */
