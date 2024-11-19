// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

class JObjectBase;

namespace Smart
{

/**
 * Jafg implementation of a non-intrusive unique pointer that is usable with j objects.
 */
template <typename T>
struct TUnique final
{
    static_assert(!std::is_array_v<T>, "TUnique does not support arrays.");
    static_assert(!std::is_reference_v<T>, "TUnique does not support references.");
    static_assert(!std::is_void_v<T>, "TUnique does not support void.");
    static_assert(!std::is_const_v<T>, "TUnique does not support const.");
    static_assert(!std::is_volatile_v<T>, "TUnique does not support volatile.");
    static_assert(!std::is_pointer_v<T>, "TUnique does not support pointers.");
    static_assert(!std::is_same_v<T, JObjectBase>, "TUnique does not support j objects.");
    static_assert(!std::derived_from<T, JObjectBase>, "TUnique does not support j objects.");
    static_assert(!std::is_same_v<T, std::nullptr_t>, "TUnique does not support nullptr.");

    TUnique() = default;
    TUnique(T* InObject) : Object(InObject) { }
    PROHIBIT_COPY(TUnique)
    TUnique(TUnique&& InOther) noexcept : Object(InOther.Object) { InOther.Object = nullptr; }
    TUnique& operator=(T* InObject) { delete this->Object; this->Object = InObject; return *this; }
    TUnique& operator=(TUnique&& InOther) noexcept { delete this->Object; this->Object = InOther.Object; InOther.Object = nullptr; return *this; }
    ~TUnique() { delete this->Object; }

    FORCEINLINE bool IsValid() const { return this->Object != nullptr; }
    FORCEINLINE void Set(T* InObject) { delete this->Object; this->Object = InObject; }
    FORCEINLINE void Reset() { delete this->Object; this->Object = nullptr; }

    FORCEINLINE auto GetValue() -> T& { check( this->Object ) return *this->Object; }
    FORCEINLINE auto GetValue() const -> const T& { check( this->Object ) return *this->Object; }
    FORCEINLINE auto GetValue(const T& DefaultValue) const -> T& { return this->Object ? *this->Object : DefaultValue; }
    FORCEINLINE auto GetValue(T&& DefaultValue) const -> T& { return this->Object ? *this->Object : std::move(DefaultValue); }
    FORCEINLINE auto GetValuePtr() -> T* { return this->Object; }
    FORCEINLINE auto GetValuePtr() const -> const T* { return this->Object; }

    FORCEINLINE auto MoveOut() -> TUnique { return std::move(*this); }

    FORCEINLINE auto operator +() -> T& { return this->GetValue(); }
    FORCEINLINE auto operator +() const -> const T& { return this->GetValue(); }
    FORCEINLINE auto operator -() -> T& { return this->GetValue(); }
    FORCEINLINE auto operator -() const -> const T& { return this->GetValue(); }
    FORCEINLINE auto operator *() -> T& { return this->GetValue(); }
    FORCEINLINE auto operator *() const -> const T& { return this->GetValue(); }
    FORCEINLINE auto operator !() const -> bool { return !this->Object; }
    FORCEINLINE      operator bool() const { return this->Object != nullptr; }

    FORCEINLINE auto operator!=(const TUnique& Other) const -> bool { return !(*this == Other); }
    FORCEINLINE auto operator==(const TUnique& Other) const -> bool { return this->Object == Other.Object; }
    FORCEINLINE auto operator!=(const T* Other) const -> bool { return this->Object != Other; }
    FORCEINLINE auto operator==(const T* Other) const -> bool { return this->Object == Other; }

private:

    T* Object = nullptr;
};

template <typename T>
FORCEINLINE TUnique<T> MakeUnique(T* InObject)
{
    return TUnique<T>(InObject);
}

template <typename T, typename ... Args>
FORCEINLINE auto EmplaceUnique(Args&&... InArgs) -> TUnique<T>
{
    return TUnique<T>(new T(std::forward<Args>(InArgs)...));
}

} /* ~Namespace Smart */

} /* ~Namespace Jafg */
