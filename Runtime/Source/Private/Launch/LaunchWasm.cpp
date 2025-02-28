// Copyright mzoesch. All rights reserved.

#if PLATFORM_WASM

#include "Core/Application.h"
#include "Launch/LaunchWasm.h"
#include "Engine/Engine.h"
#include "Async/TaskUtility.h"

using namespace Jafg;

namespace
{

bool bRunExit = false;

} /* ~Namespace <Anonymous> */

extern auto GuardedMain() -> EPlatformExit::Type;
extern void EngineTick();
extern void EngineExit();

void WasmGuardedLoop()
{
    check( Tasks::IsOnMasterThread() )

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

    /*
     * Also here. We should call JavaScript to handle console events. We then just forward JavaScript-Console In
     * to the CLI of the engine and give back the reply to the browser commandline.
     */
    ::EngineTick();

    return;
}

int32 main(int32 Argc, char* Argv[])
{
    /*
     * This is kinda stupid. When serving over the Internet, we cannot get the command line arguments.
     * So we should use the URL query string to pass the command line arguments.
     * For a more future problem.
     */
    LString CmdLine;
    for (int32 Index = 0; Index < Argc; ++Index)
    {
        CmdLine += Argv[Index];
        CmdLine += " ";
        continue;
    }

    Application::Private::CommandLine = std::move(CmdLine);
    Application::Private::bDebuggerPresent = false; // No debugger is present in the browser.
    Application::Private::UpdateApplicationCommandLineVariables();
    Application::Private::bAlwaysReportCrash = false; // We do not want to report crashes in the browser.
                                                      // This is the responsibility of JavaScript.

    const EPlatformExit::Type ErrorLevel = GuardedMain();

    if (bRunExit == false) // If the engine ticked at least for one tick, this control path is unreachable.
    {                      // Emscripten will not make a clean exit - call destructors, etc.
        bRunExit = true;   // When the engine started ticking, we can only exit the engine through a throw.
                           // So this is unsafe - but who cares. This is not a native build anyways.
        check( Tasks::IsOnMasterThread() )
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
