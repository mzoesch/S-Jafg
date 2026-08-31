// Copyright mzoesch. All rights reserved.

#pragma once

#include "Cli/CliObject.h"
#include "Cli/CliType.h"

namespace Jafg
{

typedef TFunction<void(const LString& InValue)> LOnVariableChangedDelegate;

//# A variable inside the cli of the engine.
class LCliVariable final : public LCliObject
{
public:

    FORCEINLINE LCliVariable() = delete;
    //# If no default value is provided, the default value of the provided type will be used.
    ENGINE_API  LCliVariable(const LString& InIdentifier, LCliType&& InType);
    ENGINE_API  LCliVariable(const LString& InIdentifier, LCliType&& InType, const LString& InDefault);
    ENGINE_API  LCliVariable(const LString& InIdentifier, LCliType&& InType, const LString& InDefault, LOnVariableChangedDelegate&& InDelegate);
    ENGINE_API  LCliVariable(const LString& InIdentifier, LCliType&& InType, LOnVariableChangedDelegate&& InDelegate);
    ENGINE_API  LCliVariable(const LString& InIdentifier, const LString& InHelp, LCliType&& InType);
    ENGINE_API  LCliVariable(const LString& InIdentifier, const LString& InHelp, LCliType&& InType, const LString& InDefault);
    ENGINE_API  LCliVariable(const LString& InIdentifier, const LString& InHelp, LCliType&& InType, const LString& InDefault, LOnVariableChangedDelegate&& InDelegate);
    ENGINE_API  LCliVariable(const LString& InIdentifier, const LString& InHelp, LCliType&& InType, LOnVariableChangedDelegate&& InDelegate);
    PROHIBIT_COPY(LCliVariable)
    FORCEINLINE LCliVariable(LCliVariable&& InOther) noexcept
    : Type(std::move(InOther.Type)), Value(std::move(InOther.Value)), OnVariableChangedDelegate(std::move(InOther.OnVariableChangedDelegate))
    {
        this->LCliObject::operator=(std::move(InOther));
        check( InOther.GetUuid() == LCliObject::NoUuid && InOther.GetIdentifier().empty() && InOther.GetHelp().empty() )
        check( InOther.Type.GetDefault().empty() && InOther.Type.IsTypeDelegateValid() == false && InOther.Type.IsValueSetDelegateValid() == false )
        check( InOther.Value.empty() && InOther.OnVariableChangedDelegate.IsValid() == false )

        return;
    }
    FORCEINLINE LCliVariable& operator=(LCliVariable&& InOther) noexcept
    {
        this->Type = std::move(InOther.Type);
        this->Value = std::move(InOther.Value);
        this->OnVariableChangedDelegate = std::move(InOther.OnVariableChangedDelegate);

        this->LCliObject::operator=(std::move(InOther));
        check( InOther.GetUuid() == LCliObject::NoUuid && InOther.GetIdentifier().empty() && InOther.GetHelp().empty() )
        check( InOther.Type.GetDefault().empty() && InOther.Type.IsTypeDelegateValid() == false && InOther.Type.IsValueSetDelegateValid() == false )
        check( InOther.Value.empty() && InOther.OnVariableChangedDelegate.IsValid() == false )

        return *this;
    }

    ENGINE_API  auto GetType() const -> const LCliType*;
    ENGINE_API  bool SetValue(const LString& InValue);
    FORCEINLINE auto GetValue() const -> const LString& { return this->Value; }

    template<typename TField>
    FORCEINLINE void GetValue(TField* Destination) const { serde::from_string(Destination, this->Value); }
    template<typename TField> requires std::is_default_constructible_v<TField>
        && (std::is_move_constructible_v<TField> || std::is_move_assignable_v<TField>)
    FORCEINLINE TField GetValue() const
    {
        TField OutValue;
        this->GetValue(&OutValue);
        return OutValue;
    }

private:

    LCliType Type;
    LString Value;
    LOnVariableChangedDelegate OnVariableChangedDelegate;
};

} /* ~Namespace Jafg */
