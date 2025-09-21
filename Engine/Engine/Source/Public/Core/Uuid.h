// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg
{

typedef u32 LUnderlyingUuidType;

namespace Private
{

ENGINE_API extern LUnderlyingUuidType CurrentUuidValue;

} /* ~Namespace Private */

struct LUuid final
{
    static constexpr LUnderlyingUuidType NoUuid { 0 };

    FORCEINLINE constexpr LUuid() noexcept = default;
    FORCEINLINE constexpr LUuid(const LUuid& InOther) noexcept = default;
    FORCEINLINE constexpr LUuid(LUuid&& InOther) noexcept;
    FORCEINLINE constexpr LUuid& operator=(const LUuid& InOther) noexcept = default;
    FORCEINLINE constexpr LUuid& operator=(LUuid&& InOther) noexcept;
    FORCEINLINE constexpr ~LUuid() noexcept = default;

    FORCEINLINE constexpr bool IsValid() const noexcept { return this->Value != NoUuid; }
    FORCEINLINE constexpr LUnderlyingUuidType GetValue() const noexcept { return this->Value; }

    FORCEINLINE constexpr  void Reset() noexcept { this->Value = NoUuid; }

    FORCEINLINE std::strong_ordering operator<=>(const LUuid& InOther) const noexcept = default;

    FORCEINLINE void         GenerateNew() noexcept;
    FORCEINLINE static LUuid Generate();

private:

    FORCEINLINE explicit constexpr LUuid(const LUnderlyingUuidType InValue) noexcept;

    LUnderlyingUuidType Value { NoUuid };
};

FORCEINLINE constexpr LUuid::LUuid(LUuid&& InOther) noexcept
    : Value(std::move(InOther.Value))
{
    InOther.Value = NoUuid;
}

FORCEINLINE constexpr LUuid& LUuid::operator=(LUuid&& InOther) noexcept
{
    this->Value = std::move(InOther.Value);
    InOther.Value = NoUuid;
    return *this;
}

FORCEINLINE void LUuid::GenerateNew() noexcept
{
    this->Value = ++Private::CurrentUuidValue;
}

FORCEINLINE LUuid LUuid::Generate()
{
    return LUuid{ ++Private::CurrentUuidValue };
}

FORCEINLINE constexpr LUuid::LUuid(const LUnderlyingUuidType InValue) noexcept: Value(InValue)
{
    check( InValue != NoUuid )
}

} /* ~Namespace Jafg */
