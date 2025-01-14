# Copyright mzoesch. All rights reserved.

from Programs.Meta import *

def apply_policies(in_const_target: Target, me: Module) -> None:
    me.pch = PchUsage.ALLOW
    me.pch_content = '#include "CoreAfx.h"'
    me.kind = ModuleKind.LAUNCH

    me.public_dependencies.extend([
        'Lal',
        'TesterForward',
        'Tester',
    ])

    return None
