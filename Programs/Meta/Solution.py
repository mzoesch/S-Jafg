# Copyright mzoesch. All rights reserved.

import re
import Programs.Reflector.ReflectionUtility as ru
from typing import List


class BufferedSolution:
    """A solution that has not yet executed its associated python script."""

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


class SolutionArgs:
    ...


class Solution:
    """A fully qualified solution."""

    def __init__(self, parent: BufferedSolution):
        self._parent: BufferedSolution = parent
        self._loaded: bool = False

        self.startup: str = ''
        self.use_white_list_modules: bool = False
        self.white_list_modules: List[str] = []
        self.use_black_list_target: bool = False
        self.black_list_targets: List[str] = []

        return

    def validate(self) -> None:
        self._parent.validate()
        return None

    def load(self) -> None:
        if self._loaded:
            raise ValueError('Solution already loaded.')
        self._loaded = True
        args: SolutionArgs = SolutionArgs()
        ru.exec_function(self._parent._absolute_py_path, 'add_solution', self, args)
        return None

    def does_blacklist_target(self, rel_target: str, name_target) -> bool:
        if self.use_black_list_target is False:
            return False
        if rel_target in self.black_list_targets:
            return True
        if name_target in self.black_list_targets:
            return True
        return False

    def does_whitelist_module(self, rel_module: str, name_module: str) -> bool:
        if self.use_white_list_modules is False:
            return True
        if rel_module in self.white_list_modules:
            return True
        if name_module in self.white_list_modules:
            return True
        return False
