# Copyright mzoesch. All rights reserved.

import re
from .Pch import PchUsage
from .ModuleKind import ModuleKind
from Programs.Reflector.ReflectionUtility import get_base as ru_get_base
from typing import List
import Programs.Reflector.ReflectionUtility as ru


class Module:
    def __init__(self, absolute_py_path: str):
        """..."""

        """NEVER MODIFY THIS VARIABLE."""
        self._absolute_py_path: str = absolute_py_path
        """NEVER MODIFY THIS VARIABLE."""
        self._relative_py_dir: str = ru.abs_to_rel_fancy(ru.get_dir(self._absolute_py_path))
        """NEVER MODIFY THIS VARIABLE."""
        self._name: str = ru.get_base(self._absolute_py_path).split('.')[0]
        """Okay to modify if needed. This is what the user will see when selecting the module to load."""
        self.friendly_name: str = Module._convert_to_readable(self._name.replace('_', ' '))

        """The pch to use for this module only."""
        self.pch: PchUsage = PchUsage.ALLOW
        self.pch_content: str = ''

        """How the compiler and linker should treat this module."""
        self.kind: ModuleKind = ModuleKind.SHARED

        """
        Public dependencies. This dependencies are transitively linked.
        Either the module name, module relative path to the engine root or the relative path
        to a static library / shared dynamically linked library.
        """
        self.public_dependencies: List[str] = []

        """
        Private dependencies. This dependencies are intransitively linked.
        Either the module name, module relative path to the engine root or the relative path
        to a static library / shared dynamically linked library.
        """
        self.private_dependencies: List[str] = []

        return

    def get_unique_name(self) -> str:
        return self._relative_py_dir

    @staticmethod
    def _convert_to_readable(unreadable_str: str) -> str:
        string = unreadable_str.replace('_', ' ')
        string = re.sub(r'([a-z])([A-Z])', r'\1 \2', string)
        return string.title()

    def _reset_volatile_fields(self) -> None:
        self.pch = PchUsage.ALLOW
        self.kind = ModuleKind.SHARED
        self.public_dependencies = []
        self.private_dependencies = []

        return None
