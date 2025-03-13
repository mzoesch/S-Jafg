# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_solution(me: Solution, args: SolutionArgs) -> None:
    me.startup = 'LalTestRuntime'

    me.use_white_list_modules = True
    me.white_list_modules.extend([
        "~Engine/Tester",
        "~Engine/TesterForward",
        "~Lal/Lal",
        "~Lal/LalTestRuntime",
    ])

    me.use_black_list_target = True
    me.black_list_targets.extend([
        '~Runtime/Client',
        '~Runtime/Daemon'
    ])

    return None
