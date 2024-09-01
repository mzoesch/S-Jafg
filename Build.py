import os
import platform
from enum import Enum
from Programs.BuildTool.RunSubprocess import run_subprocess

class Platform(Enum):
    WINDOWS = 1
    OSX     = 2
    LINUX   = 3

    @staticmethod
    def is_windows() -> bool:
        return Platform.get_current_platform() == Platform.WINDOWS

    @staticmethod
    def get_current_platform() -> 'Platform':
        if platform.system() == 'Windows':
            return Platform.WINDOWS
        raise NotImplementedError(f'Unsupported platform: {platform.system()}.') 

def fix_cpp_pch_subsystem() -> None:
    """
    Fix the wrongly generated precompiled header subsystem in the solution from /Yu to /Yc.
    """

    print('Fixing precompiled header subsystem (from /Yu to /Yc) ...')

    # App .vcxproj file
    app_vcxproj_path: str = os.path.abspath('./App/App.vcxproj')
    new_app_vcxproj_content: str = ''
    with open(app_vcxproj_path, 'r') as app_vcxproj_file:
        for line in app_vcxproj_file:
            new_app_vcxproj_content += line.replace(
                '<PrecompiledHeader>Use</PrecompiledHeader>',
                '<PrecompiledHeader>Create</PrecompiledHeader>'
            )
    with open(app_vcxproj_path, 'w') as app_vcxproj_file:
        app_vcxproj_file.write(new_app_vcxproj_content)

    # Core .vcxproj file
    core_vcxproj_path: str = os.path.abspath('./Core/Core.vcxproj')
    new_core_vcxproj_content: str = ''
    with open(core_vcxproj_path, 'r') as core_vcxproj_file:
        for line in core_vcxproj_file:
            new_core_vcxproj_content += line.replace(
                '<PrecompiledHeader>Use</PrecompiledHeader>',
                '<PrecompiledHeader>Create</PrecompiledHeader>'
            )
    with open(core_vcxproj_path, 'w') as core_vcxproj_file:
        core_vcxproj_file.write(new_core_vcxproj_content)

    return None

def generate_solution_files_with_premake() -> None:
    print('Generating solution files with premake ...')
    print('Resolved platform: ', Platform.get_current_platform(), ".", sep='')

    if Platform.is_windows():
        run_subprocess(os.path.abspath('./Programs/BuildTool/RunPremake-Win.bat'), "nopause")

    fix_cpp_pch_subsystem()

    return None

def main(*args, **kwargs) -> int:
    print('Updating submodules ...')
    run_subprocess('git', 'submodule', 'update', '--init', '--recursive')

    generate_solution_files_with_premake()

    return 0

if __name__ == '__main__':
    import sys
    result: int = main(*sys.argv[1:])
    print(f'Exit code: {result}.')
    sys.exit(result)
