// Copyright mzoesch. All rights reserved.

#pragma once

#include "CommandLineInterface.h"
#include "User/Preferences/PreferenceValue.h"

namespace Jafg
{

class LPreferenceValue_CliType : public LPreferenceValue
{
public:

    LPreferenceValue_CliType(const LName InName, const LString& InDisplayName, const LCliTypeHandle InType)
        : LPreferenceValue(InName, InDisplayName, LBuildPreference::CreateWeakFunction(LPreferenceValue_CliType::BuildDefault)), Type(InType) { }
    LPreferenceValue_CliType(const LName InName, LString&& InDisplayName, const LCliTypeHandle InType)
        : LPreferenceValue(InName, std::move(InDisplayName), LBuildPreference::CreateWeakFunction(LPreferenceValue_CliType::BuildDefault)), Type(InType) { }
    LPreferenceValue_CliType(const LName InName, const LString& InDisplayName, LBuildPreference&& InBuildDelegate, const LCliTypeHandle InType)
        : LPreferenceValue(InName, InDisplayName, std::move(InBuildDelegate)), Type(InType) { }
    LPreferenceValue_CliType(const LName InName, LString&& InDisplayName, LBuildPreference&& InBuildDelegate, const LCliTypeHandle InType)
        : LPreferenceValue(InName, std::move(InDisplayName), std::move(InBuildDelegate)), Type(InType) { }

    ENGINE_API virtual void StoreInitial() override;
    ENGINE_API virtual void ResetToDefault() override;
    ENGINE_API virtual void ResetToInitial() override;

    ENGINE_API static void BuildDefault(const LPreference* Self, WParentBase* Target);

private:

    LCliTypeHandle Type;
};

class LPreferenceValue_CliCommand : public LPreferenceValue
{
public:

    LPreferenceValue_CliCommand(const LName InName, const LString& InDisplayName, const LCliTypeHandle InType)
    : LPreferenceValue(InName, InDisplayName, LBuildPreference::CreateWeakFunction(LPreferenceValue_CliCommand::BuildDefault)), Type(InType) { }
    LPreferenceValue_CliCommand(const LName InName, LString&& InDisplayName, const LCliTypeHandle InType)
        : LPreferenceValue(InName, std::move(InDisplayName), LBuildPreference::CreateWeakFunction(LPreferenceValue_CliCommand::BuildDefault)), Type(InType) { }
    LPreferenceValue_CliCommand(const LName InName, const LString& InDisplayName, LBuildPreference&& InBuildDelegate, const LCliTypeHandle InType)
        : LPreferenceValue(InName, InDisplayName, std::move(InBuildDelegate)), Type(InType) { }
    LPreferenceValue_CliCommand(const LName InName, LString&& InDisplayName, LBuildPreference&& InBuildDelegate, const LCliTypeHandle InType)
        : LPreferenceValue(InName, std::move(InDisplayName), std::move(InBuildDelegate)), Type(InType) { }

    ENGINE_API virtual void StoreInitial() override;
    ENGINE_API virtual void ResetToDefault() override;
    ENGINE_API virtual void ResetToInitial() override;

    ENGINE_API static void BuildDefault(const LPreference* Self, WParentBase* Target);

private:

    LCliCommandHandle Type;
};

class LPreferenceValue_CliVariable : public LPreferenceValue
{
public:

    LPreferenceValue_CliVariable(const LName InName, const LString& InDisplayName, const LCliTypeHandle InType)
    : LPreferenceValue(InName, InDisplayName, LBuildPreference::CreateWeakFunction(LPreferenceValue_CliVariable::BuildDefault)), Type(InType) { }
    LPreferenceValue_CliVariable(const LName InName, LString&& InDisplayName, const LCliTypeHandle InType)
        : LPreferenceValue(InName, std::move(InDisplayName), LBuildPreference::CreateWeakFunction(LPreferenceValue_CliVariable::BuildDefault)), Type(InType) { }
    LPreferenceValue_CliVariable(const LName InName, const LString& InDisplayName, LBuildPreference&& InBuildDelegate, const LCliTypeHandle InType)
        : LPreferenceValue(InName, InDisplayName, std::move(InBuildDelegate)), Type(InType) { }
    LPreferenceValue_CliVariable(const LName InName, LString&& InDisplayName, LBuildPreference&& InBuildDelegate, const LCliTypeHandle InType)
        : LPreferenceValue(InName, std::move(InDisplayName), std::move(InBuildDelegate)), Type(InType) { }

    ENGINE_API virtual void StoreInitial() override;
    ENGINE_API virtual void ResetToDefault() override;
    ENGINE_API virtual void ResetToInitial() override;

    ENGINE_API static void BuildDefault(const LPreference* Self, WParentBase* Target);

private:

    LCliVariableHandle Type;
};

} /* ~Namespace Jafg */
