// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

typedef u8 LRotatorAxis;

namespace ERotatorAxis
{

enum Type : LRotatorAxis
{
    None  = 0 << 0,
    Pitch = 1 << 0,
    Yaw   = 1 << 1,
    Roll  = 1 << 2,
};

FORCEINLINE Type operator|(const Type& Lhs, const Type& Rhs)
{
    return static_cast<Type>(static_cast<LRotatorAxis>(Lhs) | static_cast<LRotatorAxis>(Rhs));
}

FORCEINLINE Type& operator|=(Type& Lhs, const Type& Rhs)
{
    Lhs = Lhs | Rhs;
    return Lhs;
}

FORCEINLINE Type operator&(const Type& Lhs, const Type& Rhs)
{
    return static_cast<Type>(static_cast<LRotatorAxis>(Lhs) & static_cast<LRotatorAxis>(Rhs));
}

FORCEINLINE Type& operator&=(Type& Lhs, const Type& Rhs)
{
    Lhs = Lhs & Rhs;
    return Lhs;
}

FORCEINLINE Type operator~(const Type& Lhs)
{
    return static_cast<Type>(~static_cast<LRotatorAxis>(Lhs));
}

template <typename ... FlagsTy>
constexpr ERotatorAxis::Type CombineFlags(FlagsTy ... Flags)
{
    static_assert(
        (std::is_same_v<FlagsTy, ERotatorAxis::Type> && ...),
        "All arguments must be of type EClassFlags::Type."
    );

    return (static_cast<ERotatorAxis::Type>(Flags) | ...);
}

} /* ~Namespace ERotatorAxis */

/** Jafg implementation of a rotational vector. */
template <typename T>
struct TRotator final
{
    static_assert(std::is_floating_point_v<T>, "Generic type T of TVector must be a floating point type.");

    /** Pitch angle in degrees around the Y axis. */
    T Pitch;

    /** Yaw angle in degrees around the Z axis. */
    T Yaw;

    /** Roll angle in degrees around the X axis. */
    T Roll;

    /** Global zero rotator constant (0, 0, 0). */
    static const TRotator<T> ZeroRotator;

    /** Global one rotator constant (1, 1, 1). */
    static const TRotator<T> OneRotator;

    FORCEINLINE          TRotator() = default;
    FORCEINLINE explicit TRotator(const T InFloatingPoint);
    FORCEINLINE explicit TRotator(const T InPitch, const T InYaw, const T InRoll);
    FORCEINLINE          TRotator(const TRotator<T>& InRotator) : Pitch(InRotator.Pitch), Yaw(InRotator.Yaw), Roll(InRotator.Roll) { }
    FORCEINLINE          TRotator(TRotator<T>&& InRotator) noexcept : Pitch(InRotator.Pitch), Yaw(InRotator.Yaw), Roll(InRotator.Roll) { }

    FORCEINLINE TRotator<T>& operator =(const TRotator<T>& InRotator) noexcept;
    FORCEINLINE TRotator<T>& operator =(TRotator<T>&& InRotator) noexcept;
    FORCEINLINE TRotator<T>& operator =(const TRotator<T>&& InRotator) noexcept = delete;

    FORCEINLINE TRotator<T>& Add(const TRotator<T>& InRotator);
    FORCEINLINE TRotator<T>& Subtract(const TRotator<T>& InRotator);
    FORCEINLINE TRotator<T>& Multiply(const T InScale);
    FORCEINLINE TRotator<T>& Divide(const T InScale);

    FORCEINLINE TRotator<T>  operator +(const TRotator<T>& InRotator) const;
    FORCEINLINE TRotator<T>& operator+=(const TRotator<T>& InRotator);
    FORCEINLINE TRotator<T>  operator -(const TRotator<T>& InRotator) const;
    FORCEINLINE TRotator<T>& operator-=(const TRotator<T>& InRotator);
    FORCEINLINE TRotator<T>  operator *(const T InScale) const;
    FORCEINLINE TRotator<T>& operator*=(const T InScale);
    FORCEINLINE TRotator<T>  operator /(const T InScale) const;
    FORCEINLINE TRotator<T>& operator/=(const T InScale);
    FORCEINLINE TRotator<T>  operator -() const { return TRotator<T>(-Pitch, -Yaw, -Roll); }

    FORCEINLINE bool Equals(const TRotator<T>& InRotator, T Tolerance = JAFG_NOT_SO_SMALL_NUMBER) const;
    FORCEINLINE bool operator==(const TRotator<T>& InRotator) const;
    FORCEINLINE bool operator!=(const TRotator<T>& InRotator) const { return !(*this == InRotator); }

    FORCEINLINE bool IsZero() const;
    FORCEINLINE bool IsNearlyZero(T Tolerance = JAFG_NOT_SO_SMALL_NUMBER) const;

    /** Clamps an angle to the range of [0, 360[. */
    FORCEINLINE static auto ClampAxis(T Angle) -> T;
    /** Normalizes an angle to the range of ]-180, 180] */
    FORCEINLINE static auto NormalizeAxis(T Angle) -> T;
    FORCEINLINE        auto NormalizeRotation() -> void;

    /** Constrains one or multiple axes to a defined constraint in the range of ]0, 180]. */
    FORCEINLINE auto ConstrainAxis(const LRotatorAxis AxisFlags, const T Constraint) -> void;

    TVector<T> ToVector() const;

    LString ToString() const
    {
        return LString::SprintF("{:.2f} {:.2f} {:.2f}", Pitch, Yaw, Roll);
    }
};

template <typename T>
TRotator<T>::TRotator(const T InFloatingPoint): Pitch(InFloatingPoint), Yaw(InFloatingPoint), Roll(InFloatingPoint)
{
}

template <typename T>
TRotator<T>::TRotator(const T InPitch, const T InYaw, const T InRoll): Pitch(InPitch), Yaw(InYaw), Roll(InRoll)
{
}

template <typename T>
TRotator<T>& TRotator<T>::operator=(const TRotator<T>& InRotator) noexcept
{
    this->Pitch = InRotator.Pitch; this->Yaw = InRotator.Yaw; this->Roll = InRotator.Roll; return *this;
}

template <typename T>
TRotator<T>& TRotator<T>::operator=(TRotator<T>&& InRotator) noexcept
{
    this->Pitch = InRotator.Pitch; this->Yaw = InRotator.Yaw; this->Roll = InRotator.Roll; return *this;
}

template <typename T>
TRotator<T>& TRotator<T>::Add(const TRotator<T>& InRotator)
{
    *this += InRotator;
    return *this;
}

template <typename T>
TRotator<T>& TRotator<T>::Subtract(const TRotator<T>& InRotator)
{
    *this -= InRotator;
    return *this;
}

template <typename T>
TRotator<T>& TRotator<T>::Multiply(const T InScale)
{
    *this *= InScale;
    return *this;
}

template <typename T>
TRotator<T>& TRotator<T>::Divide(const T InScale)
{
    *this /= InScale;
    return *this;
}

template <typename T>
TRotator<T> TRotator<T>::operator+(const TRotator<T>& InRotator) const
{
    return TRotator<T>(this->Pitch + InRotator.Pitch, this->Yaw + InRotator.Yaw, this->Roll + InRotator.Roll);
}

template <typename T>
TRotator<T>& TRotator<T>::operator+=(const TRotator<T>& InRotator)
{
    this->Pitch += InRotator.Pitch;
    this->Yaw   += InRotator.Yaw;
    this->Roll  += InRotator.Roll;
    return *this;
}

template <typename T>
TRotator<T> TRotator<T>::operator-(const TRotator<T>& InRotator) const
{
    return TRotator<T>(this->Pitch - InRotator.Pitch, this->Yaw - InRotator.Yaw, this->Roll - InRotator.Roll);
}

template <typename T>
TRotator<T>& TRotator<T>::operator-=(const TRotator<T>& InRotator)
{
    this->Pitch -= InRotator.Pitch;
    this->Yaw   -= InRotator.Yaw;
    this->Roll  -= InRotator.Roll;
    return *this;
}

template <typename T>
TRotator<T> TRotator<T>::operator*(const T InScale) const
{
    return TRotator<T>(this->Pitch * InScale, this->Yaw * InScale, this->Roll * InScale);
}

template <typename T>
TRotator<T>& TRotator<T>::operator*=(const T InScale)
{
    this->Pitch *= InScale;
    this->Yaw   *= InScale;
    this->Roll  *= InScale;
    return *this;
}

template <typename T>
TRotator<T> TRotator<T>::operator/(const T InScale) const
{
    return TRotator<T>(this->Pitch / InScale, this->Yaw / InScale, this->Roll / InScale);
}

template <typename T>
TRotator<T>& TRotator<T>::operator/=(const T InScale)
{
    this->Pitch /= InScale;
    this->Yaw   /= InScale;
    this->Roll  /= InScale;
    return *this;
}

template <typename T>
bool TRotator<T>::Equals(const TRotator<T>& InRotator, T Tolerance) const
{
    return Maths::Absolute(NormalizeAxis(Pitch - InRotator.Pitch)) < Tolerance
        && Maths::Absolute(NormalizeAxis(Yaw   - InRotator.Yaw  )) < Tolerance
        && Maths::Absolute(NormalizeAxis(Roll  - InRotator.Roll )) < Tolerance;
}

template <typename T>
bool TRotator<T>::operator==(const TRotator<T>& InRotator) const
{
    return Pitch == InRotator.Pitch && Yaw == InRotator.Yaw && Roll == InRotator.Roll;
}

template <typename T>
bool TRotator<T>::IsZero() const
{
    return ClampAxis(Pitch) == 0 && ClampAxis(Yaw) == 0 && ClampAxis(Roll) == 0;
}

template <typename T>
bool TRotator<T>::IsNearlyZero(T Tolerance) const
{
    return Maths::Absolute(NormalizeAxis(Pitch)) < Tolerance
        && Maths::Absolute(NormalizeAxis(Yaw  )) < Tolerance
        && Maths::Absolute(NormalizeAxis(Roll )) < Tolerance;
}

template <typename T>
T TRotator<T>::ClampAxis(T Angle)
{
    /* @return an angle in the range of ]-360, 360[. */
    Angle = Maths::Fmod(Angle, static_cast<T>(360.0));

    if (Angle < static_cast<T>(0.0))
    {
        /* Shift to [0, 360[ range. */
        Angle += static_cast<T>(360.0);
    }

    return Angle;
}

template <typename T>
T TRotator<T>::NormalizeAxis(T Angle)
{
    Angle = ClampAxis(Angle);
    if (Angle > static_cast<T>(JAFG_DEG_HALF_CIRCLE_D))
    {
        /* Shift to ]-180, 180]. */
        Angle -= static_cast<T>(JAFG_DEG_FULL_CIRCLE_D);
    }
    return Angle;
}

template <typename T>
void TRotator<T>::NormalizeRotation()
{
    this->Pitch = TRotator<T>::NormalizeAxis(this->Pitch);
    this->Yaw   = TRotator<T>::NormalizeAxis(this->Yaw);
    this->Roll  = TRotator<T>::NormalizeAxis(this->Roll);

    return;
}

template <typename T>
void TRotator<T>::ConstrainAxis(const LRotatorAxis AxisFlags, const T Constraint)
{
    check( AxisFlags != ERotatorAxis::None )
    check( Constraint > 0.0f && Constraint <= 180.0f )

    if (AxisFlags & ERotatorAxis::Pitch)
    {
        this->Pitch = Maths::Clamp(this->Pitch, -Constraint, Constraint);
    }

    if (AxisFlags & ERotatorAxis::Yaw)
    {
        this->Yaw = Maths::Clamp(this->Yaw, -Constraint, Constraint);
    }

    if (AxisFlags & ERotatorAxis::Roll)
    {
        this->Roll = Maths::Clamp(this->Roll, -Constraint, Constraint);
    }

    return;
}

template <typename T>
TVector<T> TRotator<T>::ToVector() const
{
    /* Clamp to the range of ]-360, 360[. */
    const T PitchNoWinding = Maths::Fmod(this->Pitch, static_cast<T>(JAFG_DEG_FULL_CIRCLE_D));
    const T YawNoWinding   = Maths::Fmod(this->Yaw,   static_cast<T>(JAFG_DEG_FULL_CIRCLE_D));

    T CP, SP, CY, SY;
    Maths::SinCos(&SP, &CP, Maths::ToRadians(PitchNoWinding));
    Maths::SinCos(&SY, &CY, Maths::ToRadians(YawNoWinding));

    return TVector<T>(CP * CY, CP * SY, SP);
}

} /* ~Namespace Jafg */
