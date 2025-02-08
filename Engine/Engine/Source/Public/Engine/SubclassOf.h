// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/ObjectBase.h"

namespace Jafg
{

template <typename InTObj>
class TSubclassOf;

template <typename InTObj>
class TSubclassOf final
{
    template <typename U>
    friend class TSubclassOf;

public:

    using TObj = InTObj;

    TSubclassOf() = default;
    ~TSubclassOf() = default;

    FORCEINLINE TSubclassOf(LNullptrTy) { this->Class = nullptr; }

    FORCEINLINE TSubclassOf(const LObjectClass* InClass) : Class(InClass) { check( this->IsValidType() ) }
    FORCEINLINE TSubclassOf& operator=(const LObjectClass* InClass)
    {
        this->Class = InClass;
        check( this->IsValidType() )
        return *this;
    }

    template <typename U>
    FORCEINLINE TSubclassOf(const TSubclassOf<U>& Other)
    {
        static_assert(std::is_base_of_v<Private::JObjectBase, U>);
        static_assert(std::is_base_of_v<TObj, U>);
        this->Class = Other.Class;
        check( this->IsValidType() )
    }
    template <typename U>
    FORCEINLINE TSubclassOf& operator=(const TSubclassOf<U>& Other)
    {
        static_assert(std::is_base_of_v<Private::JObjectBase, U>);
        static_assert(std::is_base_of_v<TObj, U>);
        this->Class = Other.Class;
        check( this->IsValidType() )
        return *this;
    }

    template <typename U>
    FORCEINLINE TSubclassOf(U* What)
    {
        static_assert(std::is_base_of_v<Private::JObjectBase, U>);
        static_assert(std::is_base_of_v<TObj, U>);
        this->Class = What->GetVTable();
        check( this->IsValidType() )
        return *this;
    }
    template <typename U>
    FORCEINLINE TSubclassOf& operator=(U* What)
    {
        static_assert(std::is_base_of_v<Private::JObjectBase, U>);
        static_assert(std::is_base_of_v<TObj, U>);
        this->Class = What->GetVTable();
        check( this->IsValidType() )
        return *this;
    }

    FORCEINLINE bool IsSet() const { return this->Class != nullptr; }
    template <typename U>
    FORCEINLINE void Set()
    {
        static_assert(std::is_base_of_v<Private::JObjectBase, U>);
        static_assert(std::is_base_of_v<TObj, U>);
        this->Class = U::StaticClass();
        checkSlow( this->IsValidType() )
        return;
    }
    FORCEINLINE void Set(LNullptrTy)
    {
        this->Class = nullptr;
        checkSlow( this->IsValidType() )
        return;
    }

    FORCEINLINE operator const LObjectClass*() const { return **this; }
    FORCEINLINE const LObjectClass* Get() const { return **this; }
    FORCEINLINE const LObjectClass* operator->() const { return **this; }
    FORCEINLINE const LObjectClass* operator*() const
    {
        if (this->Class == nullptr || this->Class->DerivesFrom(TObj::StaticClass()) == false)
        {
            return nullptr;
        }

        return this->Class;
    }

    FORCEINLINE const TObj* GetDefault()
    {
        if (this->Class)
        {
            Private::JObjectBase* Default = this->Class->GetDefaultPackageReferrer();
            check( Default && Default->GetVTable()->DerivesFrom(TObj::StaticClass()) )
            return static_cast<TObj*>(Default);
        }

        return nullptr;
    }

    FORCEINLINE bool IsValidType()
    {
        if (this->Class)
        {
            return this->Class->DerivesFrom(TObj::StaticClass());
        }
        return true;
    }

    FORCEINLINE operator bool() const { return this->Class != nullptr; }
    FORCEINLINE bool operator==(const LNullptrTy) const { return this->Class == nullptr; }
    FORCEINLINE bool operator!=(const LNullptrTy) const { return this->Class != nullptr; }

private:

    const LObjectClass* Class = nullptr;
};

} /* ~Namespace Jafg */
