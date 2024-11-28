# Copyright mzoesch. All rights reserved.
import os.path
import shutil
import subprocess

from ..Shared.RunSubprocess import *
from ..Shared.SharedDefinitions import *

def generate_solution(emulate_compilation: bool) -> None:
    """Will generate the solution for the program."""

    print('Generating solution for the program.')

    compiled_working_dir: str = get_engine_root_dir() + '/Programs/'
    target_binary: str = get_engine_root_dir() + '/Programs/Jafg.exe'
    subprocess.check_call(f'{target_binary} --SolutionGenerator --Generate', cwd=compiled_working_dir)

    previous_working_dir: str = os.getcwd()
    os.chdir(get_engine_root_dir())
    print(f'Current working dir [{os.getcwd()}].')

    # print('Removing solution trees from the Saved directory.')
    # if os.path.exists('Saved/Solution'):
    #     for file in os.listdir('Saved/Solution'):
    #         if (
    #                     file != 'CMakeCache.txt'
    #                 and file != '.idea'
    #                 and file != 'CMakeFiles'
    #                 and (".vcxproj" not in file)
    #                 and file != '.vs'
    #                 and file != 'Jafg.sln'
    #         ):
    #             file_path: str = os.path.join('Saved/Solution', file)
    #             if os.path.isfile(file_path):
    #                 os.unlink(file_path)
    #             elif os.path.isdir(file_path):
    #                 shutil.rmtree(file_path)
    # if not os.path.exists('Saved'):
    #     os.makedirs('Saved')
    # if not os.path.exists('Saved/Solution'):
    #     os.makedirs('Saved/Solution')
    # if os.path.exists('Saved/Minimal'):
    #     shutil.rmtree('Saved/Minimal')
    # if not os.path.exists('Saved/Minimal'):
    #     os.makedirs('Saved/Minimal')

    solution_args = ['cmake', '../..', '-G', 'Visual Studio 17 2022']
    minimal_args =  ['cmake', '../..', '-G', 'MinGW Makefiles', '-DCMAKE_BUILD_TYPE=Debug-Client']

    print(f'Executing: {solution_args} in Saved/Solution')
    subprocess.check_call(solution_args, cwd='Saved/Solution')
    print(f'Executing: {minimal_args} in Saved/Minimal')
    subprocess.check_call(minimal_args, cwd='Saved/Minimal')

    print('Executing: {target_binary} --SolutionGenerator --PostLuaRun')
    subprocess.check_call(f'{target_binary} --SolutionGenerator --PostLuaRun', cwd=compiled_working_dir)

    if emulate_compilation:
        print('Emulating a full all build compilation of Jafg...')
        print(f'Executing: {target_binary} --SolutionGenerator --Emulate-Compilation-All')
        subprocess.check_call(
            f'{target_binary} --SolutionGenerator --Emulate-Compilation-All',
            cwd=compiled_working_dir
        )
        print('Emulation complete.')

        print(f'Executing: {solution_args} in Saved/Solution')
        subprocess.check_call(solution_args, cwd='Saved/Solution')
        print(f'Executing: {minimal_args} in Saved/Minimal')
        subprocess.check_call(minimal_args, cwd='Saved/Minimal')
        print('Finished reflecting emulation in the solution.')

    print('Executing: {target_binary} --SolutionGenerator --PostLuaRun')
    subprocess.check_call(f'{target_binary} --SolutionGenerator --PostLuaRun', cwd=compiled_working_dir)

    if not os.path.exists('Saved/Solution/Jafg.sln'):
        raise RuntimeError('Solution file not found in the expected location at Saved/Solution/Jafg.sln.')
    if not os.path.exists('Jafg.sln.lnk'):
        if Platform.is_windows():
            subprocess.check_call([
                'powershell',
                '-ExecutionPolicy', 'Bypass',
                '-File',
                'Programs/Shell/CreateSymLinkSln.ps1',
            ])
            print('Created symlink to the solution file.')
        else:
            raise ValueError('Platform is missing implementation for this operation.')

    os.chdir(previous_working_dir)
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
