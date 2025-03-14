# Copyright mzoesch. All rights reserved.

import os
import sys
import json
from Programs.Shared import *
from Programs.Meta.Application import GApp
from Programs.Meta.BuildConfiguration import BuildConfiguration, BufferedBuildConfiguration
from Programs.Meta.Solution import BufferedSolution, Solution
from Programs.Meta.Platform import BufferedPlatform, Platform
from Programs.Meta.Target import BufferedTarget, Target
from Programs.Meta.Module import BufferedModule, Module
from Programs.Meta.Pch import PchUsage, pch_usage_to_bool
from Programs.Meta.ModuleKind import ModuleKind, module_kind_to_string
from Programs.Reflector import ReflectionUtility as ru


def reflect_all_of_engine() -> None:
    root_dir = str(get_abs_engine_root_dir())
    for entry in ru.get_all_dirs_in_directory(root_dir, True):
        if GApp.workspace.is_tl_ignored(ru.get_base(entry)) or GApp.workspace.is_ignored(ru.get_base(entry)):
            continue

        scan_dir_for_sln_p_bc_t_m(entry)
        continue

    create_cached_program_agnostic_file()
    return None


def scan_dir_for_sln_p_bc_t_m(absolute_path: str) -> None:
    print(f'Scanning [{absolute_path}].')

    for f_entry in ru.get_all_files_in_directory(absolute_path):
        if '.solution.py' not in f_entry:
            continue
        solution: BufferedSolution = BufferedSolution(os.path.join(absolute_path, f_entry))
        GApp.add_solution(solution)
        continue

    for f_entry in ru.get_all_files_in_directory(absolute_path):
        if '.platform.py' not in f_entry:
            continue
        platform: BufferedPlatform = BufferedPlatform(os.path.join(absolute_path, f_entry))
        GApp.add_platform(platform)
        continue

    for f_entry in ru.get_all_files_in_directory(absolute_path):
        if '.build_configuration.py' not in f_entry:
            continue
        build_configuration: BufferedBuildConfiguration = BufferedBuildConfiguration(os.path.join(absolute_path, f_entry))
        GApp.add_build_configuration(build_configuration)
        continue

    for f_entry in ru.get_all_files_in_directory(absolute_path):
        if '.target.py' not in f_entry:
            continue
        target: BufferedTarget = BufferedTarget(os.path.join(absolute_path, f_entry))
        GApp.add_target(target)
        continue

    module_init_py: str = f'{ru.get_base(absolute_path)}.module.py'
    for f_entry in ru.get_all_files_in_directory(absolute_path):
        if '.module.py' not in f_entry:
            continue
        entry: str = os.path.join(absolute_path, f_entry)
        if module_init_py not in entry:
            raise ValueError(f'Faulty module file: [{entry}]. Does not match parent.')
    if ru.is_path_valid(os.path.join(absolute_path, module_init_py)):
        module: BufferedModule = BufferedModule(os.path.join(absolute_path, module_init_py))
        GApp.add_module(module)
        return None  # Do not scan further (nested modules are not allowed).

    for entry in ru.get_all_dirs_in_directory(absolute_path, True):
        if GApp.workspace.is_ignored(ru.get_base(entry)):
            continue
        scan_dir_for_sln_p_bc_t_m(entry)
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

    data: dict = {'solutions': []}
    for solution in GApp.solutions:
        data['solutions'].append(apply_solution(solution))
        continue

    with open(absolute_cached_file_path, 'w') as f:
        json.dump(data, f, indent=2)
        f.write('\n')

    return None


def apply_solution(solution: BufferedSolution) -> dict:
    volatile_solution: Solution = Solution(solution)
    volatile_solution.load()
    volatile_solution.validate()

    out_data: dict = {
        'name': solution._name,
        'relative_dir': solution._relative_py_dir,
        'startup': volatile_solution.startup,
        'max_modules': [],
        'platforms': []
    }

    for platform in GApp.platforms:
        out_data['platforms'].append(apply_platform(volatile_solution, platform))
        continue

    class _Module:
        def __init__(self, name: str, relative_dir: str):
            self.name = name
            self.relative_dir = relative_dir
    _modules: list[_Module] = []

    for p in out_data['platforms']:
        for c in p['configs']:
            for t in c['targets']:
                for m in t['modules']:
                    if m['relative_dir'] not in [x.relative_dir for x in _modules]:
                        _modules.append(_Module(m['name'], m['relative_dir']))
                    continue
                continue
            continue
        continue

    for m in _modules:
        out_data['max_modules'].append({
            'name': m.name,
            'relative_dir': m.relative_dir
        })
        continue

    return out_data


def apply_platform(solution: Solution, platform: BufferedPlatform) -> dict:
    volatile_platform: Platform = Platform(platform)
    volatile_platform.load(solution)
    volatile_platform.validate()

    out_data: dict = {
        'name': platform._name,
        'version': volatile_platform.version,
        'architecture': volatile_platform.architecture,
        'toolset': volatile_platform.toolset,
        'lnk_flags': volatile_platform.lnk_flags,
        'build_flags': volatile_platform.build_flags,
        'defines': [],
        'configs': []
    }

    for d in volatile_platform.defines:
        if d == '':
            raise ValueError('Platform defines cannot be empty.')
        out_data['defines'].append(d)
        continue

    for build_configuration in GApp.builds:
        out_data['configs'].append(apply_build_configuration(solution, volatile_platform, build_configuration))
        continue

    return out_data


def apply_build_configuration(solution: Solution, platform: Platform, build_configuration: BufferedBuildConfiguration) -> dict:
        volatile_build_configuration: BuildConfiguration = BuildConfiguration(build_configuration)
        volatile_build_configuration.load(solution, platform)
        volatile_build_configuration.validate()

        out_data: dict = {
            'name': build_configuration._name,
            'runtime': volatile_build_configuration.runtime,
            'symbols': volatile_build_configuration.symbols,
            'optimize': volatile_build_configuration.optimize,
            'defines': [],
            'targets': [],
        }

        for d in volatile_build_configuration.defines:
            if d == '':
                raise ValueError('Build configuration defines cannot be empty.')
            out_data['defines'].append(d)
            continue

        for target in GApp.targets:
            if solution.use_black_list_target and solution.does_blacklist_target(target.get_unique_name(), target._name):
                continue
            out_data['targets'].append(apply_target(solution, platform, volatile_build_configuration, target))
            continue

        return out_data


def apply_target(solution: Solution, platform: Platform, build_configuration: BuildConfiguration, target: BufferedTarget) -> dict:
    volatile_target: Target = Target(target)
    volatile_target.load(solution, platform, build_configuration)
    volatile_target.validate()

    out_data: dict = {
        'name': target._name,
        'defines': [],
        'modules': [],
    }

    for d in volatile_target.defines:
        if d == '':
            raise ValueError('Target defines cannot be empty.')
        out_data['defines'].append(d)
        continue

    for module in GApp.modules:
        if solution.use_white_list_modules and not solution.does_whitelist_module(module._relative_py_dir, module._name):
            continue
        out_data['modules'].append(apply_module(solution, platform, build_configuration, volatile_target, module))
        continue

    return out_data


def apply_module(solution: Solution, platform: Platform, build_configuration: BuildConfiguration, target: Target, module: BufferedModule) -> dict:
    volatile_module: Module = Module(module)
    volatile_module._add_default_flags(solution, platform, build_configuration, target)
    volatile_module.load(solution, platform, build_configuration, target)
    volatile_module.validate()

    out_data: dict = {
        'name': module._name,
        'friendly_name': volatile_module.friendly_name,
        'relative_dir': module._relative_py_dir,
        'pch': pch_usage_to_bool(volatile_module.pch),
        'pch_content': volatile_module.pch_content,
        'kind': module_kind_to_string(volatile_module.kind),
        'entry': volatile_module.entry,
        'pre_builds': volatile_module.pre_builds,
        'post_builds': volatile_module.post_builds,
        'public_dependencies': volatile_module.public_dependencies,
        'private_dependencies': volatile_module.private_dependencies,
        'native_includes': volatile_module.native_includes,
        'native_dependencies': volatile_module.native_dependencies,
        'native_runtime_dependencies': volatile_module.native_runtime_dependencies,
        'native_additional_runtime_dependencies': volatile_module.native_additional_runtime_dependencies,
    }

    return out_data
