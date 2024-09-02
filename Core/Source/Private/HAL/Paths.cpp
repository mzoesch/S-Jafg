// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "HAL/Paths.h"
#include "HAL/PlatformMisc.h"

TChar Paths::GetNormalPathSeparator()
{
    return '/';
}

String Paths::Normalize(const String& Path)
{
    String Out;
    for (const TChar& Char : Path)
    {
        if (Char == '\\')
        {
            Out += GetNormalPathSeparator();
        }
        else
        {
            Out += Char;
        }

        continue;
    }

    return Out;
}

String Paths::Combine(const String& A, const String& B)
{
    String Out = Normalize(A);
    if (Out.back() != GetNormalPathSeparator())
    {
        Out += GetNormalPathSeparator();
    }

    return Out + Normalize(B);
}

String Paths::Combine(const String& A, const String& B, const bool bPreventNormalization)
{
    if (bPreventNormalization)
    {
        if (A.back() != GetNormalPathSeparator())
        {
            return A + GetNormalPathSeparator() + B;
        }

        return A + B;
    }

    return Combine(A, B);
}

String Paths::Combine(const String& A, const String& B, const bool bPreventNormalization, const TChar Separator)
{
    if (bPreventNormalization)
    {
        if (A.back() != Separator)
        {
            return A + Separator + B;
        }

        return A + B;
    }

    String NormalA = Normalize(A);
    if (NormalA.back() != Separator)
    {
        NormalA += Separator;
    }

    return NormalA + Normalize(B);
}

String Paths::EngineRootDir()
{
    return Paths::Normalize(PlatformMisc::GetEngineRootDir());
}
