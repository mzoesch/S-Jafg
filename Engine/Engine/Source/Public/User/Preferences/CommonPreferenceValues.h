// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Preferences/PreferenceValue.h"

namespace Jafg
{

class LPreferenceValue_Scalar;
class LPreferenceValue_Color;
class LPreferenceValue_Key;

typedef TFunction<LString(const f64 Value)> LPreferenceScalarFormatFunction;
typedef TFunction<f64(void)>                LPreferenceScalarValueGetter;
typedef TFunction<void(const f64 Value)>    LPreferenceScalarValueSetter;

class LPreferenceValue_Scalar : public LPreferenceValue
{
public:

    LPreferenceValue_Scalar(const LName InName, const LString& InDisplayName)
        : LPreferenceValue(InName, InDisplayName, LBuildPreference::CreateWeakDelegate(LPreferenceValue_Scalar::BuildDefault)) { }
    LPreferenceValue_Scalar(const LName InName, LString&& InDisplayName)
        : LPreferenceValue(InName, std::move(InDisplayName), LBuildPreference::CreateWeakDelegate(LPreferenceValue_Scalar::BuildDefault)) { }
    LPreferenceValue_Scalar(const LName InName, const LString& InDisplayName, LBuildPreference&& InBuildDelegate)
        : LPreferenceValue(InName, InDisplayName, std::move(InBuildDelegate)) { }
    LPreferenceValue_Scalar(const LName InName, LString&& InDisplayName, LBuildPreference&& InBuildDelegate)
        : LPreferenceValue(InName, std::move(InDisplayName), std::move(InBuildDelegate)) { }

    ENGINE_API virtual void StoreInitial() override;
    ENGINE_API virtual void ResetToDefault() override;
    ENGINE_API virtual void ResetToInitial() override;

    FORCEINLINE auto GetInitialF() const -> f32 { return static_cast<f32>(this->Initial); }
    FORCEINLINE auto GetInitial() const -> f64 { return this->Initial; }

    FORCEINLINE bool IsDefaultValueValid() const { return this->Default.has_value(); }
    FORCEINLINE void SetDefaultValue(const f32  InValue) { this->SetDefaultValue(static_cast<f64>(InValue)); }
    FORCEINLINE void SetDefaultValue(const f64 InValue) { this->Default = InValue; }
    FORCEINLINE auto GetDefaultValueF() const -> TOptional<f32>;
    FORCEINLINE auto GetDefaultValue() const -> TOptional<f64> { return this->Default; }
    FORCEINLINE void SetMinimum(const f32  InValue) { this->SetMinimum(static_cast<f64>(InValue)); }

    FORCEINLINE void SetMinimum(const f64 InValue) { this->Minimum = InValue; }
    FORCEINLINE bool IsMinimumValid() const { return this->Minimum.has_value(); }
    FORCEINLINE auto GetMinimumF() const -> TOptional<f32>;
    FORCEINLINE auto GetMinimum() const -> TOptional<f64> { return this->Minimum; }
    FORCEINLINE void SetMaximum(const f32  InValue) { this->SetMaximum(static_cast<f64>(InValue)); }
    FORCEINLINE void SetMaximum(const f64 InValue) { this->Maximum = InValue; }
    FORCEINLINE bool IsMaximumValid() const { return this->Maximum.has_value(); }
    FORCEINLINE auto GetMaximumF() const -> TOptional<f32>;
    FORCEINLINE auto GetMaximum() const -> TOptional<f64> { return this->Maximum; }

    ENGINE_API static LString Fmt_Raw(const f64 Value);
    ENGINE_API static LString Fmt_ZeroToOneAsPercent(const f64 Value);
    FORCEINLINE void SetDisplayFormat(LPreferenceScalarFormatFunction&& InFnFormat) { this->FnFormat = std::move(InFnFormat); }
    FORCEINLINE bool IsDisplayFormatValid() const { return this->FnFormat.IsValid(); }
    FORCEINLINE auto GetFormattedText() const -> LString { return this->FnFormat(this->GetValue()); }

    FORCEINLINE auto GetValueF() const -> f32 { return static_cast<f32>(this->Getter()); }
    FORCEINLINE auto GetValue() const -> f64 { return this->Getter(); }
    FORCEINLINE void SetValue(const f32  InValue) { this->SetValue(static_cast<f64>(InValue)); }
    FORCEINLINE void SetValue(const f64 InValue) { this->Setter(InValue); }
    FORCEINLINE void SetValueGetter(LPreferenceScalarValueGetter&& InGetter) { this->Getter = std::move(InGetter); }
    FORCEINLINE bool IsValueGetterValid() const { return this->Getter.IsValid(); }
    FORCEINLINE void SetValueSetter(LPreferenceScalarValueSetter&& InSetter) { this->Setter = std::move(InSetter); }
    FORCEINLINE bool IsValueSetterValid() const { return this->Setter.IsValid(); }

    ENGINE_API static void BuildDefault(const LPreference* Self, WParentBase* Target);

protected:

    f64 Initial {  0.0 };
    f64 Step    { 0.01 };
    TOptional<f64> Default;
    TOptional<f64> Minimum;
    TOptional<f64> Maximum;
    LPreferenceScalarFormatFunction FnFormat;
    LPreferenceScalarValueGetter Getter;
    LPreferenceScalarValueSetter Setter;
};

class LPreferenceValue_Color : public LPreferenceValue
{
};

class LPreferenceValue_InputAction : public LPreferenceValue
{
public:

    FORCEINLINE LPreferenceValue_InputAction(LName InName)
        : LPreferenceValue(std::move(InName), LBuildPreference::CreateWeakFunction(LPreferenceValue_InputAction::BuildDefault)) { }
    FORCEINLINE LPreferenceValue_InputAction(LName InName, LBuildPreference&& InBuildDelegate)
        : LPreferenceValue(std::move(InName), std::move(InBuildDelegate)) { }

    FORCEINLINE LPreferenceValue_InputAction(LName InName, const LString& InDisplayName)
        : LPreferenceValue(std::move(InName), InDisplayName, LBuildPreference::CreateWeakFunction(LPreferenceValue_InputAction::BuildDefault)) { }
    FORCEINLINE LPreferenceValue_InputAction(LName InName, const LString& InDisplayName, LBuildPreference&& InBuildDelegate)
        : LPreferenceValue(std::move(InName), InDisplayName, std::move(InBuildDelegate)) { }

    FORCEINLINE LPreferenceValue_InputAction(LName InName, LString&& InDisplayName)
        : LPreferenceValue(std::move(InName), std::move(InDisplayName), LBuildPreference::CreateWeakFunction(LPreferenceValue_InputAction::BuildDefault)) { }
    FORCEINLINE LPreferenceValue_InputAction(LName InName, LString&& InDisplayName, LBuildPreference&& InBuildDelegate)
        : LPreferenceValue(std::move(InName), std::move(InDisplayName), std::move(InBuildDelegate)) { }

    ENGINE_API virtual void StoreInitial() override;
    ENGINE_API virtual void ResetToDefault() override;
    ENGINE_API virtual void ResetToInitial() override;

    ENGINE_API static void BuildDefault(const LPreference* Self, WParentBase* Target);
};

} /* ~Namespace Jafg */

TOptional<f32> Jafg::LPreferenceValue_Scalar::GetDefaultValueF() const
{
    if (this->Default.has_value())
    {
        return static_cast<f32>(this->Default.value());
    }

    return { };
}

TOptional<f32> Jafg::LPreferenceValue_Scalar::GetMinimumF() const
{
    if (this->Minimum.has_value())
    {
        return static_cast<f32>(this->Minimum.value());
    }

    return { };
}

TOptional<f32> Jafg::LPreferenceValue_Scalar::GetMaximumF() const
{
    if (this->Maximum.has_value())
    {
        return static_cast<f32>(this->Maximum.value());
    }

    return { };
}
