# Copyright mzoesch. All rights reserved.

import re
import Programs.Reflector.ReflectionUtility as ru
from typing import List
from Programs.Meta.Solution import Solution
from Programs.Meta.Platform import Platform
from Programs.Meta.BuildConfiguration import BuildConfiguration


class BufferedTarget:
    """A target that has not yet executed its associated python script."""

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
            raise ValueError('Target path cannot be empty.')
        if self._relative_py_dir == '':
            raise ValueError('Target path cannot be empty.')
        if self._name == '':
            raise ValueError('Target name cannot be empty.')
        if bool(re.fullmatch(r'[A-Za-z_]+', self._name)) is False:
            raise ValueError(f'Target name may only contain latin letters. Faulty target: [{self._name}].')
        return None

    def get_unique_name(self):
        return f'{self._relative_py_dir}/{self._name}'


class TargetArgs:
    def __init__(self, solution: Solution, platform: Platform, build_configuration: BuildConfiguration):
        self.solution: Solution = solution
        self.platform: Platform = platform
        self.build_configuration: BuildConfiguration = build_configuration
        return

class Target:
    """A fully qualified target."""

    def __init__(self, parent: BufferedTarget):
        self._parent: BufferedTarget = parent
        self._loaded: bool = False

        self.defines: List[str] = []

        return

    def validate(self) -> None:
        self._parent.validate()
        return None

    def load(self, solution: Solution, platform: Platform, build_configuration: BuildConfiguration) -> None:
        if self._loaded:
            raise ValueError('Solution already loaded.')
        self._loaded = True
        args: TargetArgs = TargetArgs(solution, platform, build_configuration)
        ru.exec_function(self._parent._absolute_py_path, 'add_target', self, args)
        return None
