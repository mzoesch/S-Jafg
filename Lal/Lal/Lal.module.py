# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_module(me: Module, args: ModuleArgs) -> None:
    me.pch = PchUsage.ALLOW
    me.pch_content = '#include "CoreAfx.h"'
    me.kind = ModuleKind.STATIC

    if 'WITH_TESTS' in args.target.defines:
        me.public_dependencies.append('~Engine/TesterForward')

    return None
