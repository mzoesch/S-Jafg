# Copyright mzoesch. All rights reserved.

from enum import Enum


class ModuleKind(Enum):

    """This module is shared across other modules."""
    SHARED = 0

    """This module is statically compiled."""
    STATIC = 1

    """This module must define an entry point to link against."""
    LAUNCH = 2


def module_kind_to_string(module_kind: ModuleKind) -> str:
    if module_kind == ModuleKind.SHARED:
        return "Shared"

    if module_kind == ModuleKind.STATIC:
        return "Static"

    if module_kind == ModuleKind.LAUNCH:
        return "Launch"

    return "Unknown"
