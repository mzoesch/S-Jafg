# Copyright mzoesch. All rights reserved.

import os
from pathlib import Path
from ..Shared.SharedDefinitions import *
from ..Shared.RunSubprocess import *

def fix_cpp_pch_subsystem_for_project(project_file_path: str) -> None:
    """
    Fix the wrongly generated precompiled header subsystem in the solution from /Yu to /Yc.
    """

    if not os.path.exists(project_file_path):
        print(f'Error: Project file "{project_file_path}" does not exist.')
        return None

    new_vcxproj_content: str = ''
    with open(project_file_path, 'r') as project_file:
        for line in project_file:
            new_vcxproj_content += line.replace(
                '<PrecompiledHeader>Use</PrecompiledHeader>',
                '<PrecompiledHeader>Create</PrecompiledHeader>'
            )
    with open(project_file_path, 'w') as project_file:
        project_file.write(new_vcxproj_content)

    return None

def fix_cpp_pch_subsystem() -> None:
    print('Fixing precompiled header subsystem (from /Yu to /Yc) ...')

    fix_cpp_pch_subsystem_for_project(os.path.abspath('./Runtime/Runtime.vcxproj'))
    fix_cpp_pch_subsystem_for_project(os.path.abspath('./Core/Core.vcxproj'))
    fix_cpp_pch_subsystem_for_project(os.path.abspath('./Lal/Lal.vcxproj'))

    return None

def run() -> EErrorLevel:
    print('Generating solution files with Premake5 ...')
    print('Resolved platform: ', Platform.get_current_platform(), '.', sep='')

    if Platform.is_windows():
        subprocess: str = os.path.abspath(get_engine_root_dir() + '/Programs/BuildTool/RunPremake-Win.bat')
        print('Running command: ', subprocess, sep='')
        run_subprocess(subprocess)
    else:
        raise ValueError('Platform not supported.')

    cwd: str = os.getcwd()
    os.chdir(get_engine_root_dir())
    if os.getcwd() != cwd:
        print('Changed cwd to: ', os.getcwd(), '.', sep='')
    
    fix_cpp_pch_subsystem()
    
    os.chdir(cwd)

    return EErrorLevel.SUCCESS
