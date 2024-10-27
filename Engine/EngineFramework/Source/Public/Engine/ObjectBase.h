// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine/ObjectContext.h"
#include "Engine/ObjectBaseUtility.h"
#include "Engine/ObjectMacros.h"
#include "ObjectBase.generated.h"

namespace Jafg
{

struct LTemporalStructForCreatingObjects;

struct LObjectInitializer final
{
    LObjectInitializer()  = default;
    ~LObjectInitializer() = default;

    Private::LObjectContext* Outer = nullptr;
};

FORCEINLINE ENGINEFRAMEWORK_API auto GetDefaultObjectInitializer() -> LObjectInitializer { return LObjectInitializer(); }

namespace Private
{

PRAGMA_FOR_JAFG_BUILD_TOOL("NextIsObjectBaseClass")
DECLARE_JAFG_CLASS()
class ENGINEFRAMEWORK_API JObjectBase
{
    GENERATED_CLASS_BODY()

protected:

    explicit JObjectBase(const LObjectInitializer& ObjectInitializer);
    virtual ~JObjectBase() = default;

public:

    FORCEINLINE virtual auto HasContext() const -> bool { return this->Outer != nullptr; }
    FORCEINLINE virtual auto GetContext() const -> ::Jafg::Private::LObjectContext* { return this->Outer; }

    virtual void BeginLife();

private:

    ::Jafg::Private::LObjectContext* Outer = nullptr;
};

} /* ~Namespace Private */

struct LTemporalStructForCreatingObjects final
{
    LTemporalStructForCreatingObjects() = delete;
    PROHIBIT_REALLOC_OF_ANY_FROM(LTemporalStructForCreatingObjects)
    ~LTemporalStructForCreatingObjects() = delete;

    template <typename T = ::Jafg::Private::JObjectBase, typename U = ::Jafg::Private::LObjectContext>
    static T* TempHelperJObjectCreation(U* Outer)
    {
        LObjectInitializer ObjectInitializer = GetDefaultObjectInitializer();
        ObjectInitializer.Outer = Outer;
        T* Out = new T(ObjectInitializer);
        Out->BeginLife();
        return Out;
    }

};

template <typename T = ::Jafg::Private::JObjectBase, typename U = ::Jafg::Private::LObjectContext>
T* TempHelperJObjectCreation(U* Outer)
{
    return LTemporalStructForCreatingObjects::TempHelperJObjectCreation<T, U>(Outer);
}

::Jafg::Private::JObjectBase* NewJObject();
::Jafg::Private::JObjectBase* NewJObject(::Jafg::Private::LObjectContext* Outer);

} /* ~Namespace Jafg */
