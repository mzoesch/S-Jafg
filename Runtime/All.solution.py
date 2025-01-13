# Copyright mzoesch. All rights reserved.

from Programs.Meta import *
from Programs.Meta.CommonSolution import set_common_sln_values


def add_solution(me: Solution) -> None:
    set_common_sln_values(me)

    me.startup = 'Runtime'

    me.use_white_list_modules = False
    me.use_black_list_target = False

    return None
