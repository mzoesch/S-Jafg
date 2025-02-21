// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/Name.h"

namespace Jafg
{

class LPreference;
class LPreferenceCollection;
class LPreferenceValue;
class WWidgetParentBase;

MAKE_DELEGATE_SIGNATURE(LBuildPreference, void, const LPreference* Self, WWidgetParentBase* Target)

//#
//# A preference adjustable by the user, that comes with default user interface and cli support.
//#
class LPreference
{
    friend LPreferenceCollection;

public:

    LPreference(const LName InName, const LString& InDisplayName) : Name(InName), DisplayName(InDisplayName) { }
    LPreference(const LName InName, LString&& InDisplayName) : Name(InName), DisplayName(std::move(InDisplayName)) { }
    LPreference(const LName InName, const LString& InDisplayName, LBuildPreference&& InBuildDelegate) : Name(InName), DisplayName(InDisplayName), OnBuild(std::move(InBuildDelegate)) { }
    LPreference(const LName InName, LString&& InDisplayName, LBuildPreference&& InBuildDelegate) : Name(InName), DisplayName(std::move(InDisplayName)), OnBuild(std::move(InBuildDelegate)) { }
    PROHIBIT_COPY(LPreference)
    DEFAULT_MOVE(LPreference)
    virtual ~LPreference(void) = default;

    FORCEINLINE const LName& GetName(void) const { return this->Name; }
    FORCEINLINE const LString& GetDisplayName(void) const { return this->DisplayName; }

    FORCEINLINE virtual const TdhArray<Smart::TUnique<LPreference>>& GetChildPreferences(void) const
    {
        static const TdhArray<Smart::TUnique<LPreference>> _; return _;
    }

    FORCEINLINE virtual bool IsLeaf() const { return true; }
    FORCEINLINE bool IsBuildable() const { return this->OnBuild.IsBound(); }
    FORCEINLINE void SetBuildPreference(LBuildPreference&& InBuildPreference) { this->OnBuild = std::move(InBuildPreference); }
    FORCEINLINE bool Build(WWidgetParentBase* Target) const { check( this->IsLeaf() ) return this->OnBuild.InvokeIfBound(this, Target); }

private:

    LName   Name;
    LString DisplayName;
    LBuildPreference OnBuild;
};

} /* ~Namespace Jafg */
