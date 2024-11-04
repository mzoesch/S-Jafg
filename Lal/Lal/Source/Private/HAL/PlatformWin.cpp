// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"

#if PLATFORM_WINDOWS

#include "HAL/PlatformWin.h"

void LWinPlatformBreakDefines::OnProgramPanic(
    const std::string& InMessage,
    const std::string& InFile,
    const int32_t InLine
)
{
    const std::string  InCaption     = "Jafg panicked";
    const std::wstring InCaptionWide = std::wstring(InCaption.begin(), InCaption.end());

    std::string InMessageWithAdditionalInfo;
    InMessageWithAdditionalInfo += "Jafg entered an one-way enclosing block inside a critical control path and panicked.\n\n";
    InMessageWithAdditionalInfo += "File: ";
    InMessageWithAdditionalInfo += InFile;
    InMessageWithAdditionalInfo += "\nLine: ";
    InMessageWithAdditionalInfo += std::to_string(InLine);
    InMessageWithAdditionalInfo += "\n\nExpression: ";
    InMessageWithAdditionalInfo += InMessage;

    const std::wstring InMessageWide = std::wstring(InMessageWithAdditionalInfo.begin(), InMessageWithAdditionalInfo.end());

#if IN_SHIPPING
    /*
     * We only need this in shipping because abort will ask the debugger, if attached, to load the memory dump.
     */
    MessageBox(nullptr, InMessageWide.c_str(), InCaptionWide.c_str(), MB_ICONERROR | MB_OK);
#endif /* IN_SHIPPING */

    /*
         * To we actually want to call abort?
         * Maybe we want to call a delegate here to allow for a custom panic handler for the most
         * critical systems before aborting.
         * E.g., close SQL handlers. Nothing would be more stupid if the program crashes
         * and our db is corrupted. At least for the user.
         */
    abort();
}

#endif /* PLATFORM_WINDOWS */
