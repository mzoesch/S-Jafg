// Copyright mzoesch. All rights reserved.

#include "Minimal.afx"

#if JAFG_PLATFORM_DESKTOP

#if JAFG_PLATFORM_LINUX
    #include <cstdlib>
#endif /* JAFG_PLATFORM_LINUX */

#include "Framework/Frontend.h"

void Jafg::LFrontendNativeDesktop::OpenDirectory(LPath const& Directory) const
{
#if JAFG_PLATFORM_LINUX
    LString Cmd{"xdg-open \"" + absolute(Directory).native() +"\""};
    LOG_VERBOSE(LogSystem, "Executing: {}", Cmd)
    std::system(Cmd.c_str());
#elif JAFG_PLATFORM_WINDOWS
    LOG_VERBOSE(LogSystem, "Executing: open {}", absolute(Directory))
    ShellExecute(nullptr
        , LITERAL_TEXT("open")
            , absolute(Directory).native().c_str()
            , nullptr
        , nullptr
        , SW_SHOWDEFAULT
        );
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
    for (auto* Term: Terms)
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
    LOG_VERBOSE(LogSystem, "Executing: open cmd.exe /K cd /d \"{}\"", absolute(Directory))
    ShellExecute(nullptr
        , LITERAL_TEXT("open")
            , LITERAL_TEXT("cmd.exe")
            , (LITERAL_TEXT("/K cd /d \"") + absolute(Directory).native() + LITERAL_TEXT("\"")).c_str()
        , nullptr
        , SW_SHOWDEFAULT
        );
#else /* JAFG_PLATFORM_WINDOWS */
    #error Missing implementation.
#endif /* !JAFG_PLATFORM_WINDOWS */
}

void Jafg::LFrontendNativeDesktop::OpenUrl(LStringView Url) const
{
#if JAFG_PLATFORM_LINUX
    LString Cmd{"xdg-open \"" + LString{Url} + "\""};
    LOG_VERBOSE(LogSystem, "Executing: {}", Cmd)
    std::system(Cmd.c_str());
#elif JAFG_PLATFORM_WINDOWS
    LOG_VERBOSE(LogSystem, "Executing: open {}", Url)
    ShellExecute(nullptr
        , LITERAL_TEXT("open")
            , algo::utf8_to_utf16(Url).c_str()
            , nullptr
        , nullptr
        , SW_SHOWDEFAULT
        );
#else /* JAFG_PLATFORM_WINDOWS */
    #error Missing implementation.
#endif /* !JAFG_PLATFORM_WINDOWS */
}

#endif /* JAFG_PLATFORM_DESKTOP */
