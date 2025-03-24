# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_platform(me: Platform, args: PlatformArgs) -> None:
    me.defines.extend(['PLATFORM_WASM'])
    me.architecture = 'x86'

    # !!EM!!! but premake does not support it now. Only some random legacy versions
    me.toolset = 'em'; # Emulate with gcc; Just for intellisense - not compiling

    me.unity = True

    me.build_flags.append('-fno-rtti')

    me.target_props_lnk_flags.extend([
        '-sMIN_WEBGL_VERSION=2',
        '-sMAX_WEBGL_VERSION=2',
        '-sUSE_FREETYPE=1',
        '-sUSE_PTHREADS=1',
        '-sPTHREAD_POOL_SIZE_STRICT=2',
        '-sPTHREAD_POOL_SIZE=3',
        '-sALLOW_MEMORY_GROWTH',
        '--shell-file',
        '${REAL_ENGINE_ROOT_DIR}/Content/Wasm/MinimalShell.html',
    ])
    me.target_props_build_flags.append('-pthread')
    me.target_props_sfx = '.html'

    return None
