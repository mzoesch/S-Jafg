// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

//# A shared owning reference to a resource.
template<typename T>
struct TResourceReference final
{
    constexpr TResourceReference() noexcept = default;
    explicit TResourceReference(std::shared_ptr<T> InResource) noexcept : Resource{std::move(InResource)} {}
    DEFAULT_MOVE(TResourceReference)
    PROHIBIT_COPY(TResourceReference)
    ~TResourceReference() noexcept = default;

    //# @return A new owning handle to the underlying resource.
    inline decltype(auto) GetNewHandle(this auto&& Self) noexcept
    {
        check(Self.IsValid())
        return std::remove_cvref_t<decltype(Self)>{Self.Resource};
    }

    inline bool IsValid() const noexcept { return this->Resource.get() != nullptr; }
    inline explicit operator bool() const noexcept { return this->IsValid(); }

    inline CONSTEXPR_CHECK T& operator*() noexcept { check(this->IsValid()) return *this->Resource; }
    inline CONSTEXPR_CHECK T const& operator*() const noexcept { check(this->IsValid()) return *this->Resource; }
    inline CONSTEXPR_CHECK T* operator->() noexcept { check(this->IsValid()) return this->Resource.get(); }
    inline CONSTEXPR_CHECK T const* operator->() const noexcept { check(this->IsValid()) return this->Resource.get(); }

private:

    std::shared_ptr<T> Resource;
};

} /* ~Namespace Jafg */
