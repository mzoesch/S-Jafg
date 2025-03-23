# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_target(me: Target, args: TargetArgs) -> None:
    me.defines.extend(['AS_CLIENT', 'WITH_TESTS'])
    return None
