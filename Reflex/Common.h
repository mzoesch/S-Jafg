// Copyright mzoesch. All rights reserved.

#pragma once

#include <string>

namespace Reflex
{

inline bool StrStartsWith(std::string const& Str, std::string const& Prefix)
{
    return Str.size() >= Prefix.size() && Str.compare(0, Prefix.size(), Prefix) == 0;
}

inline bool StrEndsWith(std::string const& Str, std::string const& Suffix)
{
    if (Str.length() >= Suffix.length())
    {
        return 0 == Str.compare(Str.length() - Suffix.length(), Suffix.length(), Suffix);
    }

    return false;
}

} /* ~Namespace Jafg */
