// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/ObjectClass.h"
#include "Engine/ObjectBaseUtility.h"

const Jafg::LSimpleString& Jafg::LObjectClass::GetSpacedClassName() const
{
    return Private::GObjectRegistry->GetPanickedPackageByStaticClass(this)->SpacedClassName;
}
