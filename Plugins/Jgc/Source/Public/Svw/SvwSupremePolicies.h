// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SupremePolicies.h"
#include "SvwSupremePolicies.generated.h"

namespace Jgc
{

DECLARE_JAFG_CLASS()
class JSvwSupremePolicies : public Jafg::JSupremePolicies
{
    GENERATED_CLASS_BODY()

protected:

    explicit JSvwSupremePolicies(Jafg::LCxxObjectInitializer const& ObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(JSvwSupremePolicies)
};

} /* ~Namespace Jgc */
