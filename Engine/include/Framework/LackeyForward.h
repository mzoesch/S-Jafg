// Copyright mzoesch. All rights reserved.

#pragma once


#include "Minimal.afx"

namespace Jafg
{

enum struct ELackey
{
    Local, Proxy,
};

class LLocalLackey;
class LProxyLackey;
typedef std::variant<std::monostate, LLocalLackey*, LProxyLackey*> LLackey;

namespace Detail
{

class LLackeyBase;

} /* ~Namespace Detail */

} /* ~Namespace Jafg */
