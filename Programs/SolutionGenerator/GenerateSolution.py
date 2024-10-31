# Copyright mzoesch. All rights reserved.
import os.path

from ..Shared.RunSubprocess import *
from ..Shared.SharedDefinitions import *

def generate_solution() -> None:
    """Will generate the solution for the program."""

    print('Generating solution for the program.')

    compiled_working_dir: str = get_engine_root_dir() + '/Programs/'
    target_binary: str = get_engine_root_dir() + '/Programs/Jafg.exe'
    subprocess.check_call(f'{target_binary} --SolutionGenerator --Generate', shell=True, cwd=compiled_working_dir)

    if Platform.is_windows():
        premake_process: str = os.path.abspath(get_engine_root_dir() + '/Programs/Shell/RunPremake-Win.bat')
        print(f'Running premake script: {premake_process}')
        run_subprocess(premake_process)
    else:
        raise ValueError('Platform not supported.')

    subprocess.check_call(f'{target_binary} --SolutionGenerator --PostLuaRun', shell=True, cwd=compiled_working_dir)

    print('Solution generated successfully.')

    return None

def generate_solution_for_lal_unit_tests() -> None:
    """
    Will bring the solution to only have the lal environment and the unit tests for it.
    This significantly reduces compile times when making changes to critical parts of this app.
    """

    print('Generating lal unit test solution for the program.')

    compiled_working_dir: str = get_engine_root_dir() + '/Programs/'
    target_binary: str = get_engine_root_dir() + '/Programs/Jafg.exe'
    subprocess.check_call(f'{target_binary} --SolutionGenerator --Generate-Lal-Unit-Tests', shell=True, cwd=compiled_working_dir)

    if Platform.is_windows():
        premake_process: str = os.path.abspath(get_engine_root_dir() + '/Programs/Shell/RunPremake-Win.bat')
        print(f'Running premake script: {premake_process}')
        run_subprocess(premake_process)
    else:
        raise ValueError('Platform not supported.')

    subprocess.check_call(f'{target_binary} --SolutionGenerator --PostLuaRun', shell=True, cwd=compiled_working_dir)

    print('Solution generated successfully.')
    
    return None
