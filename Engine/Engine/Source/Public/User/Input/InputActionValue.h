// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "User/Input/InputActionCategory.h"

namespace Jafg
{

//#
//# Value returned by a triggered input action to the bound delegate functor.
//#
struct LInputActionValue final
{
    using LReal = f32;

    //#
    //# The threshold for an input action to be considered triggered.
    //# TODO: We should make this configurable by the user.
    //#
    LReal ThresholdForInputActionValueTrigger = static_cast<LReal>(LAL_DOUBLE_SMALL_NUMBER);

    using Axis0D = bool;
    using Axis1D = LReal;
    using Axis2D = TVector2<LReal>;
    using Axis3D = TVector3<LReal>;

    FORCEINLINE LInputActionValue() noexcept : Value(Axis3D::ZeroVector), ValueType(EInputActionCategory::None) { }
    FORCEINLINE LInputActionValue(const EInputActionCategory::Type InValueType) noexcept
        : Value(Axis3D::ZeroVector), ValueType(InValueType)
    {
        check( this->ValueType != EInputActionCategory::None )
    }

    FORCEINLINE LInputActionValue(const Axis0D& InValue, const EInputActionCategory::Type InValueType = EInputActionCategory::Boolean) noexcept
        : Value(InValue ? static_cast<LReal>(1.0) : static_cast<LReal>(2.0)), ValueType(InValueType)
    {
        check( this->ValueType != EInputActionCategory::None )
    }

    FORCEINLINE LInputActionValue(const Axis1D& InValue, const EInputActionCategory::Type InValueType = EInputActionCategory::Axis1D) noexcept
        : Value(InValue), ValueType(InValueType)
        {
        check( this->ValueType != EInputActionCategory::None )
    }

    FORCEINLINE LInputActionValue(const Axis2D& InValue, const EInputActionCategory::Type InValueType = EInputActionCategory::Axis2D) noexcept
        : Value({InValue.X, InValue.Y, static_cast<LReal>(0.0)}), ValueType(InValueType)
    {
        check( this->ValueType != EInputActionCategory::None )
    }

    FORCEINLINE LInputActionValue(const Axis3D& InValue, const EInputActionCategory::Type InValueType = EInputActionCategory::Axis3D) noexcept
        : Value(InValue), ValueType(InValueType)
    {
        check( this->ValueType != EInputActionCategory::None )
    }

    DEFAULT_REALLOC_OF_ANY_FORM(LInputActionValue)

    ~LInputActionValue() = default;

    FORCEINLINE bool  IsNonZero() const { return this->Value.SquaredMagnitude() > Maths::Squared(LInputActionValue::ThresholdForInputActionValueTrigger); }
    FORCEINLINE LReal GetMagnitude() const { return this->Value.Magnitude(); }
    FORCEINLINE LReal GetSquaredMagnitude() const { return this->Value.SquaredMagnitude(); }

    FORCEINLINE Axis0D GetBooleanValue() const { return this->IsNonZero(); }
    FORCEINLINE Axis1D GetAxis1DValue() const { return this->Value.X; }
    FORCEINLINE Axis2D GetAxis2DValue() const { return this->Value.XY(); }
    FORCEINLINE Axis3D GetRawValue() const { return this->Value; }
    FORCEINLINE EInputActionCategory::Type GetValueType() const { return this->ValueType; }

    template <typename T>
    FORCEINLINE T Get() const noexcept { static_assert(sizeof(T) == 0, "Unsupported value type for input action value!"); return T(); }

    FORCEINLINE LInputActionValue& operator+=(const Axis0D  InValue) noexcept { this->Value.X += (InValue ? static_cast<LReal>(1.0) : static_cast<LReal>(0.0)); return *this; }
    FORCEINLINE LInputActionValue& operator-=(const Axis0D  InValue) noexcept { this->Value.X -= (InValue ? static_cast<LReal>(1.0) : static_cast<LReal>(0.0)); return *this; }
    FORCEINLINE LInputActionValue& operator+=(const Axis1D  InValue) noexcept { this->Value.X += InValue; return *this; }
    FORCEINLINE LInputActionValue& operator-=(const Axis1D  InValue) noexcept { this->Value.X -= InValue; return *this; }
    FORCEINLINE LInputActionValue& operator+=(const Axis2D& InValue) noexcept { this->Value   += InValue; return *this; }
    FORCEINLINE LInputActionValue& operator-=(const Axis2D& InValue) noexcept { this->Value   -= InValue; return *this; }
    FORCEINLINE LInputActionValue& operator+=(const Axis3D& InValue) noexcept { this->Value   += InValue; return *this; }
    FORCEINLINE LInputActionValue& operator-=(const Axis3D& InValue) noexcept { this->Value   -= InValue; return *this; }

    FORCEINLINE LString ToString() const;

private:

    Axis3D Value;
    EInputActionCategory::Type ValueType;
};

template<>
FORCEINLINE LInputActionValue::Axis0D LInputActionValue::Get<LInputActionValue::Axis0D>() const noexcept
{
    check( this->ValueType == EInputActionCategory::Boolean )
    return this->IsNonZero();
}

template<>
FORCEINLINE LInputActionValue::Axis1D LInputActionValue::Get<LInputActionValue::Axis1D>() const noexcept
{
    check( this->ValueType == EInputActionCategory::Axis1D )
    return this->Value.X;
}

template<>
FORCEINLINE LInputActionValue::Axis2D LInputActionValue::Get<LInputActionValue::Axis2D>() const noexcept
{
    check( this->ValueType == EInputActionCategory::Axis2D )
    return Axis2D(this->Value.X, this->Value.Y);
}

template<>
FORCEINLINE LInputActionValue::Axis3D LInputActionValue::Get<LInputActionValue::Axis3D>() const noexcept
{
    check( this->ValueType == EInputActionCategory::Axis3D )
    return this->Value;
}

FORCEINLINE LString LInputActionValue::ToString() const
{
    check( this->ValueType != EInputActionCategory::None )

    if (this->ValueType == EInputActionCategory::Boolean)
    {
        return this->IsNonZero() ? "true" : "false";
    }
    if (this->ValueType == EInputActionCategory::Axis1D)
    {
        return LString::SprintF("{:.3f}", this->Value.X);
    }
    if (this->ValueType == EInputActionCategory::Axis2D)
    {
        return LString::SprintF("{:.3f},{:.3f}", this->Value.X, this->Value.Y);
    }

    checkNoEntry()
    return { };
}

} /* ~Namespace Jafg */
