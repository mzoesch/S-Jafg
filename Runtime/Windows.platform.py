# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_platform(me: Platform, args: PlatformArgs) -> None:
    me.defines.extend(['PLATFORM_WINDOWS', 'PLATFORM_WINDOWS_WITH_MSVC'])
    return None
