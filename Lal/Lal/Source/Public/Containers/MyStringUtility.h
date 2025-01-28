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

    for (const char& Char : InString)
    {
        if (Char >= 'A' && Char <= 'Z')
        {
            Out.Add(' ');
        }

        Out.Add(Char);
    }

    return Out;
}

} /* ~Namespace Jafg::Strings */
