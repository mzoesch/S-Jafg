// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"

#if PLATFORM_WASM

#include "Hal/PlatformWasm.h"

void LWasmPlatformBreakDefines::OnProgramPanic(const char* InMessage, const char* InFile, LWasmPlatformTypes::int32 InLine)
{
    const std::string Out = std::format("Message: %s;\nFile: %s;\nLine: %d;\n", InMessage, InFile, InLine);
    emscripten_log(EM_LOG_ERROR | EM_LOG_C_STACK, Out.c_str());

    abort();
}

namespace PlatformHal
{

void Sleep(const double InSeconds)
{
    emscripten_sleep(static_cast<uint32>(InSeconds * JAFG_S2MS_D));
}

} /* ~Namespace PlatformHal */

#endif /* PLATFORM_WASM */
