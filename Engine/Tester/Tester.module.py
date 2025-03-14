# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_module(me: Module, args: ModuleArgs) -> None:
    me.pch = PchUsage.PROHIBIT
    me.kind = ModuleKind.SHARED

    me.public_dependencies.extend(['~Lal/Lal', '~Engine/TesterForward'])

    return None
