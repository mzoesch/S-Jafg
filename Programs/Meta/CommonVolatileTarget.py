# Copyright mzoesch. All rights reserved.

from Programs.Meta import *

def set_common_volatile_target_values(me: Target) -> None:
    if me.volatile_build_configuration == BuildConfiguration.DEBUG:
        me.volatile_runtime = 'Debug'
        me.volatile_optimize = False
        me.volatile_symbols = True
    elif me.volatile_build_configuration == BuildConfiguration.DEVELOPMENT:
        me.volatile_runtime = 'Release'
        me.volatile_optimize = True
        me.volatile_symbols = True
    elif me.volatile_build_configuration == BuildConfiguration.SHIPPING:
        me.volatile_runtime = 'Release'
        me.volatile_optimize = True
        me.volatile_symbols = False
    else:
        raise ValueError('Build configuration is not volatile.')

    return None
