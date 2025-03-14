# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_platform(me: Platform, args: PlatformArgs) -> None:
    me.defines.extend(['PLATFORM_WASM'])
    me.architecture = 'x86'
    me.toolset = 'gcc'

    me.build_flags.append('-fno-rtti')

    return None
