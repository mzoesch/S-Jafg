# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_build_configuration(me: BuildConfiguration, args: BuildConfigurationArgs) -> None:
    me.defines.append('IN_SHIPPING')
    me.runtime = 'Release'
    me.symbols = False
    me.optimize = True

    return None
