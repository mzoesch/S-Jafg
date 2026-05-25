// Copyright mzoesch. All rights reserved.

#include "Minimal.afx"

#if JAFG_PLATFORM_DESKTOP

#if JAFG_PLATFORM_LINUX
    #include <cstdlib>
#endif

#include "Framework/Frontend.h"

void Jafg::LFrontendNativeDesktop::OpenDirectory(LPath const& Directory) const
{
#if JAFG_PLATFORM_LINUX
    LString Cmd{"xdg-open \"" + absolute(Directory).native() +"\""};
    LOG_VERBOSE(LogSystem, "Executing: {}", Cmd)
    std::system(Cmd.c_str());
#elif JAFG_PLATFORM_WINDOWS
    #error Missing implementation.
#else /* JAFG_PLATFORM_WINDOWS */
    #error Missing implementation.
#endif /* !JAFG_PLATFORM_WINDOWS */
}

// TODO: User prefs for preferred terminal.
void Jafg::LFrontendNativeDesktop::OpenTerminal(LPath const& Directory) const
{
#if JAFG_PLATFORM_LINUX
    char const* Terms[]
    {
        "konsole",
        "x-terminal-emulator",
        "gnome-terminal",
        "xfce4-terminal",
        "kitty",
        "alacritty",
        "wezterm",
        "xterm"
    };
    for (auto* Term : Terms)
    {
        auto Cmd{algo::sprintf("which {} > /dev/null 2>&1 && cd \"{}\" && {} &"
            , Term, absolute(Directory).native(), Term)};
        LOG_VERBOSE(LogSystem, "Executing: {}", Cmd)
        if (!std::system(Cmd.c_str()))
        {
            break;
        }
    }
#elif JAFG_PLATFORM_WINDOWS
    #error Missing implementation.
#else /* JAFG_PLATFORM_WINDOWS */
    #error Missing implementation.
#endif /* !JAFG_PLATFORM_WINDOWS */
}

#endif /* JAFG_PLATFORM_DESKTOP */
