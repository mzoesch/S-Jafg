# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_platform(me: Platform, args: PlatformArgs) -> None:
    me.defines.extend(['PLATFORM_WINDOWS', 'PLATFORM_WINDOWS_WITH_MSVC'])
    me.architecture = 'x86_64'
    me.toolset = 'msc'

    me.lnk_flags.extend([
        '/SUBSYSTEM:WINDOWS',
        '/NODEFAULTLIB:LIBCMT',
        '/NODEFAULTLIB:MSVCRT',
    ])
    me.build_flags.extend([
        '/Zc:__cplusplus',
        '/GR-',
        '/MP',
    ])

    return None
