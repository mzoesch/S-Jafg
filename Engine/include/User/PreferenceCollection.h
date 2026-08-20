// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

struct LPreference;
struct LPreferenceValue;
struct LPreferenceCollection;

namespace Detail
{

struct LPreferenceSealer
{
    friend LPreferenceValue;
    friend LPreferenceCollection;
private:
    consteval LPreferenceSealer() noexcept {}
};

} /* ~Namespace Detail */

enum struct EPreference
{
    Value,
    Collection,
};

struct LPreference
{
    LPreference(EPreference Type
        , LString Identifier
        , LString DisplayName
        , Detail::LPreferenceSealer) noexcept
        : Identifier{std::move(Identifier)}, DisplayName{std::move(DisplayName)}, Type{Type} {}
    PROHIBIT_COPY(LPreference)
    DEFAULT_MOVE(LPreference)
    virtual ~LPreference() noexcept = default;

    LString Identifier;
    LString DisplayName;

    NODISCARD FORCEINLINE bool IsValue() const noexcept { return this->Type == EPreference::Value; }
    NODISCARD FORCEINLINE bool IsCollection() const noexcept { return this->Type == EPreference::Collection; }

    NODISCARD FORCEINLINE constexpr LPreferenceValue& AsValue() noexcept;
    NODISCARD FORCEINLINE constexpr LPreferenceValue const& AsValue() const noexcept;
    NODISCARD FORCEINLINE constexpr LPreferenceCollection& AsCollection() noexcept;
    NODISCARD FORCEINLINE constexpr LPreferenceCollection const& AsCollection() const noexcept;

private:

    EPreference Type;
};

//# A nestable collection of preferences.
struct LPreferenceCollection: LPreference
{
    LPreferenceCollection(
          LString Identifier
        , LString DisplayName
        ) noexcept: LPreference{EPreference::Collection, std::move(Identifier), std::move(DisplayName), {}} {}

    NODISCARD decltype(auto) begin(this auto&& Self) noexcept { return Self.Children.begin(); }
    NODISCARD decltype(auto) end(this auto&& Self) noexcept { return Self.Children.end(); }

    LPreferenceCollection& EmplaceCollection(auto&&... Args) noexcept
        requires std::constructible_from<LPreferenceCollection, decltype(Args)...>
    {
        return *this->Children.emplace_back(std::make_unique<LPreferenceCollection>(std::forward<decltype(Args)>(Args)...));
    }

    TArray<std::unique_ptr<LPreference>> Children;
};

FORCEINLINE constexpr LPreferenceCollection& LPreference::AsCollection() noexcept
{
    check(this->Type == EPreference::Collection)
    return *static_cast<LPreferenceCollection*>(this);
}
FORCEINLINE constexpr LPreferenceCollection const& LPreference::AsCollection() const noexcept
{
    check(this->Type == EPreference::Collection)
    return *static_cast<LPreferenceCollection const*>(this);
}

} /* ~Namespace Jafg */
