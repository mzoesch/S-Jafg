// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

//# The state that is required after a resource load function was called.
enum struct EResourceStateBits
{
    //#
    //# No state is required. Just allocate the resource. The client will handle the rest.
    //# If another client already requested host or device memory then the resource will be in the same
    //# state as requested previously by the other client.
    //#
    None = 0 << 0,
    //#
    //# The mesh data will be loaded to host memory. If another client already requested device memory
    //# then the resource will be loaded to host and device memory.
    //#
    Host = 1 << 0,
    //#
    //# The mesh data will be loaded to device memory. Host memory will be orphaned if host memory was
    //# not requested by another client.
    //#
    Device = 1 << 1,
};
ENUM_STRUCT_FLAGS(EResourceStateBits, EResourceState)

//# A shared owning reference to a resource.
template<typename T>
struct TResourceReference final
{
    constexpr TResourceReference() noexcept = default;
    explicit TResourceReference(std::shared_ptr<T> InResource) noexcept : Resource{std::move(InResource)} {}
    DEFAULT_REALLOC_OF_ANY_FORM(TResourceReference)
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
