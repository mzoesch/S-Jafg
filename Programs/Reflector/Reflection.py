# Copyright mzoesch. All rights reserved.

import os
import sys
import json
from Programs.Shared import *
from Programs.Meta.Application import GApp
from Programs.Meta.BuildConfig import BuildConfiguration
from Programs.Meta.Solution import Solution
from Programs.Meta.Target import Target
from Programs.Meta.Module import Module
from Programs.Meta.Pch import PchUsage, volatile_pch_usage_to_string
from Programs.Meta.ModuleKind import ModuleKind, module_kind_to_string
from Programs.Meta.CommonVolatileTarget import set_common_volatile_target_values
from Programs.Reflector import ReflectionUtility as ru

def reflect_all_targets_and_modules() -> None:
    root_dir = str(get_abs_engine_root_dir())
    for entry in ru.get_all_dirs_in_directory(root_dir, True):
        if (GApp.workspace.is_tl_ignored(ru.get_base(entry))
                or GApp.workspace.is_ignored(ru.get_base(entry))):
            continue
        scan_dir_for_targets_and_modules(entry)

    create_cached_program_agnostic_file()

    return None


def scan_dir_for_targets_and_modules(absolute_path: str) -> None:
    print(f'Scanning: {absolute_path}')

    for f_entry in ru.get_all_files_in_directory(absolute_path):
        if '.target.py' not in f_entry:
            continue
        target: Target = Target(os.path.join(absolute_path, f_entry))
        load_target(target, target._absolute_py_path)
        GApp.add_target(target)

    for f_entry in ru.get_all_files_in_directory(absolute_path):
        if '.solution.py' not in f_entry:
            continue
        solution: Solution = Solution(os.path.join(absolute_path, f_entry))
        load_solution(solution, solution._absolute_py_path)
        GApp.add_solution(solution)

    module_init_py: str = f'{ru.get_base(absolute_path)}.module.py'
    if ru.is_path_valid(os.path.join(absolute_path, module_init_py)):
        module: Module = Module(os.path.join(absolute_path, module_init_py))
        GApp.add_module(module)
        return None  # Do not scan further (nested modules are not allowed).

    for entry in ru.get_all_dirs_in_directory(absolute_path, True):
        if GApp.workspace.is_ignored(ru.get_base(entry)):
            continue
        scan_dir_for_targets_and_modules(entry)
    return None


def load_target(target: Target, absolute_path: str) -> None:
    ru.exec_function(absolute_path, 'add_target', target)
    return None


def load_volatile_target(target: Target, absolute_path: str) -> None:
    ru.try_exec_function(absolute_path, 'load_volatile_target', target)
    return None


def load_solution(solution: Solution, absolute_path: str) -> None:
    ru.exec_function(absolute_path, 'add_solution', solution)
    return None


def create_cached_program_agnostic_file() -> None:
    """
    The file that can be read and understood by any program.
    This file defines the structure of the core C++ application.
    """

    if ru.is_path_valid(os.path.join(get_abs_engine_root_dir(), 'Saved')) is False:
        os.mkdir(os.path.join(get_abs_engine_root_dir(), 'Saved'))
    absolute_cached_file_path: str = os.path.join(get_abs_engine_root_dir(), 'Saved', 'structure.jproject')

    if ru.is_path_valid(absolute_cached_file_path):  # Truncate file
        os.truncate(absolute_cached_file_path, 0)
    else:
        with open(absolute_cached_file_path, 'w') as _:
            pass

    data: dict = {'Solutions': []}
    for solution in GApp.solutions:
        apply_solution(solution, data)

    with open(absolute_cached_file_path, 'w') as f:
        json.dump(data, f, indent=2)
        f.write('\n')

    return None


def apply_solution(solution: Solution, out_data: dict) -> None:
    """Apply a config to all targets."""

    out_data['Solutions'].append({})

    sln_cursor = out_data['Solutions'][len(out_data['Solutions']) - 1]
    sln_cursor['RelativePyPath'] = solution._relative_py_dir
    sln_cursor['Name'] = solution._name
    sln_cursor['Startup'] = solution.startup
    sln_cursor['Targets'] = []

    for target in GApp.targets:
        if (solution.use_black_list_target is False
                or solution.does_blacklist_target(target.get_unique_name(), target._name) is False):

            target.volatile_build_configuration = BuildConfiguration.DEBUG
            def_len: int = len(target.defines)
            set_common_volatile_target_values(target)
            load_volatile_target(target, target._absolute_py_path)
            sln_cursor['Targets'].append(apply_target(solution, target))
            target.defines = target.defines[:def_len]

            target.volatile_build_configuration = BuildConfiguration.DEVELOPMENT
            def_len: int = len(target.defines)
            set_common_volatile_target_values(target)
            load_volatile_target(target, target._absolute_py_path)
            sln_cursor['Targets'].append(apply_target(solution, target))
            target.defines = target.defines[:def_len]

            target.volatile_build_configuration = BuildConfiguration.SHIPPING
            def_len: int = len(target.defines)
            set_common_volatile_target_values(target)
            load_volatile_target(target, target._absolute_py_path)
            sln_cursor['Targets'].append(apply_target(solution, target))
            target.defines = target.defines[:def_len]

            target.volatile_build_configuration = BuildConfiguration.UNKNOWN
            target.volatile_runtime = ''
            target.volatile_symbols = False
            target.volatile_optimize = True

    return None


def apply_target(solution: Solution, target: Target) -> dict:
    """Apply a target inside a config."""

    if target.volatile_runtime == '':
        raise ValueError('Runtime is not valid.')

    target_cursor: dict = {
        'Name': target.get_volatile_identifier(),
        'Defines': target.get_volatile_defines(),
        'Runtime': target.volatile_runtime,
        'Symbols': target.volatile_symbols,
        'Optimize': target.volatile_optimize,
        'Modules': []
    }

    for module in GApp.modules:
        if solution.does_whitelist_module(module.get_unique_name(), module._name):
            module._reset_volatile_fields()
            apply_module(target, module)
            if module.pch == PchUsage.PROHIBIT:
                module.pch_content = ''
            elif module.pch_content == '':
                raise ValueError(f'Module {module.get_unique_name()} has no pch content.')
            target_cursor['Modules'].append({
                'RelativeDir': module._relative_py_dir,
                'Name': module._name,
                'PchUsage': volatile_pch_usage_to_string(module.pch),
                'PchContent': module.pch_content,
                'Kind': module_kind_to_string(module.kind),
                'PublicDependencies': module.public_dependencies,
                'PrivateDependencies': module.private_dependencies
            })

    return target_cursor


def apply_module(target: Target, module: Module) -> None:
    """Apply a module with the policies of a target."""

    if target.volatile_build_configuration == BuildConfiguration.UNKNOWN:
        raise ValueError('Tried to apply volatile policies for module without build configuration.')

    ru.exec_function(module._absolute_py_path, 'apply_policies', target, module)

    return None
