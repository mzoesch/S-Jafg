# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_platform(me: Platform, args: PlatformArgs) -> None:
    me.defines.extend(['PLATFORM_WASM'])
    me.architecture = 'x86'

    # !!EM!!! but premake does not support it now. Only some random legacy versions
    me.toolset = 'em'; # Emulate with gcc; Just for intellisense - not compiling 

    me.build_flags.append('-fno-rtti')

    return None
