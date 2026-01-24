// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"
#include "Runtime/Args.h"

namespace Jafg
{

struct LProgramParameter;

namespace Detail
{

ENGINE_API extern std::vector<LProgramParameter*> GRegisteredProgramArguments;

} /* ~Namespace Detail */

struct LProgramParameter
{
    //#
    //# Constructs a program argument and optionally registers it to the global engine argument object.
    //#
    //# @param Identifier    The identifier of the argument.
    //# @param Description   A short description of the argument.
    //# @param DefaultValue  The default value of the argument.
    //# @param bIgnoreOnFail If true, ignores registration failure. Otherwise, the program will panic.
    //# @param bAutoRegister If true, automatically registers the argument upon construction.
    //#
    LProgramParameter(
        LString Identifier,
        LString Description,
        const bool bIgnoreOnFail = true,
        const bool bAutoRegister = true
    ) noexcept
        : Identifier { std::move(Identifier) }
        , Description { std::move(Description) }
    {
        if (bAutoRegister)
        {
            if (const bool bOk { Register() }; bIgnoreOnFail == false && bOk == false)
            {
                LOG_FATAL(LogProgramArguments,
                    "Failed to register program argument [{}].",
                    Identifier
                    )
            }
        }

        return;
    }

    //# @return True if successfully registered to the global engine argument object.
    bool Register()
    {
        if (std::ranges::contains(Jafg::Detail::GRegisteredProgramArguments, this))
        {
            return false;
        }

        Jafg::Detail::GRegisteredProgramArguments.emplace_back(this);

        return true;
    }

    LString Identifier;
    LString Description;
};

} /* ~Namespace Jafg */
