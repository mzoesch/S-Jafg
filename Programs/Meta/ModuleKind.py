# Copyright mzoesch. All rights reserved.

from enum import Enum


class ModuleKind(Enum):
    """
    Describes the different kinds of modules.
    """

    """
    This module is shared across other modules.
    This module has be linked against at compile time.
    """
    SHARED = 0

    """
    This module is statically compiled.
    This module has to be linked against at compile time.
    """
    STATIC = 1

    """
    This module must define an entry point to link against.
    This module should not be linked against.
    """
    LAUNCH = 2

    """
    This module is shared across other modules.
    This module must never be linked against. It can be loaded dynamically at runtime.
    """
    PLUGIN = 3


def module_kind_to_string(module_kind: ModuleKind) -> str:
    if module_kind == ModuleKind.SHARED:
        return "Shared"

    if module_kind == ModuleKind.STATIC:
        return "Static"

    if module_kind == ModuleKind.LAUNCH:
        return "Launch"

    if module_kind == ModuleKind.PLUGIN:
        return "Plugin"

    return "Unknown"
