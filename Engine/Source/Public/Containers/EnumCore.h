// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template<typename T> requires std::is_enum_v<T>
struct TFlags
{
public:

    typedef T type;
    typedef std::underlying_type_t<type> mask;

    FORCEINLINE constexpr TFlags() noexcept : Mask{0} {}
    FORCEINLINE constexpr TFlags(type Bits) noexcept : Mask(static_cast<mask>(Bits)) {}
    FORCEINLINE constexpr TFlags(TFlags const&) noexcept = default;
    FORCEINLINE constexpr explicit TFlags(mask Mask) noexcept : Mask(Mask) {}

    FORCEINLINE constexpr auto operator<=>(TFlags const&) const = default;

    FORCEINLINE constexpr bool operator!() const noexcept { return !Mask; }
    FORCEINLINE explicit constexpr operator bool() const noexcept { return !!this->Mask; }
    FORCEINLINE explicit constexpr operator mask() const noexcept { return this->Mask; }

    FORCEINLINE constexpr TFlags& operator|=(TFlags const& Rhs) noexcept { this->Mask |= Rhs.Mask; return *this; }
    FORCEINLINE constexpr TFlags& operator&=(TFlags const& Rhs) noexcept { this->Mask &= Rhs.Mask; return *this; }
    FORCEINLINE constexpr TFlags& operator^=(TFlags const& Rhs) noexcept { this->Mask ^= Rhs.Mask; return *this; }

    FORCEINLINE constexpr TFlags operator|(TFlags const& Rhs) const noexcept { return TFlags{this->Mask | Rhs.Mask}; }
    FORCEINLINE constexpr TFlags operator&(TFlags const& Rhs) const noexcept { return TFlags{this->Mask & Rhs.Mask}; }
    FORCEINLINE constexpr TFlags operator^(TFlags const& Rhs) const noexcept { return TFlags{this->Mask ^ Rhs.Mask}; }

    //# TODO: This essentially requires reflection...
    FORCEINLINE constexpr TFlags operator~() const noexcept = delete;

    FORCEINLINE constexpr TFlags& operator=(TFlags const& Rhs ) noexcept = default;

private:

    mask Mask;
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
