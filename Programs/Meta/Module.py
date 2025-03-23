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


#   Exceptions off?
#   Unity build here?
class Module:
    """
    A fully qualified module.

    Things to consider:
      RTTI is always off in any configuration for all modules.
    """

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
        Entry for a module: [MAIN, WIN_MAIN].
        """
        self.entry: str = 'MAIN'

        """
        Pre build steps.
        Expanded variables: 
          $PythonExecutable     The absolute path to the venv of the jafg python executable.
          $PythonScript         The relative path to the entry point of the jafg python script.
          $RootDir              The absolute path to the engine root.
          $Kind                 The kind of the module it is currently compiled for.
          $Platform             The target platform to compile for.
          $Arch                 The target architecture to compile for.
          $Target               The target to compile for.
        """
        self.pre_builds: List[str] = []

        """
        Post build steps.
        See #pre_builds for expanded variables.
        """
        self.post_builds: List[str] = []

        """
        Public module dependencies. These modules are transitively linked.
        Automatically includes the public directories of said dependencies and links against them.
        Specify either the module name or module relative path to the engine root, e.g. '~Engine/Engine' or 'Engine'.
        """
        self.public_dependencies: List[str] = []

        """
        Private module dependencies. These modules are intransitively linked.
        Automatically includes the public directories of said dependencies and links against them.
        Specify either the module name or module relative path to the engine root,
        e.g. '~Engine/EngineForward' or 'EngineForward'.
        """
        self.private_dependencies: List[str] = []

        """
        Native include paths. Can be anything the compiler / linker understands.
        These includes are intransitively included and therefore always private.
        This is by design as you should always build an interface for native deps.
        """
        self.native_includes: List[str] = []

        """
        Native dependencies that are used during linking. Can be anything the linker understands.
        These dependencies are intransitively included and therefore always private.
        This is by design as you should always build an interface for native deps.
        """
        self.native_dependencies: List[str] = []

        """
        Native runtime dependencies that are used during linking and at runtime. Path has to be relative to the
        to level engine root. E.g. 'Engine/Vendor/Lib/MySharedLib.dylib'.
        """
        self.native_runtime_dependencies: List[str] = []

        """
        Native additional runtime dependencies that are only used at runtime. Path has to be relative to the
        to level engine root. E.g. 'Engine/Vendor/Lib/MySharedLib.pdb'.
        """
        self.native_additional_runtime_dependencies: List[str] = []

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

    def _add_default_flags(self, solution: Solution, platform: Platform, build_configuration: BuildConfiguration, target: Target) -> None:
        if len(self.pre_builds) > 0 or len(self.post_builds) > 0:
            raise ValueError('Expected zero but got >0.')

        self.pre_builds.append(f'$PythonExecutable $PythonScript -INVOKE -- --BuildTool pre-build SLN={solution._parent._name} MODULE={self._parent._relative_py_dir} KIND=$Kind PLATFORM=$Platform ARCH=$Arch TARGET=$Target')
        self.post_builds.append(f'$PythonExecutable $PythonScript -INVOKE -- --BuildTool post-build SLN={solution._parent._name} MODULE={self._parent._relative_py_dir} KIND=$Kind PLATFORM=$Platform ARCH=$Arch TARGET=$Target')

        return None

    @staticmethod
    def _convert_to_readable(unreadable_str: str) -> str:
        string = unreadable_str.replace('_', ' ')
        string = re.sub(r'([a-z])([A-Z])', r'\1 \2', string)
        return string.title()
