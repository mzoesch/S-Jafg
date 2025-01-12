# Copyright mzoesch. All rights reserved.

from Programs.Meta import *

def add_solution(me: Solution) -> None:
    me.use_white_list_modules = True
    me.white_list_modules.extend([
        '~Engine/Engine',
        '~Engine/EngineForward',
        '~Engine/Tester',
        '~Engine/TesterForward',
        '~Lal/Lal',
        '~Runtime'
    ])

    me.use_black_list_target = False

    return None
