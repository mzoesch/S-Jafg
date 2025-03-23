# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_platform(me: Platform, args: PlatformArgs) -> None:
    me.defines.extend(['PLATFORM_LINUX', 'WITH_CLANG'])
    me.architecture = 'x86_64'
    me.toolset = 'clang'

    me.build_flags.append('-fno-rtti')

    return None
