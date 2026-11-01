// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

//#
//# Optional plugin info that plugins may use / derive for ease of use.
//#
struct LPluginInfo
{
    LPath Content;
    LString ContentString;

    void Initialize(LPath Path)
    {
        check(this->Content.empty())
        this->Content = canonical(Path);
        this->ContentString = this->Content.string();
    }

    NODISCARD FORCEINLINE operator LPath const&() const noexcept
    {
        check(!this->Content.empty())
        return this->Content;
    }
    NODISCARD FORCEINLINE operator LString const&() const noexcept
    {
        check(!this->ContentString.empty())
        return this->ContentString;
    }
    NODISCARD FORCEINLINE operator LStringView() const noexcept
    {
        check(!this->ContentString.empty())
        return this->ContentString;
    }

    NODISCARD FORCEINLINE constexpr LPath const& operator*() const noexcept
    {
        check(!this->Content.empty())
        return this->Content;
    }

    NODISCARD LPath operator/(auto&& Path) const noexcept
        requires requires(LPath const& P) { P / Path; }
    {
        check(!this->Content.empty())
        return this->Content / Path;
    }
};

} /* ~Namespace Jafg */
