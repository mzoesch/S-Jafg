// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine/ObjectBaseTypes.h"

namespace Jafg
{

namespace Private
{

class JObjectBase;
class LObjectRegistry;
struct LRegistrationCallbackHelper;

} /* ~Namespace Private */

/**
 * LObjectClass aka StaticClass.
 *
 * Wrapper around the jafg object referrer class that provides some extra information.
 */
class LObjectClass final
{
    friend Private::LObjectRegistry;
    friend Private::LRegistrationCallbackHelper;

public:

    LObjectClass()  = default;
    PROHIBIT_REALLOC_OF_ANY_FROM(LObjectClass)
    ~LObjectClass() = default;

    FORCEINLINE auto GetSpacedClassName() const -> const LSimpleString& { return this->SpacedClassName; }

    template <typename TObj>
    FORCEINLINE auto GetDefaultPackageReferrer() const -> const TObj*
    {
        static_assert(std::is_base_of_v<Private::JObjectBase, TObj>, "TObj must be a derived class of JObjectBase.");
        return static_cast<const TObj*>(this->DefaultPackageReferrer);
    }
    template <typename TObj>
    FORCEINLINE auto GetMutableDefaultPackageReferrer() const -> TObj*
    {
        static_assert(std::is_base_of_v<Private::JObjectBase, TObj>, "TObj must be a derived class of JObjectBase.");
        return static_cast<TObj*>(this->DefaultPackageReferrer);
    }

    FORCEINLINE auto GetDefaultPackageReferrer()        const -> const Private::JObjectBase* { return this->DefaultPackageReferrer;}
    FORCEINLINE auto GetMutableDefaultPackageReferrer() const ->       Private::JObjectBase* { return this->DefaultPackageReferrer;}

    FORCEINLINE auto GetParent()         ->       LObjectClass*            { return this->Parent; }
    FORCEINLINE auto GetParent()   const -> const LObjectClass*            { return this->Parent; }
    FORCEINLINE auto GetChildren()       ->       TdhArray<LObjectClass*>& { return this->Children; }
    FORCEINLINE auto GetChildren() const -> const TdhArray<LObjectClass*>& { return this->Children; }

    /** Check if this object derives from the given parent. */
    ENGINEFRAMEWORK_API auto DerivesFrom(const LObjectClass* InParent) const -> bool;

    FORCEINLINE auto GetTotalByteSize() const -> int32 { return this->TotalByteSize; }

    FORCEINLINE auto GetFlags()      const -> LClassFlags { return this->Flags; }
    FORCEINLINE auto HasAnyFlags()   const -> bool        { return  this->Flags != EClassFlags::None;                           }
    FORCEINLINE auto IsAbstract()    const -> bool        { return (this->Flags  & EClassFlags::Abstract) != EClassFlags::None; }
    FORCEINLINE auto IsNotAbstract() const -> bool        { return (this->Flags  & EClassFlags::Abstract) == EClassFlags::None; }

private:

    LSimpleString           SpacedClassName        = { };

    /** The single parent of this object. */
    LObjectClass*           Parent                 = nullptr;

    /** All the children that this object acts as a meaningful parent. */
    TdhArray<LObjectClass*> Children               = { };

    /** The total byte size from one instance of this object. */
    int32                   TotalByteSize         = INDEX_NONE;

    /** The flags that describe this object class. */
    LClassFlags             Flags                  = EClassFlags::None;

    /** The default object initializer for this object class. */
    Private::JObjectBase*   DefaultPackageReferrer = nullptr;
};

} /* ~Namespace Jafg */
