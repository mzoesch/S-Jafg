# Copyright mzoesch. All rights reserved.

import re
import Programs.Reflector.ReflectionUtility as ru
from typing import List
from Programs.Meta.Solution import Solution
from Programs.Meta.Platform import Platform


class BufferedBuildConfiguration:
    """A build configuration that has not yet executed its associated python script."""

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
            raise ValueError('Solution path cannot be empty.')
        if self._relative_py_dir == '':
            raise ValueError('Solution path cannot be empty.')
        if self._name == '':
            raise ValueError('Solution name cannot be empty.')
        if bool(re.fullmatch(r"[A-Za-z]+", self._name)) is False:
            raise ValueError(f'Solution name may only contain latin letters. Faulty solution: [{self._name}].')
        return None


class BuildConfigurationArgs:
    def __init__(self, solution: Solution, platform: Platform):
        self.solution: Solution = solution
        self.platform: Platform = platform
        return


class BuildConfiguration:
    """A fully qualified build configuration."""

    def __init__(self, parent: BufferedBuildConfiguration) -> None:
        self._parent = parent
        self._loaded: bool = False

        self.defines: List[str] = []

        return

    def get_name(self) -> str:
        return self._parent._name

    def validate(self) -> None:
        self._parent.validate()
        return None

    def load(self, solution: Solution, platform: Platform) -> None:
        if self._loaded:
            raise ValueError('Solution already loaded.')
        self._loaded = True
        args: BuildConfigurationArgs = BuildConfigurationArgs(solution, platform)
        ru.exec_function(self._parent._absolute_py_path, 'add_build_configuration', self, args)
        return None
