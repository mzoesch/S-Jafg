# Copyright mzoesch. All rights reserved.

from Programs.Meta import *

def apply_policies(in_const_target: Target, me: Module) -> None:
    me.pch = PchUsage.PROHIBIT
    me.kind = ModuleKind.SHARED

    me.public_dependencies.append('Lal')

    return None

