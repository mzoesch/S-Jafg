// Copyright mzoesch. All rights reserved.

#if PLATFORM_WASM

#include "CoreAfx.h"
#include "Launch/LaunchWasm.h"
#include "Engine/Engine.h"
#include "Async/TaskUtility.h"
#include "System/VFilesystem.h"
#include "LaunchWasm.generated.h"

namespace
{

bool bRunExit = false;

} /* ~Namespace <Anonymous> */

extern auto GuardedMain(const char* CmdLine) -> EPlatformExit::Type;
extern void EngineTick();
extern void EngineExit();

void WasmGuardedLoop()
{
    check( Jafg::Tasks::IsOnMasterThread() )

    if (::Jafg::IsTearingDown())
    {
        LOG_INFO(LogGuardedMain, "Canceling main loop ...")
        ::emscripten_cancel_main_loop();
        if (bRunExit == false)
        {
            bRunExit = true;
            ::EngineExit();
        }
        return;
    }

    ::EngineTick();

    return;
}

int32 main(int32 Argc, char* Argv[])
{
    /*
     * Nothing to guard here. The Java Script will handle that for us.
     */
    const EPlatformExit::Type ErrorLevel = GuardedMain(nullptr); // Unused command line arguments. Maybe we can use
                                                                 // arguments in the future through parsing them from
                                                                 // the unified resource locator.

    if (bRunExit == false) // If the engine ticked at least for one tick, this control path is unreachable.
    {                      // Emscripten will not make a clean exit - call destructors, etc.
        bRunExit = true;   // When the engine started ticking, we can only exit the engine through a throw.
                           // So this is unsafe - but who cares. This is not a native build anyways.
        check( Jafg::Tasks::IsOnMasterThread() )
        LOG_WARNING(LogSystem, "Engine did not tick at least once. Exiting the engine ...")
        ::EngineExit();
    }

    if (ErrorLevel != EPlatformExit::Success)
    {
        LOG_ERROR(LogGuardedMain, "Wasm error occurred. Error level: {}.", static_cast<int32>(ErrorLevel))
    }

    return static_cast<int32>(ErrorLevel);
}

#endif /* PLATFORM_WASM */
