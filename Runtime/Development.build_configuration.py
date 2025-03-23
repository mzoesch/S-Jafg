# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_build_configuration(me: BuildConfiguration, args: BuildConfigurationArgs) -> None:
    me.defines.append('IN_DEVELOPMENT')
    me.runtime = 'Release'
    me.symbols = True
    me.optimize = True

    return None
