// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg::TesterForward
{

#ifdef PLATFORM_WASM
    int DontDiscardMe();
#else
    __declspec( dllexport ) int DontDiscardMe();
#endif

} /* ~Namespace Jafg::TesterForward */
