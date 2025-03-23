# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_module(me: Module, args: ModuleArgs) -> None:
    me.pch = PchUsage.ALLOW
    me.kind = ModuleKind.LAUNCH

    me.public_dependencies.append('~Engine/Engine')

    if 'WITH_TESTS' in args.target.defines:
        me.public_dependencies.append('~Engine/Tester')

    if args.platform == 'Windows':
        me.entry = 'WIN_MAIN'

    return None
