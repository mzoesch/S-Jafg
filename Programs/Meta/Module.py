# Copyright mzoesch. All rights reserved.

import re
import Programs.Reflector.ReflectionUtility as ru
from typing import List
from Programs.Meta.Pch import PchUsage
from Programs.Meta.Solution import Solution
from Programs.Meta.ModuleKind import ModuleKind
from Programs.Meta.Platform import Platform
from Programs.Meta.BuildConfiguration import BuildConfiguration
from Programs.Meta.Target import Target


class BufferedModule:
    """A module that has not yet executed its associated python script."""

    def __init__(self, absolute_solution_py_path: str):
        """..."""

        """NEVER MODIFY THIS VARIABLE."""
        self._absolute_py_path = ru.normalize_path(absolute_solution_py_path)
        """NEVER MODIFY THIS VARIABLE."""
        self._relative_py_dir = ru.abs_to_rel_fancy(ru.get_dir(self._absolute_py_path))
        """NEVER MODIFY THIS VARIABLE."""
        self._name: str = ru.get_base(absolute_solution_py_path).split('.')[0]

        return

    def validate(self):
        if self._absolute_py_path == '':
            raise ValueError('Module path cannot be empty.')
        if self._relative_py_dir == '':
            raise ValueError('Module path cannot be empty.')
        if self._name == '':
            raise ValueError('Module name cannot be empty.')
        if bool(re.fullmatch(r"[A-Za-z]+", self._name)) is False:
            raise ValueError(f'Module name may only contain latin letters. Faulty Module: [{self._name}].')
        return None


class ModuleArgs:
    def __init__(self, solution: Solution, platform: Platform, build_configuration: BuildConfiguration, target: Target):
        self.solution: Solution = solution
        self.platform: Platform = platform
        self.build_configuration: BuildConfiguration = build_configuration
        self.target: Target = target
        return


class Module:
    """A fully qualified module."""

    def __init__(self, parent: BufferedModule):
        self._parent: BufferedModule = parent
        self._loaded: bool = False

        """What the user will see when selecting the module. Okay to modify."""
        self.friendly_name: str = Module._convert_to_readable(self._parent._name.replace('_', ' '))

        """The pch to use for this module only. Ok to modify."""
        self.pch: PchUsage = PchUsage.ALLOW
        self.pch_content: str = '#include "CoreAfx.h"'

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

        """
        Private native include dirs and dependencies. Can be anything the compiler / linker understands.
        Only private exists and not public so that these dependencies are never transitively included.
        This is by design as you should always build an interface for native deps.
        Paths are relative to the top level engine root dir.
        """
        self.native_includes: List[str] = []
        self.native_dependencies: List[str] = []

        """
        Files that always have to be in the same dir (not subdir) when the lib / app is being launched.
        """
        self.native_runtime_dependencies: List[str] = []

        return

    def validate(self) -> None:
        self._parent.validate()
        return None

    def load(self, solution: Solution, platform: Platform, build_configuration: BuildConfiguration, target: Target) -> None:
        if self._loaded:
            raise ValueError('Solution already loaded.')
        self._loaded = True
        args: ModuleArgs = ModuleArgs(solution, platform, build_configuration, target)
        ru.exec_function(self._parent._absolute_py_path, 'add_module', self, args)
        return None

    @staticmethod
    def _convert_to_readable(unreadable_str: str) -> str:
        string = unreadable_str.replace('_', ' ')
        string = re.sub(r'([a-z])([A-Z])', r'\1 \2', string)
        return string.title()
