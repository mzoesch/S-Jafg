# Copyright mzoesch. All rights reserved.

import subprocess
import os
from .Shared.SharedDefinitions import *
from .Shared.RunSubprocess import *

def parse_args_to_kwargs(*args: list[str]) -> dict[str, str]:
    """Will convert a list of arguments to a dictionary of keyword arguments."""

    kwargs = {}

    for arg in args:
        if '=' in arg:
            key, value = arg.split('=', 1)
            kwargs[key] = value

    return kwargs

def exec_custom_command(*args, **kwargs) -> None:
    """Will execute a custom command with the given arguments and keyword arguments."""
    saved_cwd = os.getcwd()
    os.chdir(get_engine_root_dir())

    if '--WasmRuntime' in args:
        if '--BUILD_CONFIG' not in kwargs:
            raise Exception(f'No build config specified: {kwargs=}')
        build_config: str = kwargs.get('--BUILD_CONFIG')

        if not os.path.exists('Saved'):
            os.makedirs('Saved')
        if not os.path.exists(f'Saved/MinimalWasm-{build_config}'):
            os.makedirs(f'Saved/MinimalWasm-{build_config}')

        if not os.path.exists(f'Saved/MinimalWasm-{build_config}/Makefile'):
            print(
                f'Path [/Saved/SolutionWasm-{build_config}/Makefile] does not exist. Calling'
                f' emscripten wrapper to build our cmake.'
            )
            args = [
                os.path.abspath(get_engine_root_dir() + '/Programs/Shell/WasmEmscriptenMake.bat'),
                build_config
            ]
            print(f'Executing: {args}')
            run_subprocess(*args)
        print(f'Path [/Saved/SolutionWasm-{build_config}/Makefile] exists. Skipping make call.')

        cleanness: str = 'Clean' if '-clean-first-cmake' in args else ''
        args = [
            os.path.abspath(
                get_engine_root_dir() +
                f'/Programs/Shell/Wasm{cleanness}.bat'
            ),
            build_config
        ]
        print(f'Executing: {args}')
        run_subprocess(*args)
        print(f'Finished executing. Ok.')
    else:
        raise Exception(f'Unknown custom command: {args=}, {kwargs=}')

    os.chdir(saved_cwd)

    return None

def route_to_program(*args, **kwargs) -> None:
    """Will call the main function of the program to run in this library."""

    print(f'Try running program with: {args=}.')

    if '--fwd' in args:
        compiled_working_dir: str = get_engine_root_dir() + '/Programs/'
        target_binary: str = get_engine_root_dir() + '/Programs/Jafg.exe'
        cat_args: str = ' '.join(args)
        subprocess.check_call(f'{target_binary} {cat_args}', shell=True, cwd=compiled_working_dir)
        return None

    if '--install-deps' in args:
        from Programs.SolutionGenerator import InstallDeps as install_deps
        install_deps.install_deps()

    if '--compile-scripts' in args:
        from Programs.SolutionGenerator import CompileScripts as compile_scripts
        compile_scripts.compile_scripts()

    if '--generate-solution' in args:
        from Programs.SolutionGenerator import GenerateSolution as generate_solution
        generate_solution.generate_solution(True if '--emulate' in args else False)

    if '--generate-solution-for-lal-unit-tests' in args:
        from Programs.SolutionGenerator import GenerateSolution as generate_solution
        generate_solution.generate_solution_for_lal_unit_tests()

    if '--custom-command' in args:
        additional_kwargs = parse_args_to_kwargs(*args)
        kwargs.update(additional_kwargs)
        exec_custom_command(*args, **kwargs)

    return None
