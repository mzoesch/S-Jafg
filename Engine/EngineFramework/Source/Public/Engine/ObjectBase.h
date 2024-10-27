// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "Engine/ObjectBaseTypes.h"
#include "Engine/ObjectContext.h"
#include "Engine/ObjectClass.h"
#include "Engine/ObjectBaseUtility.h"
#include "Engine/ObjectMacros.h"
#include "ObjectBase.generated.h"

namespace Jafg
{

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

} /* ~Namespace Jafg */
