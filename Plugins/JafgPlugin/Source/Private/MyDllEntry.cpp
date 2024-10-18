// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include <iostream>

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason )
    {
    case DLL_PROCESS_ATTACH:
    {
        // Initialize once for each new process.
        // Return FALSE to fail DLL load.
        std::cout << "DLL_PROCESS_ATTACH" << std::endl;
        break;
    }

    case DLL_THREAD_ATTACH:
    {
        // Do thread-specific initialization.
        std::cout << "DLL_THREAD_ATTACH" << std::endl;
        break;
    }

    case DLL_THREAD_DETACH:
    {
        // Do thread-specific cleanup.
        std::cout << "DLL_THREAD_DETACH" << std::endl;
        break;
    }

    case DLL_PROCESS_DETACH:
    {
        // Perform any necessary cleanup.
        std::cout << "DLL_PROCESS_DETACH" << std::endl;
        break;
    }
    }

    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
