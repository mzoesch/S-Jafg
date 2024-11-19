// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "User/Input/InputActionCategory.h"

namespace Jafg
{

/**
 * Value returned by a triggered input action to the bound delegate functor.
 */
struct LInputActionValue final
{
    using LReal = float;

    /**
     * The threshold for an input action to be considered triggered.
     * TODO: We should make this configurable by the user.
     */
    LReal ThresholdForInputActionValueTrigger = static_cast<LReal>(JAFG_DOUBLE_SMALL_NUMBER);

    using Axis0D = bool;
    using Axis1D = LReal;
    using Axis2D = LVector2;
    using Axis3D = LVector3;

    LInputActionValue() = default;
    LInputActionValue(const EInputActionCategory::Type InValueType)
        : ValueType(InValueType)
    {
        check( this->ValueType != EInputActionCategory::None )
    }
    LInputActionValue(const Axis3D& InValue, const EInputActionCategory::Type InValueType)
        : Value(InValue), ValueType(InValueType)
    {
        check( this->ValueType != EInputActionCategory::None )
    }
    DEFAULT_REALLOC_OF_ANY_FORM(LInputActionValue)
    ~LInputActionValue() = default;

    FORCEINLINE auto IsNonZero() const -> bool;
    FORCEINLINE auto GetMagnitude() const -> LReal;
    FORCEINLINE auto GetSquaredMagnitude() const -> LReal;

    FORCEINLINE auto GetBooleanValue() const -> bool { return this->IsNonZero(); }
    FORCEINLINE auto GetAxis1DValue() const -> Axis1D { return this->Value.X; }
    FORCEINLINE auto GetAxis2DValue() const -> Axis2D { return this->Value.XY(); }
    FORCEINLINE auto GetRawValue() const -> Axis3D { return this->Value; }
    FORCEINLINE auto GetValueType() const -> EInputActionCategory::Type { return this->ValueType; }

    template <typename T>
    FORCEINLINE T Get() const { static_assert(sizeof(T) == 0, "Unsupported value type for input action value!"); return T(); }

    FORCEINLINE auto operator+=(const Axis0D  InValue) -> LInputActionValue& { this->Value.X += (InValue ? static_cast<LReal>(1.0) : static_cast<LReal>(0.0)); return *this; }
    FORCEINLINE auto operator-=(const Axis0D  InValue) -> LInputActionValue& { this->Value.X -= (InValue ? static_cast<LReal>(1.0) : static_cast<LReal>(0.0)); return *this; }
    FORCEINLINE auto operator+=(const Axis1D  InValue) -> LInputActionValue& { this->Value.X += InValue; return *this; }
    FORCEINLINE auto operator-=(const Axis1D  InValue) -> LInputActionValue& { this->Value.X -= InValue; return *this; }
    FORCEINLINE auto operator+=(const Axis2D& InValue) -> LInputActionValue& { this->Value   += InValue; return *this; }
    FORCEINLINE auto operator-=(const Axis2D& InValue) -> LInputActionValue& { this->Value   -= InValue; return *this; }
    FORCEINLINE auto operator+=(const Axis3D& InValue) -> LInputActionValue& { this->Value   += InValue; return *this; }
    FORCEINLINE auto operator-=(const Axis3D& InValue) -> LInputActionValue& { this->Value   -= InValue; return *this; }

    FORCEINLINE LSimpleString ToString() const;

private:

    Axis3D                     Value     = Axis3D::Zero();
    EInputActionCategory::Type ValueType = EInputActionCategory::None;
};

bool LInputActionValue::IsNonZero() const
{
    return this->Value.SquaredMagnitude() > Maths::Squared(
        LInputActionValue::ThresholdForInputActionValueTrigger
    );
}

LInputActionValue::LReal LInputActionValue::GetMagnitude() const
{
    return this->Value.Magnitude();
}

LInputActionValue::LReal LInputActionValue::GetSquaredMagnitude() const
{
    return this->Value.SquaredMagnitude();
}

template<>
inline LInputActionValue::Axis0D LInputActionValue::Get<LInputActionValue::Axis0D>() const
{
    check( this->ValueType == EInputActionCategory::Boolean )
    return this->IsNonZero();
}

template<>
inline LInputActionValue::Axis1D LInputActionValue::Get<LInputActionValue::Axis1D>() const
{
    check( this->ValueType == EInputActionCategory::Axis1D )
    return this->Value.X;
}

template<>
inline LInputActionValue::Axis2D LInputActionValue::Get<LInputActionValue::Axis2D>() const
{
    check( this->ValueType == EInputActionCategory::Axis2D )
    return Axis2D(this->Value.X, this->Value.Y);
}

template<>
inline LInputActionValue::Axis3D LInputActionValue::Get<LInputActionValue::Axis3D>() const
{
    check( this->ValueType == EInputActionCategory::Axis3D )
    return this->Value;
}

LSimpleString LInputActionValue::ToString() const
{
    check( this->ValueType != EInputActionCategory::None )

    if (this->ValueType == EInputActionCategory::Boolean)
    {
        return this->IsNonZero() ? "true" : "false";
    }
    if (this->ValueType == EInputActionCategory::Axis1D)
    {
        return LSimpleString::SprintF("{:.3f}", this->Value.X);
    }
    if (this->ValueType == EInputActionCategory::Axis2D)
    {
        return LSimpleString::SprintF("{:.3f},{:.3f}", this->Value.X, this->Value.Y);
    }

    checkNoEntry()
    return { };
}

} /* ~Namespace Jafg */
