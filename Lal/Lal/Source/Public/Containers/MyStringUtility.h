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
    Out.Reserve(InString.GetSizeInBytes());

    bool bFirst = true;
    for (const char& Char : InString)
    {
        if (Char >= 'A' && Char <= 'Z')
        {
            if (bFirst == false)
            {
                Out.Append(' ');
            }
        }

        Out.Append(Char);

        bFirst = false;
        continue;
    }

    return Out;
}

} /* ~Namespace Jafg::Strings */
