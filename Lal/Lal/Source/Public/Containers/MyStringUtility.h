// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg::Strings
{

template <typename TString>
LAL_API TString AddSpacesToCamelCase(const TString& InString);

template <typename TString>
TString AddSpacesToCamelCase(const TString& InString)
{
    TString Out;
    Out.reserve(InString.size());

    bool bFirst = true;
    for (const char& Char : InString)
    {
        if (Char >= 'A' && Char <= 'Z')
        {
            if (bFirst == false)
            {
                Out.push_back(' ');
            }
        }

        Out.push_back(Char);

        bFirst = false;
        continue;
    }

    return Out;
}

} /* ~Namespace Jafg::Strings */
