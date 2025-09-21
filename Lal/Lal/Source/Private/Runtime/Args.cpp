// Copyright mzoesch. All rights reserved.

#include "Runtime/Args.h"

namespace JafgCore
{

CORE_API extern std::vector<Lal::LProgramParameter*> GRegisteredProgramArguments;

} /* ~Namespace JafgCore */

bool Lal::LProgramParameter::Register()
{
    if (std::ranges::contains(JafgCore::GRegisteredProgramArguments, this))
    {
        return false;
    }

    JafgCore::GRegisteredProgramArguments.emplace_back(this);

    return true;
}
