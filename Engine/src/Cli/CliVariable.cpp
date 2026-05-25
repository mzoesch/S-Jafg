// Copyright mzoesch. All rights reserved.

#include "Cli/CliVariable.h"
#include "Engine/Engine.h"
#include "Cli/CommandLineInterface.h"

const Jafg::LCliType* Jafg::LCliVariable::GetType() const
{
    check( GEngine )
    return GEngine->GetCommandLineInterface().GetTypeAsserted(this->Type);
}

bool Jafg::LCliVariable::SetValue(const LString& InValue)
{
    if (this->Value == InValue)
    {
        return false;
    }

    check( GEngine )
    const LCliType* EngineType = GEngine->GetCommandLineInterface().GetTypeAsserted(this->Type);
    const bool bOut = EngineType->SetVariable(InValue, &this->Value);
    if (bOut && this->OnVariableChangedDelegate.IsValid())
    {
        this->OnVariableChangedDelegate(this->Value);
    }

    return bOut;
}

Jafg::LCliVariable::LCliVariable(const LString& InIdentifier, LCliType&& InType)
    : LCliObject(InIdentifier), Type(std::move(InType))
{
    check( GEngine )
    const LCliType* EngineType = GEngine->GetCommandLineInterface().GetTypeAsserted(this->Type);
    this->Value = EngineType->GetDefault();

    return;
}

Jafg::LCliVariable::LCliVariable(const LString& InIdentifier, LCliType&& InType, const LString& InDefault)
    : LCliObject(InIdentifier), Type(std::move(InType)), Value(InDefault)
{
    return;
}

Jafg::LCliVariable::LCliVariable(const LString& InIdentifier, LCliType&& InType, const LString& InDefault, LOnVariableChangedDelegate&& InDelegate)
    : LCliObject(InIdentifier), Type(std::move(InType)), Value(InDefault), OnVariableChangedDelegate(std::move(InDelegate))
{
    return;
}

Jafg::LCliVariable::LCliVariable(const LString& InIdentifier, LCliType&& InType, LOnVariableChangedDelegate&& InDelegate)
    : LCliObject(InIdentifier), Type(std::move(InType)), OnVariableChangedDelegate(std::move(InDelegate))
{
    check( GEngine )
    const LCliType* EngineType = GEngine->GetCommandLineInterface().GetTypeAsserted(this->Type);
    this->Value = EngineType->GetDefault();

    return;
}

Jafg::LCliVariable::LCliVariable(const LString& InIdentifier, const LString& InHelp, LCliType&& InType)
    : LCliObject(InIdentifier, InHelp), Type(std::move(InType))
{
    check( GEngine )
    const LCliType* EngineType = GEngine->GetCommandLineInterface().GetTypeAsserted(this->Type);
    this->Value = EngineType->GetDefault();

    return;
}

Jafg::LCliVariable::LCliVariable(const LString& InIdentifier, const LString& InHelp, LCliType&& InType, const LString& InDefault)
    : LCliObject(InIdentifier, InHelp), Type(std::move(InType)), Value(InDefault)
{
    return;
}

Jafg::LCliVariable::LCliVariable(const LString& InIdentifier, const LString& InHelp, LCliType&& InType, const LString& InDefault, LOnVariableChangedDelegate&& InDelegate)
    : LCliObject(InIdentifier, InHelp), Type(std::move(InType)), Value(InDefault), OnVariableChangedDelegate(std::move(InDelegate))
{
    return;
}

Jafg::LCliVariable::LCliVariable(const LString& InIdentifier, const LString& InHelp, LCliType&& InType, LOnVariableChangedDelegate&& InDelegate)
    : LCliObject(InIdentifier, InHelp), Type(std::move(InType)), OnVariableChangedDelegate(std::move(InDelegate))
{
    check( GEngine )
    const LCliType* EngineType = GEngine->GetCommandLineInterface().GetTypeAsserted(this->Type);
    this->Value = EngineType->GetDefault();

    return;
}
