// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/CxxClass.h"
#include "Engine/CxxRecordTearDownReason.h"
#include "Foreign/PluginForward.h"

namespace Jafg
{

class LWorld;
class JCxxClass;

//# Object context used to determine the context and lifetimes of reflected classes.
class LClassOuter
{
public:

    inline LClassOuter() noexcept : HumanReadableName("<anonymous>") { }
    ENGINE_API explicit LClassOuter(LString HumanReadableName, bool bRegisterToEngine = true) noexcept;
    PROHIBIT_REALLOC_OF_ANY_FORM(LClassOuter)
    inline virtual ~LClassOuter() noexcept { this->TearDown(); }

    ENGINE_API void TearDown() noexcept;

    FORCEINLINE constexpr bool IsUserDataValid() const noexcept { return this->UserData != nullptr; }
    FORCEINLINE constexpr void SetUserData(void* InUserData) noexcept { this->UserData = InUserData;     }
    FORCEINLINE constexpr void* GetUserData() noexcept { return this->UserData; }

    FORCEINLINE void SetHumanReadableName(LStringView const& S) noexcept { this->HumanReadableName = S; }
    FORCEINLINE LString const& GetHumanReadableName() const noexcept { return this->HumanReadableName; }

    //# For internal use only. DO NOT USE.
    FORCEINLINE void _AddEmployee(JCxxClass* Employee) noexcept
    {
        check(this->IsHiredHere(Employee) == false)
        this->Employees.emplace_back(Employee);
        return;
    }

    FORCEINLINE bool IsHiredHere(JCxxClass const* Employee) const noexcept { return algo::contains(this->Employees, Employee, &TUnique<JCxxClass>::get); }
    FORCEINLINE TArray<TUnique<JCxxClass>> const& GetEmployees() const noexcept { return this->Employees; }

    //# @return How many employees were hired and killed in this outer.
    LSize KillEmployeesFromForeignPlugin(
          LLoadedPluginHandle PluginHandle
        , ECxxRecordTearDownReason::Type Reason = ECxxRecordTearDownReason::PluginUnload
        );

    virtual bool IsWorld() const noexcept { return false; }
    LWorld& AsWorld() noexcept;
    LWorld const& AsWorld() const noexcept;

    FORCEINLINE TUnique<JCxxClass> Poach(JCxxClass* Employee) noexceptcheck
    {
        check(Employee)
        auto It{algo::find(this->Employees, Employee, algo::unique_raw{})};
        check( It != this->Employees.end() )

        TUnique Out{std::move(*It)};
        check(*It == nullptr)
        check(Out.get() == Employee)
        this->Employees.erase(It);

        return Out;
    }

    FORCEINLINE TUnique<JCxxClass> PoachToNull(JCxxClass* Employee) noexceptcheck
    {
        check( Employee )
        auto It{algo::find(this->Employees, Employee, algo::unique_raw{})};
        check( It != this->Employees.end() )

        TUnique Out{std::move(*It)};
        check(*It == nullptr)
        check(Out.get() == Employee)

        return Out;
    }

protected:

    virtual void OnTearDown() { }

private:

    void RegisterToEngine();
    void UnregisterFromEngine();

    void* UserData{};
    LString HumanReadableName;
    TArray<TUnique<JCxxClass>> Employees;
    bool bWasRegisteredToEngine{};
};

inline JCxxClass::JCxxClass(LCxxDynamicInit const& Init) noexcept
    : JafgVirtualTable{Init.Class}, Outer{Init.Outer}
{
    this->Outer._AddEmployee(this);
}

template<typename TCxxClass>
inline JCxxClass::JCxxClass(TCxxStaticInit<TCxxClass> const& Init) noexcept
    : JafgVirtualTable{TCxxClass::StaticClass()}, Outer{Init.Outer}
{
    this->Outer._AddEmployee(this);
}

} /* ~Namespace Jafg */
