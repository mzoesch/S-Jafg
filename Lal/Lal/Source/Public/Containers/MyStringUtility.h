// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg::Strings
{

template <typename TString>
LAL_API TString AddSpacesToCamelCase(const TString& InString);

template <typename TString>
TString AddSpacesToCamelCase(const TString& InString)
{
    TString Out;
    Out.Reserve(InString.GetByteSize());

    bool bFirst = true;
    for (const char& Char : InString)
    {
        if (Char >= 'A' && Char <= 'Z')
        {
            if (bFirst == false)
            {
                Out.Add(' ');
            }
        }

        Out.Add(Char);

        bFirst = false;
        continue;
    }

    return Out;
}

} /* ~Namespace Jafg::Strings */
