// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Engine/ObjectBaseTypes.h"
#include "Core/Name.h"

namespace Jafg
{

class JObjectBase;

namespace Private
{

class LObjectRegistry;
struct LRegistrationCallbackHelper;

} /* ~Namespace Private */

//#
//# LObjectClass aka StaticClass.
//#
//# Wrapper around the jafg object referrer class that provides some extra information.
//#
class LObjectClass final
{
    friend Private::LObjectRegistry;
    friend Private::LRegistrationCallbackHelper;

public:

    LObjectClass()  = default;
    PROHIBIT_REALLOC_OF_ANY_FORM(LObjectClass)
    ~LObjectClass() = default;

    FORCEINLINE auto GetSpacedClassName() const -> const LSimpleString& { return this->SpacedClassName; }
    FORCEINLINE auto GetName() const -> LName { return this->ClassName; }

    template <typename TObj>
    FORCEINLINE auto GetDefaultPackageReferrer() const -> const TObj*
    {
        static_assert(std::is_base_of_v<JObjectBase, TObj>, "TObj must be a derived class of JObjectBase.");
        return static_cast<const TObj*>(this->DefaultPackageReferrer);
    }
    template <typename TObj>
    FORCEINLINE auto GetMutableDefaultPackageReferrer() const -> TObj*
    {
        static_assert(std::is_base_of_v<JObjectBase, TObj>, "TObj must be a derived class of JObjectBase.");
        return static_cast<TObj*>(this->DefaultPackageReferrer);
    }

    FORCEINLINE auto GetDefaultPackageReferrer()        const -> const JObjectBase* { return this->DefaultPackageReferrer;}
    FORCEINLINE auto GetMutableDefaultPackageReferrer() const ->       JObjectBase* { return this->DefaultPackageReferrer;}

    FORCEINLINE auto GetParent()         ->       LObjectClass*            { return this->Parent; }
    FORCEINLINE auto GetParent()   const -> const LObjectClass*            { return this->Parent; }
    FORCEINLINE auto GetChildren()       ->       TArray<LObjectClass*>& { return this->Children; }
    FORCEINLINE auto GetChildren() const -> const TArray<LObjectClass*>& { return this->Children; }

    /** Check if this object derives from the given parent. */
    ENGINE_API auto DerivesFrom(const LObjectClass* InParent) const -> bool;

    FORCEINLINE auto GetTotalByteSize() const -> i32 { return this->TotalByteSize; }

    FORCEINLINE auto GetFlags()      const -> LClassFlags { return this->Flags; }
    FORCEINLINE bool HasAnyFlags()   const { return  this->Flags != EClassFlags::None;                           }
    FORCEINLINE bool IsAbstract()    const { return (this->Flags  & EClassFlags::Abstract) != EClassFlags::None; }
    FORCEINLINE bool IsNotAbstract() const { return (this->Flags  & EClassFlags::Abstract) == EClassFlags::None; }
    FORCEINLINE bool IsConfig()      const { return (this->Flags  & EClassFlags::Config)   != EClassFlags::None; }
    FORCEINLINE bool IsNotConfig()   const { return (this->Flags  & EClassFlags::Config)   == EClassFlags::None; }

private:

    LSimpleString           SpacedClassName        = { };
    LName                   ClassName              = { };

    //# The single parent of this object.
    LObjectClass*           Parent                 = nullptr;

    //# All the children that this object acts as a meaningful parent.
    TArray<LObjectClass*> Children               = { };

    //# The total byte size from one instance of this object.
    i32                   TotalByteSize         = INDEX_NONE;

    //# The flags that describe this object class. */
    LClassFlags             Flags                  = EClassFlags::None;

    //# The default object initializer for this object class.
    JObjectBase*   DefaultPackageReferrer = nullptr;
};

} /* ~Namespace Jafg */
