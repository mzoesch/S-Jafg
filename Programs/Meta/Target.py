# Copyright mzoesch. All rights reserved.

import Programs.Reflector.ReflectionUtility as ru
from Programs.Meta.BuildConfig import BuildConfiguration, build_configuration_to_string, build_configuration_to_define

from typing import List


class Target:
    def __init__(self, absolute_target_py_path: str):
        """..."""

        """NEVER MODIFY THIS VARIABLE."""
        self._absolute_py_path: str = absolute_target_py_path
        """NEVER MODIFY THIS VARIABLE."""
        self._relative_py_dir: str = ru.abs_to_rel_fancy(ru.get_dir(self._absolute_py_path))
        """NEVER MODIFY THIS VARIABLE."""
        self._name: str = ru.get_base(self._absolute_py_path).split('.')[0]

        """
        The defines for this target. All defines are treated as if they were public.
        """
        self.defines: List[str] = []

        """NEVER MODIFY THIS VARIABLE. Only valid in module policies apply-er functions."""
        self.volatile_build_configuration: BuildConfiguration = BuildConfiguration.UNKNOWN

        return

    def get_unique_name(self):
        return f'{self._relative_py_dir}/{self._name}'

    def get_volatile_identifier(self) -> str:
        if self.volatile_build_configuration == BuildConfiguration.UNKNOWN:
            raise ValueError('Tried to access volatile identifier without volatile build configuration.')
        return f'{self._name}-{build_configuration_to_string(self.volatile_build_configuration)}'

    def get_volatile_defines(self) -> List[str]:
        if self.volatile_build_configuration == BuildConfiguration.UNKNOWN:
            raise ValueError('Tried to access volatile define without volatile build configuration.')

        copied_defines: List[str] = list()
        copied_defines.append(build_configuration_to_define(self.volatile_build_configuration))
        for define_name in self.defines:
            copied_defines.append(define_name)
        return copied_defines
