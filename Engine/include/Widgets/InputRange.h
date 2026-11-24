// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

//#
//# The optional bounds of the values of an input (shared by every axis of it). Both are optional and independent.
//# Values that are set or entered afterward are clamped into them. If both are set, every axis shows a bar of where
//# its value is between them (@see WEditableTextButton::FillFraction).
//#
struct LInputRange final
{
    std::optional<f64> Min;
    std::optional<f64> Max;

    //# Whether there are both bounds that span a range.
    NODISCARD constexpr bool IsBounded() const noexcept { return this->Min && this->Max && *this->Max > *this->Min; }

    //# @return The value clamped into the bounds. Integral values are clamped to the closest integral inside of them.
    template<typename T> requires std::is_arithmetic_v<T>
    NODISCARD T Clamp(T Value) const noexcept
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            if (this->Min && Value < *this->Min)
            {
                return static_cast<T>(*this->Min);
            }
            if (this->Max && Value > *this->Max)
            {
                return static_cast<T>(*this->Max);
            }
            return Value;
        }
        else
        {
            f64 const AsFloat{static_cast<f64>(Value)};
            if (this->Min && AsFloat < *this->Min)
            {
                return Saturate<T>(std::ceil(*this->Min));
            }
            if (this->Max && AsFloat > *this->Max)
            {
                return Saturate<T>(std::floor(*this->Max));
            }
            return Value;
        }
    }

    //# @return Where the value is between the bounds in [0,1]. Empty if not bounded.
    NODISCARD std::optional<f32> GetFraction(f64 Value) const noexcept
    {
        if (!this->IsBounded())
        {
            return {};
        }
        return static_cast<f32>(std::clamp((Value - *this->Min) / (*this->Max - *this->Min), 0.0, 1.0));
    }

private:

    //# The integral that is closest to the value.
    template<typename T>
    NODISCARD static T Saturate(f64 Value) noexcept
    {
        if (Value <= static_cast<f64>(std::numeric_limits<T>::lowest()))
        {
            return std::numeric_limits<T>::lowest();
        }
        /* Rounds up to the next power of two for 64 bit types. Everything at or above it does not fit. */
        if (Value >= static_cast<f64>(std::numeric_limits<T>::max()))
        {
            return std::numeric_limits<T>::max();
        }
        return static_cast<T>(Value);
    }
};

} /* ~Namespace Jafg */
