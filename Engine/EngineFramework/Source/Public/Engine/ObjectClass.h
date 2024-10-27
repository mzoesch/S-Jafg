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
 * Wrapper around the jafg object referrer class that provide some extra information.
 */
class LObjectClass final
{
    friend Private::LObjectRegistry;
    friend Private::LRegistrationCallbackHelper;

public:

    LObjectClass()  = default;
    PROHIBIT_REALLOC_OF_ANY_FROM(LObjectClass)
    ~LObjectClass() = default;

    ENGINEFRAMEWORK_API auto GetSpacedClassName() const -> const LSimpleString&;

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

    FORCEINLINE auto GetTotalByteSize() const -> int32 { return this->TotalByteSize; }
    FORCEINLINE auto GetFlags() const -> LClassFlags { return this->Flags; }

private:

    /** The total byte size from one instance of this object. */
    int32                 TotalByteSize          = INDEX_NONE;

    /** The flags that describe this object class. */
    LClassFlags           Flags                  = EClassFlags::None;

    /** The default object initializer for this object class. */
    Private::JObjectBase* DefaultPackageReferrer = nullptr;
};

} /* ~Namespace Jafg */
