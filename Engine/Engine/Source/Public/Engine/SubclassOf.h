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
    DEFAULT_REALLOC_OF_ANY_FORM(TSubclassOf)
    ~TSubclassOf() = default;

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
        this->Class = Other->GetVTable();
        check( this->IsValidType() )
    }
    template <typename U>
    FORCEINLINE TSubclassOf& operator=(const TSubclassOf<U>& Other)
    {
        static_assert(std::is_base_of_v<Private::JObjectBase, U>);
        static_assert(std::is_base_of_v<TObj, U>);
        this->Class = Other->GetVTable();
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

    FORCEINLINE operator LObjectClass*() const { return **this; }
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

private:

    const LObjectClass* Class = nullptr;
};

} /* ~Namespace Jafg */
