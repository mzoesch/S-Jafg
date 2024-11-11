// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

namespace EInputActionValueType
{

enum Type : uint8
{
    None    = 0x0 << 0,
    Boolean = 0x1 << 0,
    Axis1D  = 0x1 << 1,
    Axis2D  = 0x1 << 2,
};

} /* ~Namespace EInputActionValueType */

ENUM_CLASS_FLAGS(EInputActionValueType::Type)

/**
 * Value returned by a triggered input action to the bound delegate functor.
 */
struct LUserInputActionValue final
{
    using LReal = float;

    /**
     * The threshold for an input action to be considered triggered.
     * TODO: We should make this configurable by the user.
     */
    LReal ThresholdForInputActionValueTrigger = static_cast<LReal>(0.1);

    using Axis1D = LReal;
    using Axis2D = LVector2;

    LUserInputActionValue() = default;
    DEFAULT_REALLOC_OF_ANY_FROM(LUserInputActionValue)
    ~LUserInputActionValue() = default;

    FORCEINLINE auto IsNonZero() const -> bool;
    FORCEINLINE auto GetMagnitude() const -> LReal;
    FORCEINLINE auto GetSquaredMagnitude() const -> LReal;

    FORCEINLINE auto GetBooleanValue() const -> bool { return this->IsNonZero(); }
    FORCEINLINE auto GetAxis1DValue() const -> Axis1D { return this->Value.X; }
    FORCEINLINE auto GetAxis2DValue() const -> Axis2D { return this->Value; }
    FORCEINLINE auto GetRawValue() const -> LVector2 { return this->Value; }
    FORCEINLINE auto GetValueType() const -> EInputActionValueType::Type { return this->ValueType; }

    FORCEINLINE LSimpleString ToString() const
    {
        check( this->ValueType != EInputActionValueType::None )

        if (this->ValueType == EInputActionValueType::Boolean)
        {
            return this->IsNonZero() ? "true" : "false";
        }
        if (this->ValueType == EInputActionValueType::Axis1D)
        {
            return LSimpleString::SprintF("{:.3f}", this->Value.X);
        }
        if (this->ValueType == EInputActionValueType::Axis2D)
        {
            return LSimpleString::SprintF("{:.3f},{:.3f}", this->Value.X, this->Value.Y);
        }

        checkNoEntry()
        return { };
    }

private:

    LVector2                    Value     = LVector2::Zero();
    EInputActionValueType::Type ValueType = EInputActionValueType::None;
};

bool LUserInputActionValue::IsNonZero() const
{
    return this->Value.SquaredMagnitude() > Maths::Squared(
        LUserInputActionValue::ThresholdForInputActionValueTrigger
    );
}

LUserInputActionValue::LReal LUserInputActionValue::GetMagnitude() const
{
    return this->Value.Magnitude();
}

LUserInputActionValue::LReal LUserInputActionValue::GetSquaredMagnitude() const
{
    return this->Value.SquaredMagnitude();
}

} /* ~Namespace Jafg */
