# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_solution(me: Solution, args: SolutionArgs) -> None:
    me.startup = 'Runtime'

    me.use_white_list_modules = False
    me.use_black_list_target  = False

    return None
