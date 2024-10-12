# Copyright mzoesch. All rights reserved.

import subprocess
from .Shared.SharedDefinitions import *

def parse_args_to_kwargs(*args: list[str]) -> dict[str, str]: 
    """Will convert a list of arguments to a dictionary of keyword arguments."""

    kwargs = {}

    for arg in args:
        if '=' in arg:
            key, value = arg.split('=', 1)
            kwargs[key] = value

    return kwargs

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
        generate_solution.generate_solution()

    return None
