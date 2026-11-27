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
//# The optional soft bounds are the typical values inside of them (e.g., the norm of a parameter). If both are set,
//# the bar spans them instead and dragging stays inside of them, scaled to their span. Entered values may still go
//# up to the (hard) bounds.
//#
struct LInputRange final
{
    std::optional<f64> Min;
    std::optional<f64> Max;
    std::optional<f64> SoftMin;
    std::optional<f64> SoftMax;

    //# Whether there are both bounds that span a range.
    NODISCARD constexpr bool IsBounded() const noexcept { return this->Min && this->Max && *this->Max > *this->Min; }
    //# Whether there are both soft bounds that span a range.
    NODISCARD constexpr bool IsSoftBounded() const noexcept { return this->SoftMin && this->SoftMax && *this->SoftMax > *this->SoftMin; }

    //# How much a value changes per pixel of dragging: The acceleration, scaled by the soft span if soft bounded.
    NODISCARD constexpr f64 GetDragScale(f64 Acceleration) const noexcept
    {
        return this->IsSoftBounded() ? Acceleration * (*this->SoftMax - *this->SoftMin) : Acceleration;
    }

    //#
    //# @return The value that was dragged from Start, clamped into the bounds. Dragging stays inside of the soft
    //#         bounds, but a value outside of them is never moved further away.
    //#
    template<typename T> requires std::is_arithmetic_v<T>
    NODISCARD T ClampDragged(T Start, T Value) const noexcept
    {
        if (this->IsSoftBounded())
        {
            f64 const Lower{std::min(*this->SoftMin, static_cast<f64>(Start))};
            f64 const Upper{std::max(*this->SoftMax, static_cast<f64>(Start))};
            f64 const AsFloat{static_cast<f64>(Value)};
            if (AsFloat < Lower)
            {
                return this->Clamp(Saturate<T>(std::is_floating_point_v<T> ? Lower : std::ceil(Lower)));
            }
            if (AsFloat > Upper)
            {
                return this->Clamp(Saturate<T>(std::is_floating_point_v<T> ? Upper : std::floor(Upper)));
            }
        }
        return this->Clamp(Value);
    }

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

    //# @return Where the value is between the soft bounds, else the bounds, in [0,1]. Empty if neither are bounded.
    NODISCARD std::optional<f32> GetFraction(f64 Value) const noexcept
    {
        if (this->IsSoftBounded())
        {
            return static_cast<f32>(std::clamp((Value - *this->SoftMin) / (*this->SoftMax - *this->SoftMin), 0.0, 1.0));
        }
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
