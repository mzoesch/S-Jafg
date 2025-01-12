# Copyright mzoesch. All rights reserved.

from Programs.Meta import *

def add_solution(me: Solution) -> None:
    me.use_white_list_modules = True
    me.white_list_modules.extend([
        "~Lal/Lal",
        "~Lal/LalTestRuntime",
    ])

    me.use_black_list_target = True
    me.black_list_targets.extend([
        '~Runtime/Client',
        '~Runtime/Daemon'
    ])

    return None
