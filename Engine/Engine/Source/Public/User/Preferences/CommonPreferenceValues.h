// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Preferences/PreferenceValue.h"

namespace Jafg
{

class LPreferenceValue_Scalar;
class LPreferenceValue_Color;
class LPreferenceValue_Key;

typedef TFunction<LString(const double Value)> LPreferenceScalarFormatFunction;
typedef TFunction<double(void)>                LPreferenceScalarValueGetter;
typedef TFunction<void(const double Value)>    LPreferenceScalarValueSetter;

class LPreferenceValue_Scalar : public LPreferenceValue
{
public:

    LPreferenceValue_Scalar(const LName InName, const LString& InDisplayName) : LPreferenceValue(InName, InDisplayName, LBuildPreference::CreateStrongDelegate(LPreferenceValue_Scalar::BuildDefault)) { }
    LPreferenceValue_Scalar(const LName InName, LString&& InDisplayName) : LPreferenceValue(InName, std::move(InDisplayName), LBuildPreference::CreateStrongDelegate(LPreferenceValue_Scalar::BuildDefault)) { }
    LPreferenceValue_Scalar(const LName InName, const LString& InDisplayName, LBuildPreference&& InBuildDelegate) : LPreferenceValue(InName, InDisplayName, std::move(InBuildDelegate)) { }
    LPreferenceValue_Scalar(const LName InName, LString&& InDisplayName, LBuildPreference&& InBuildDelegate) : LPreferenceValue(InName, std::move(InDisplayName), std::move(InBuildDelegate)) { }

    ENGINE_API virtual void StoreInitial() override;
    ENGINE_API virtual void ResetToDefault() override;
    ENGINE_API virtual void ResetToInitial() override;

    FORCEINLINE auto GetInitialF() const -> float { return static_cast<float>(this->Initial); }
    FORCEINLINE auto GetInitial() const -> double { return this->Initial; }

    FORCEINLINE bool IsDefaultValueValid() const { return this->Default.IsSet(); }
    FORCEINLINE void SetDefaultValue(const float  InValue) { this->SetDefaultValue(static_cast<double>(InValue)); }
    FORCEINLINE void SetDefaultValue(const double InValue) { this->Default = InValue; }
    FORCEINLINE auto GetDefaultValueF() const -> TOptional<float>;
    FORCEINLINE auto GetDefaultValue() const -> TOptional<double> { return this->Default; }
    FORCEINLINE void SetMinimum(const float  InValue) { this->SetMinimum(static_cast<double>(InValue)); }

    FORCEINLINE void SetMinimum(const double InValue) { this->Minimum = InValue; }
    FORCEINLINE bool IsMinimumValid() const { return this->Minimum.IsSet(); }
    FORCEINLINE auto GetMinimumF() const -> TOptional<float>;
    FORCEINLINE auto GetMinimum() const -> TOptional<double> { return this->Minimum; }
    FORCEINLINE void SetMaximum(const float  InValue) { this->SetMaximum(static_cast<double>(InValue)); }
    FORCEINLINE void SetMaximum(const double InValue) { this->Maximum = InValue; }
    FORCEINLINE bool IsMaximumValid() const { return this->Maximum.IsSet(); }
    FORCEINLINE auto GetMaximumF() const -> TOptional<float>;
    FORCEINLINE auto GetMaximum() const -> TOptional<double> { return this->Maximum; }

    ENGINE_API static LString Fmt_Raw(const double Value);
    ENGINE_API static LString Fmt_ZeroToOneAsPercent(const double Value);
    FORCEINLINE void SetDisplayFormat(LPreferenceScalarFormatFunction&& InFnFormat) { this->FnFormat = std::move(InFnFormat); }
    FORCEINLINE bool IsDisplayFormatValid() const { return this->FnFormat.IsBound(); }
    FORCEINLINE auto GetFormattedText() const -> LString { return this->FnFormat(this->GetValue()); }

    FORCEINLINE auto GetValueF() const -> float { return static_cast<float>(this->Getter()); }
    FORCEINLINE auto GetValue() const -> double { return this->Getter(); }
    FORCEINLINE void SetValue(const float  InValue) { this->SetValue(static_cast<double>(InValue)); }
    FORCEINLINE void SetValue(const double InValue) { this->Setter(InValue); }
    FORCEINLINE void SetValueGetter(LPreferenceScalarValueGetter&& InGetter) { this->Getter = std::move(InGetter); }
    FORCEINLINE bool IsValueGetterValid() const { return this->Getter.IsBound(); }
    FORCEINLINE void SetValueSetter(LPreferenceScalarValueSetter&& InSetter) { this->Setter = std::move(InSetter); }
    FORCEINLINE bool IsValueSetterValid() const { return this->Setter.IsBound(); }

    ENGINE_API static void BuildDefault(const LPreference* Self, WParentBase* Target);

protected:

    f64 Initial = 0.0;
    f64 Step    = 0.01;
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

class LPreferenceValue_Key : public LPreferenceValue
{
};

} /* ~Namespace Jafg */

Jafg::TOptional<f32> Jafg::LPreferenceValue_Scalar::GetDefaultValueF() const
{
    if (this->Default.IsSet())
    {
        return static_cast<f32>(this->Default.GetValue());
    }

    return { };
}

Jafg::TOptional<f32> Jafg::LPreferenceValue_Scalar::GetMinimumF() const
{
    if (this->Minimum.IsSet())
    {
        return static_cast<f32>(this->Minimum.GetValue());
    }

    return { };
}

Jafg::TOptional<f32> Jafg::LPreferenceValue_Scalar::GetMaximumF() const
{
    if (this->Maximum.IsSet())
    {
        return static_cast<f32>(this->Maximum.GetValue());
    }

    return { };
}
