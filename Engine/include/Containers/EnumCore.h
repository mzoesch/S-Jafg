// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

namespace Detail
{

struct LFlags{};

} /* ~Namespace Detail */

template<typename T> requires std::is_enum_v<T>
struct TFlags : public Detail::LFlags
{
    typedef T type;
    typedef std::underlying_type_t<type> mask;

    FORCEINLINE constexpr TFlags() noexcept : _Mask{0} {}
    FORCEINLINE constexpr TFlags(type Bits) noexcept : _Mask(static_cast<mask>(Bits)) {}
    FORCEINLINE constexpr TFlags(TFlags const&) noexcept = default;
    FORCEINLINE constexpr explicit TFlags(mask Mask) noexcept : _Mask(Mask) {}

    FORCEINLINE constexpr bool operator==(TFlags const& Rhs) const noexcept { return this->_Mask == Rhs._Mask; }
    FORCEINLINE constexpr auto operator<=>(TFlags const& Rhs) const noexcept { return this->_Mask <=> Rhs._Mask; }

    FORCEINLINE constexpr bool operator!() const noexcept { return !this->_Mask; }
    FORCEINLINE explicit constexpr operator bool() const noexcept { return !!this->_Mask; }
    FORCEINLINE explicit constexpr operator mask() const noexcept { return this->_Mask; }

    FORCEINLINE constexpr TFlags& operator|=(TFlags Rhs) noexcept { this->_Mask |= Rhs._Mask; return *this; }
    FORCEINLINE constexpr TFlags& operator&=(TFlags Rhs) noexcept { this->_Mask &= Rhs._Mask; return *this; }
    FORCEINLINE constexpr TFlags& operator^=(TFlags Rhs) noexcept { this->_Mask ^= Rhs._Mask; return *this; }

    FORCEINLINE constexpr TFlags operator|(TFlags Rhs) const noexcept { return TFlags{this->_Mask | Rhs._Mask}; }
    FORCEINLINE constexpr TFlags operator&(TFlags Rhs) const noexcept { return TFlags{this->_Mask & Rhs._Mask}; }
    FORCEINLINE constexpr TFlags operator^(TFlags Rhs) const noexcept { return TFlags{this->_Mask ^ Rhs._Mask}; }

    //# Remember to _and_ the result into a valid bitmask to preserve type safety.
    FORCEINLINE constexpr TFlags operator~() const noexcept { return TFlags{~this->_Mask}; }

    FORCEINLINE constexpr TFlags& operator=(TFlags const& Rhs ) noexcept = default;

    //# Do not access manually!!! For escaping nttp only!
    mask _Mask;
};

} /* ~Namespace Jafg */

#define ENUM_STRUCT_FLAGS(Enum, Flags)                                               \
    typedef ::Jafg::TFlags<Enum> Flags;                                               \
    FORCEINLINE constexpr ::Jafg::TFlags<Enum> operator|(Enum Lhs, Enum Rhs) noexcept \
    {                                                                                \
        return ::Jafg::TFlags<Enum>{Lhs} | Rhs;                                       \
    }                                                                                \
    FORCEINLINE constexpr ::Jafg::TFlags<Enum> operator&(Enum Lhs, Enum Rhs) noexcept \
    {                                                                                \
        return ::Jafg::TFlags<Enum>{Lhs} & Rhs;                                       \
    }                                                                                \
    FORCEINLINE constexpr ::Jafg::TFlags<Enum> operator^(Enum Lhs, Enum Rhs) noexcept \
    {                                                                                \
        return ::Jafg::TFlags<Enum>{Lhs} ^ Rhs;                                       \
    }
