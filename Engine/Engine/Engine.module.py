# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def apply_policies(in_const_target: Target, me: Module) -> None:
    me.pch = PchUsage.ALLOW
    me.pch_content = '#include "CoreAfx.h"'
    me.kind = ModuleKind.SHARED

    me.public_dependencies.append('Lal')
    me.private_dependencies.append('EngineForward')

    if 'WITH_TESTS' in in_const_target.defines:
        me.public_dependencies.append('TesterForward')

    return None
