// Copyright mzoesch. All rights reserved.

#pragma once

#include "Containers/MyStringUtility.h"
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

    FORCEINLINE LPreference(const LName InName) noexcept
        : Name(InName), DisplayName(Strings::AddSpacesToCamelCase(InName.ToString())) { }
    FORCEINLINE LPreference(const LName InName, LBuildPreference&& InBuildDelegate) noexcept
        : Name(InName), DisplayName(Strings::AddSpacesToCamelCase(InName.ToString())), OnBuildDelegate(std::move(InBuildDelegate)) { }

    FORCEINLINE LPreference(LName InName, const LString& InDisplayName) noexcept
        : Name(std::move(InName)), DisplayName(InDisplayName) { }
    FORCEINLINE LPreference(LName InName, const LString& InDisplayName, LBuildPreference&& InBuildDelegate) noexcept
        : Name(std::move(InName)), DisplayName(InDisplayName), OnBuildDelegate(std::move(InBuildDelegate)) { }

    FORCEINLINE LPreference(LName InName, LString&& InDisplayName) noexcept
        : Name(std::move(InName)), DisplayName(std::move(InDisplayName)) { }
    FORCEINLINE LPreference(LName InName, LString&& InDisplayName, LBuildPreference&& InBuildDelegate) noexcept
        : Name(std::move(InName)), DisplayName(std::move(InDisplayName)), OnBuildDelegate(std::move(InBuildDelegate)) { }

    PROHIBIT_COPY(LPreference)
    DEFAULT_MOVE(LPreference)

    virtual ~LPreference(void) = default;

    FORCEINLINE bool operator==(const LPreference& InOther) const { return this->Name == InOther.Name; }
    FORCEINLINE bool operator!=(const LPreference& InOther) const { return this->Name != InOther.Name; }
    FORCEINLINE bool operator==(const LName InOther) const { return this->Name == InOther; }
    FORCEINLINE bool operator!=(const LName InOther) const { return this->Name != InOther; }

    FORCEINLINE const LName& GetName(void) const { return this->Name; }
    FORCEINLINE const LString& GetDisplayName(void) const { return this->DisplayName; }

    FORCEINLINE virtual const TArray<TUnique<LPreference>>& GetChildPreferences() const;
    FORCEINLINE virtual const TArray<TUnique<LPreference>>& LoadAndGetChildPreferences();

    FORCEINLINE virtual bool IsLeaf() const { return true; }
    FORCEINLINE bool IsBuildable() const { return this->OnBuildDelegate.IsBound(); }
    FORCEINLINE void OnBuild(LBuildPreference&& InBuildPreference) { this->OnBuildDelegate = std::move(InBuildPreference); }

    bool Build(WParentBase* Target) const;

private:

    LName Name;
    LString DisplayName;
    LBuildPreference OnBuildDelegate;
};

FORCEINLINE const TArray<TUnique<LPreference>>& LPreference::GetChildPreferences() const
{
    static const TArray<TUnique<LPreference>> _; return _;
}

FORCEINLINE const TArray<TUnique<LPreference>>& LPreference::LoadAndGetChildPreferences()
{
    return this->GetChildPreferences();
}

FORCEINLINE bool LPreference::Build(WParentBase* Target) const
{
    if (this->IsBuildable())
    {
        LOG_VERBOSE(LogPreferences, "Building preference [{}].", this->Name)
    }

    return this->OnBuildDelegate.InvokeIfBound(this, Target);
}
} /* ~Namespace Jafg */
