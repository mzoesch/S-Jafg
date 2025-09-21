// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"
#include "Engine/ObjectBaseTypes.h"
#include "Core/Name.h"
#include "Foreign/PluginForward.h"

namespace Jafg
{

class JObjectBase;

namespace Private
{

class LObjectRegistry;
struct LRegistrationCallbackHelper;

} /* ~Namespace Private */

//#
//# LObjectClass aka a static class.
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

    FORCEINLINE const LString& GetSpacedClassName() const { return this->SpacedClassName; }
    FORCEINLINE LName          GetName() const { return this->ClassName; }

    FORCEINLINE LLoadedPluginHandle GetPluginHandle() const { return this->PluginHandle; }

    FORCEINLINE auto GetParent()         ->       LObjectClass*            { return this->Parent; }
    FORCEINLINE auto GetParent()   const -> const LObjectClass*            { return this->Parent; }
    FORCEINLINE auto GetChildren()       ->       TArray<LObjectClass*>& { return this->Children; }
    FORCEINLINE auto GetChildren() const -> const TArray<LObjectClass*>& { return this->Children; }

    /** Check if this object derives from the given parent. */
    ENGINE_API bool DerivesFrom(const LObjectClass* InParent) const;

    FORCEINLINE i32 GetTotalByteSize() const { return this->TotalByteSize; }

    FORCEINLINE auto GetFlags()      const -> LClassFlags { return this->Flags; }
    FORCEINLINE bool HasAnyFlags()   const { return  this->Flags != EClassFlags::None;                           }
    FORCEINLINE bool IsAbstract()    const { return (this->Flags  & EClassFlags::Abstract) != EClassFlags::None; }
    FORCEINLINE bool IsNotAbstract() const { return (this->Flags  & EClassFlags::Abstract) == EClassFlags::None; }
    FORCEINLINE bool IsConfig()      const { return (this->Flags  & EClassFlags::Config)   != EClassFlags::None; }
    FORCEINLINE bool IsNotConfig()   const { return (this->Flags  & EClassFlags::Config)   == EClassFlags::None; }

    FORCEINLINE const JObjectBase* GetDefaultPackageReferrer() const { return this->DefaultPackageReferrer;}
    FORCEINLINE       JObjectBase* GetMutableDefaultPackageReferrer() const { return this->DefaultPackageReferrer;}

    template <typename TObj>
    FORCEINLINE const TObj* GetDefaultPackageReferrer() const
    {
        static_assert(std::is_base_of_v<JObjectBase, TObj>, "TObj must be a derived class of JObjectBase.");
        return static_cast<const TObj*>(this->DefaultPackageReferrer);
    }
    template <typename TObj>
    FORCEINLINE TObj* GetMutableDefaultPackageReferrer() const
    {
        static_assert(std::is_base_of_v<JObjectBase, TObj>, "TObj must be a derived class of JObjectBase.");
        return static_cast<TObj*>(this->DefaultPackageReferrer);
    }

private:

    LString SpacedClassName;
    LName   ClassName;

    LLoadedPluginHandle PluginHandle;

    //# The single parent of this object.
    LObjectClass* Parent { nullptr };

    //# All the children that this object acts as a meaningful parent.
    TArray<LObjectClass*> Children;

    //# The total byte size from one instance of this object.
    i32 TotalByteSize { INDEX_NONE };

    //# The flags that describe this object class. */
    LClassFlags Flags { EClassFlags::None };

    //# The default object initializer for this object class.
    JObjectBase* DefaultPackageReferrer { nullptr };
};

} /* ~Namespace Jafg */
