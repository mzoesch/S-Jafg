// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/PreferenceCollection.h"
#include "Nodes/Node.h"

namespace Jafg
{

struct LPreferenceValue;
struct LPreferenceValueChangeRequest;

//#
//# The base class for all preferences that are conceptually a value.
//#
//# When a preference user interface menu of any form is opened, there is a set of preferences where (when following
//# the jafg standard should be some specialization of #TPreference) each preference has a default value and a
//# value (the initial value).
//# A user may change preferences but these preferences are not applied until the user confirmed their changes.
//# A user may at any time discard their current preferences.
//#
struct LPreferenceValue: LPreference
{
    typedef TFunction2<LFactoryNode(LViewport& Viewport
        , TArray<LPreferenceValueChangeRequest>& Changes
        , LPreferenceValue& Preference
        )> LCreateEditNode;

    LPreferenceValue(
          LString Identifier
        , LString DisplayName
        , LCreateEditNode CreateEditNode
        )
        : LPreference{EPreference::Value, std::move(Identifier), std::move(DisplayName), {}}
        , CreateEditNode{std::move(CreateEditNode)} {}

    //# Create a node, to allow seeing/editing the preference.
    NODISCARD LFactoryNode CreateNode(LViewport& Viewport, TArray<LPreferenceValueChangeRequest>& Changes)
    {
        check(!!this->CreateEditNode)
        return std::invoke(this->CreateEditNode, Viewport, Changes, *this);
    }

    //# Load the value from persistent storage.
    NODISCARD virtual LString GetValue() const = 0;
    //# Save to value to a persistent storage.
    virtual void SaveValue(LString Value) = 0;

private:

    mutable LCreateEditNode CreateEditNode;
};

struct LPreferenceValueChangeRequest final
{
    TReference<LPreferenceValue> Preference;
    LString Value;
};

//# Provides an easy but not optimal integration of the #LPreferenceValue interface.
struct LCommonPreferenceValue: LPreferenceValue
{
    //# Universal getter/setter functions.
    typedef TFunction2<LString()> LGetter;
    typedef TFunction2<void(LString)> LSetter;

    LCommonPreferenceValue(
          LString Identifier
        , LString DisplayName
        , LGetter Getter
        , LSetter Setter
        , LCreateEditNode CreateEditNode
        ) noexcept
        : LPreferenceValue{std::move(Identifier), std::move(DisplayName), std::move(CreateEditNode)}
        , Getter{std::move(Getter)}
        , Setter{std::move(Setter)} {}

    NODISCARD virtual LString GetValue() const override
    {
        check(!!this->Getter)
        return std::invoke(this->Getter);
    }

    virtual void SaveValue(LString Value) override
    {
        check(!!this->Setter)
        std::invoke(this->Setter, std::move(Value));
    }

private:

    mutable LGetter Getter;
    LSetter Setter;
};

FORCEINLINE constexpr LPreferenceValue& LPreference::AsValue() noexcept
{
    check(this->Type == EPreference::Value)
    return *static_cast<LPreferenceValue*>(this);
}
FORCEINLINE constexpr LPreferenceValue const& LPreference::AsValue() const noexcept
{
    check(this->Type == EPreference::Value)
    return *static_cast<LPreferenceValue const*>(this);
}

struct LPreferenceValue_Scalar;
struct LPreferenceValue_ScalarCreateInfo final
{
    LString Identifier;
    LString DisplayName;

    f64 Step;
    std::optional<f64> Minimum;
    std::optional<f64> Maximum;

    LCommonPreferenceValue::LGetter Getter;
    LCommonPreferenceValue::LSetter Setter;
    TFunction2<LFactoryNode(LViewport& Viewport
        , TArray<LPreferenceValueChangeRequest>& Changes
        , LPreferenceValue_Scalar& Preference
        )> CreateEditNode;
};

//# A preference that stores a scalar.
struct LPreferenceValue_Scalar: LCommonPreferenceValue
{
    typedef f64 value_type;

    LPreferenceValue_Scalar(LPreferenceValue_ScalarCreateInfo Info) noexcept
        : LCommonPreferenceValue{
              std::move(Info.Identifier)
            , std::move(Info.DisplayName)
            , std::move(Info.Getter)
            , std::move(Info.Setter)
            , [CreateEditNode=std::move(Info.CreateEditNode)](LViewport& Viewport
                , TArray<LPreferenceValueChangeRequest>& Changes
                , LPreferenceValue& Preference
                ) mutable
                {
                    return std::invoke(CreateEditNode, Viewport, Changes, static_cast<LPreferenceValue_Scalar&>(Preference));
                }
            }
        , Step{Info.Step}
        , Minimum{Info.Minimum}
        , Maximum{Info.Maximum}
        {}

    NODISCARD FORCEINLINE constexpr f64 GetStep() const noexcept { return this->Step; }
    NODISCARD FORCEINLINE constexpr bool HasMinimum() const noexcept { return this->Minimum.has_value(); }
    NODISCARD FORCEINLINE constexpr f64 GetMinimum() const noexcept { return this->Minimum.value_or(std::numeric_limits<f64>::lowest()); }
    NODISCARD FORCEINLINE constexpr bool HasMaximum() const noexcept { return this->Maximum.has_value(); }
    NODISCARD FORCEINLINE constexpr f64 GetMaximum() const noexcept { return this->Maximum.value_or(std::numeric_limits<f64>::max()); }

private:

    f64 Step{};

    std::optional<value_type> Minimum;
    std::optional<value_type> Maximum;
};

struct LPreferenceValue_List;
struct LPreferenceValue_ListCreateInfo final
{
    LString Identifier;
    LString DisplayName;

    TFunction2<TArray<LString>()> Items;

    LCommonPreferenceValue::LGetter Getter;
    LCommonPreferenceValue::LSetter Setter;
    TFunction2<LFactoryNode(LViewport& Viewport
        , TArray<LPreferenceValueChangeRequest>& Changes
        , LPreferenceValue_List& Preference
        )> CreateEditNode;
};

struct LPreferenceValue_List: LCommonPreferenceValue
{
    typedef LString value_type;

    LPreferenceValue_List(LPreferenceValue_ListCreateInfo Info) noexcept
        : LCommonPreferenceValue{
              std::move(Info.Identifier)
            , std::move(Info.DisplayName)
            , std::move(Info.Getter)
            , std::move(Info.Setter)
            , [CreateEditNode=std::move(Info.CreateEditNode)](LViewport& Viewport
                , TArray<LPreferenceValueChangeRequest>& Changes
                , LPreferenceValue& Preference
                ) mutable
                {
                    return std::invoke(CreateEditNode, Viewport, Changes, static_cast<LPreferenceValue_List&>(Preference));
                }
            }
        , Items{std::move(Info.Items)}
        {}

    NODISCARD FORCEINLINE constexpr TArray<value_type> GetItems() const noexcept
    {
        check(!!this->Items)
        return this->Items();
    }

private:

    mutable TFunction2<TArray<LString>()> Items;
};

} /* ~Namespace Jafg */
