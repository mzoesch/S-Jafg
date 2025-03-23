# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_target(me: Target, args: TargetArgs) -> None:
    me.defines.append('AS_DAEMON')
    return None
