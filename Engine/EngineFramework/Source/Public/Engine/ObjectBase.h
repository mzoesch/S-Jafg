// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine/ObjectContext.h"

namespace Jafg
{

namespace Private
{

PRAGMA_FOR_JAFG_BUILD_TOOL("NextIsObjectBaseClass")
DECLARE_JAFG_CLASS()
class ENGINEFRAMEWORK_API JObjectBase
{
    GENERATED_CLASS_BODY()

protected:

    JObjectBase() = default;
    explicit JObjectBase(::Jafg::Private::LObjectContext* Outer) : Outer(Outer) { }
    PROHIBIT_REALLOC_OF_ANY_FROM(JObjectBase)
    virtual ~JObjectBase() = default;

public:

    FORCEINLINE virtual auto HasContext() const -> bool { return this->Outer != nullptr; }
    FORCEINLINE virtual auto GetContext() const -> ::Jafg::Private::LObjectContext* { return this->Outer; }

    virtual void BeginLife();

private:

    ::Jafg::Private::LObjectContext* Outer = nullptr;

};

} /* ~Namespace Private */

template <typename T = ::Jafg::Private::JObjectBase, typename U = ::Jafg::Private::LObjectContext>
T* TempHelperJObjectCreation(U* Outer)
{
    T* Out = new T(Outer);
    Out->BeginLife();
    return Out;
}

::Jafg::Private::JObjectBase* NewJObject();
::Jafg::Private::JObjectBase* NewJObject(::Jafg::Private::LObjectContext* Outer);

} /* ~Namespace Jafg */
