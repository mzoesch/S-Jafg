# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_module(me: Module, args: ModuleArgs) -> None:
    me.pch = PchUsage.ALLOW
    me.pch_content = '#include "CoreAfx.h"'
    me.kind = ModuleKind.LAUNCH

    me.public_dependencies.append('~Lal/Lal')

    if 'WITH_TESTS' in args.target.defines:
        me.public_dependencies.append('~Engine/Tester')

    return None
