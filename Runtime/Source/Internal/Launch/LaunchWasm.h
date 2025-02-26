// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

#if !PLATFORM_WASM
    #error "This header file is only for WebAssembly builds."
#endif /* !PLATFORM_WASM */

PRAGMA_FOR_JAFG_BUILD_TOOL("MakeStaticClassContainer")
