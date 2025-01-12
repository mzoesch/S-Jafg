# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def apply_policies(in_const_target: Target, me: Module) -> None:
    me.pch = PchUsage.ALLOW
    me.kind = ModuleKind.LAUNCH

    me.public_dependencies.append('Engine')

    if 'WITH_TESTS' in in_const_target.defines:
        me.public_dependencies.append('Tester')

    return None
