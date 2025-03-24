# Copyright mzoesch. All rights reserved.

import re
import Programs.Reflector.ReflectionUtility as ru
from Programs.Meta import Solution


class BufferedPlatform:
    """A platform that has not yet executed its associated python script."""

    def __init__(self, absolute_py_path: str):
        """..."""

        """NEVER MODIFY THIS VARIABLE."""
        self._absolute_py_path = absolute_py_path
        """NEVER MODIFY THIS VARIABLE."""
        self._relative_py_dir: str = ru.abs_to_rel_fancy(ru.get_dir(self._absolute_py_path))
        """NEVER MODIFY THIS VARIABLE."""
        self._name: str = ru.get_base(self._absolute_py_path).split('.')[0]

        return

    def validate(self):
        if self._absolute_py_path == '':
            raise ValueError('Platform path cannot be empty.')
        if self._relative_py_dir == '':
            raise ValueError('Platform path cannot be empty.')
        if self._name == '':
            raise ValueError('Platform name cannot be empty.')
        if bool(re.fullmatch(r"[A-Za-z]+", self._name)) is False:
            raise ValueError(f'Platform name may only contain latin letters. Faulty platform: [{self._name}].')
        return None


class PlatformArgs:
    def __init__(self, solution: Solution):
        self.solution: Solution = solution
        return


class Platform:
    def __init__(self, parent: BufferedPlatform):
        self._parent: BufferedPlatform = parent
        self._loaded: bool = False

        self.defines: list[str] = []
        self.version: str = 'latest'
        self.architecture: str = ''
        self.toolset: str = ''
        self.unity = False
        self.lnk_flags: list[str] = []
        self.build_flags: list[str] = []
        self.target_props_lnk_flags: list[str] = []
        self.target_props_build_flags: list[str] = []
        self.target_props_sfx: str = ''

        return

    def get_name(self) -> str:
        return self._parent._name

    def validate(self) -> None:
        self._parent.validate()
        if self.version == '':
            raise ValueError('Platform version cannot be empty.')
        if self.architecture == '':
            raise ValueError('Platform architecture cannot be empty.')

        return None

    def load(self, solution: Solution) -> None:
        if self._loaded:
            raise ValueError('Platform already loaded.')
        self._loaded = True
        args: PlatformArgs = PlatformArgs(solution)
        ru.exec_function(self._parent._absolute_py_path, 'add_platform', self, args)
        return None
