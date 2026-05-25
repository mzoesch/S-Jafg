// Copyright mzoesch. All rights reserved.

#if JAFG_PLATFORM_WASM

#include "Hal/PlatformWasm.h"

void LWasmPlatformBreakDefines::OnProgramPanic(const char* InMessage, const char* InFile, LWasmPlatformTypes::i32 InLine)
{
    ::emscripten_cancel_main_loop();

    const std::string Out = std::format("Message: %s;\nFile: %s;\nLine: %d;\n", InMessage, InFile, InLine);
    emscripten_log(EM_LOG_ERROR | EM_LOG_C_STACK, Out.c_str());

    abort();
}

namespace Jafg::PlatformHal
{

void SleepNoStats(const double InSeconds)
{
    panic( "Sleeping is not allowed in wasm." )
}

} /* ~Namespace Jafg::PlatformHal */

#endif /* JAFG_PLATFORM_WASM */
