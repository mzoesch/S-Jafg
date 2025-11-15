// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/SupremePolicies.h"
#include "FwSupremePolicies.generated.h"

namespace Jgc
{

DECLARE_JAFG_CLASS()
class JFwSupremePolicies : public Jafg::JSupremePolicies
{
    GENERATED_CLASS_BODY()

protected:

    explicit JFwSupremePolicies(Jafg::LCxxObjectInitializer const& ObjectInitializer);
    DEFAULT_OBJECT_CDR_CTOR(JFwSupremePolicies)
};

} /* ~Namespace Jgc */
