# Copyright mzoesch. All rights reserved.

import sys
import importlib.util as importlib_util
import pathlib
from pathlib import Path
from Programs.Shared import *
from Programs.Shared.Files import download_file, unzip_file


def update_submodules() -> None:
    print('Updating submodules ...')
    run_any_task('git', 'submodule', 'update', '--init', '--recursive')
    return None


class MyPython:
    """
    Validates Python for the build tool if the minimum version is met and
    if required packages are installed. If not, it will try to install
    the required packages.
    """

    @classmethod
    def validate(cls) -> EErrorLevel:
        if not cls.__validate_python():
            return EErrorLevel.FATAL  # Cannot validate further

        for package_name in ['requests', 'filelock']:
            if not cls.__validate_package(package_name):
                return EErrorLevel.FATAL  # Cannot validate further

        return EErrorLevel.SUCCESS

    @classmethod
    def __validate_python(cls, version_major=3, version_minor=3) -> EErrorLevel:
        if sys.version is None:
            print('Python version not detected.')
            return EErrorLevel.FATAL

        print('Python version {0:d}.{1:d}.{2:d} detected.'.format(
            sys.version_info.major, sys.version_info.minor, sys.version_info.micro))

        if (sys.version_info.major < version_major
                or (sys.version_info.major == version_major
                    and sys.version_info.minor < version_minor)):
            print('Python version too low, expected version {0:d}.{1:d} or higher.'.format(
                version_major, version_minor))
            return EErrorLevel.FATAL

        return EErrorLevel.SUCCESS

    @classmethod
    def __validate_package(cls, package_name) -> EErrorLevel:
        if importlib_util.find_spec(package_name) is None:
            return cls.__install_package(package_name)

        print(f"Module [{package_name}] already installed. Skipping ...")
        return EErrorLevel.SUCCESS

    @classmethod
    def __install_package(cls, package_name) -> EErrorLevel:
        print(f"Installing {package_name} module ...")
        run_any_task('python', '-m', 'pip', 'install', package_name)
        return cls.__validate_package(package_name)


def validate_python() -> EErrorLevel:
    print('Validating python ...')
    return MyPython.validate()


class MyOutCapture:
    stdout: str = None

    def __init__(self):
        self.stdout = ""

    def append(self, line: str):
        self.stdout += line

    def print(self):
        print(self.stdout)


class MyGo:

    _go_version: str = '1.23.'

    @classmethod
    def validate(cls) -> EErrorLevel:
        return cls.__validate_go()

    @classmethod
    def __validate_go(cls) -> EErrorLevel:
        copied_stdout: MyOutCapture = MyOutCapture()
        if run_any_task_with_stdout_ok_to_fail(copied_stdout, 'go', 'version') != 0:
            print('Failed to launch popen for go detection. '
                  'Proceeding to install Jafg precompiled binaries.')
            cls.install_jafg_bins()  # This will just crash if no bins for the native platform.
            return EErrorLevel.SUCCESS

        idx: int = copied_stdout.stdout.find(f'go{cls._go_version}')
        if idx == -1:

            print('Go version not detected or incompatible version is installed. '
                  'Proceeding to install Jafg precompiled binaries.')
            cls.install_jafg_bins()  # This will just crash if no bins for the native platform.
            return EErrorLevel.SUCCESS

        print('Go version is ok. Proceeding ...')
        return EErrorLevel.SUCCESS

    @classmethod
    def install_jafg_bins(cls) -> None:
        print('Installing Jafg scripting binaries ...')

        if Platform.is_windows():
            ...  # Download win binaries, etc.

        raise NotImplementedError(
            f'Your platform does not have precompiled binaries for Jafg. '
            f'You will have to install go v{cls._go_version}.'
        )


def validate_go() -> EErrorLevel:
    print('Validating go ...')
    return MyGo.validate()


class MyPremake:
    """
    Validates Premake for the build tool if the right version
    is installed. If not, it will try to install the binaries.
    """

    version: str = '5.0.0-beta1'
    zip_url: str = f'https://github.com/premake/premake-core/releases/download/v{version}/premake-{version}-windows.zip'
    license_url: str = 'https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt'
    dir_to_locate: str = 'Programs/Vendor/Premake/Bin'

    @classmethod
    def get_abs_install_path(cls) -> Path:
        return pathlib.Path(
            f'{pathlib.Path(get_abs_engine_root_dir()).joinpath(cls.dir_to_locate).as_posix()}/'
        )

    @classmethod
    def get_abs_install_file(cls) -> Path:
        return pathlib.Path(
            f'{pathlib.Path(get_abs_engine_root_dir()).joinpath(cls.dir_to_locate).as_posix()}/premake5.exe'
        )

    @classmethod
    def get_abs_install_zip_file(cls) -> Path:
        return pathlib.Path(
            f'{pathlib.Path(get_abs_engine_root_dir()).joinpath(cls.dir_to_locate).as_posix()}/premake5.zip'
        )

    @classmethod
    def get_abs_license_file(cls) -> Path:
        return pathlib.Path(
            f'{pathlib.Path(get_abs_engine_root_dir()).joinpath(cls.dir_to_locate).as_posix()}/LICENSE'
        )

    @classmethod
    def validate(cls) -> EErrorLevel:
        if not cls.is_installed():
            return cls.install()

        print('Premake is already installed. Skipping ...')
        return EErrorLevel.SUCCESS

    @classmethod
    def is_installed(cls) -> bool:
        return cls.get_abs_install_file().exists()

    @classmethod
    def install(cls) -> EErrorLevel:
        path_file: Path = cls.get_abs_install_file()
        zip_file: Path = cls.get_abs_install_zip_file()

        print(f'Downloading Premake from {cls.zip_url} to {zip_file} ...')
        if download_file(cls.zip_url, zip_file) is not EErrorLevel.SUCCESS:
            print('Failed to download Premake.')
            return EErrorLevel.FATAL

        print('Extracting Premake ...')
        if unzip_file(zip_file, delete_zip_file=False) is not EErrorLevel.SUCCESS:
            print('Failed to extract Premake.')
            return EErrorLevel.FATAL

        print(f'Premake {cls.version} has been downloaded to {cls.dir_to_locate}.')

        if cls.get_abs_license_file().exists():
            print('Premake License file already exists. Skipping ...')
        else:
            license_path_file = cls.get_abs_license_file()
            print(f'Downloading {cls.license_url} to {license_path_file}')
            download_file(cls.license_url, license_path_file)
            print(f"Premake License file has been downloaded to '{cls.dir_to_locate}'")

        return EErrorLevel.SUCCESS


def validate_premake(is_all_platforms: bool) -> EErrorLevel:
    print('Validating premake ...')
    return MyPremake.validate()


class MyCmake:
    """
    Validates Cmake for the build tool if the right version
    is installed. If not it will fail.
    """

    @classmethod
    def validate(cls) -> EErrorLevel:
        # Python terminates if this function call fails.
        if run_any_task_ok_to_fail('cmake', '--version') == 0:
            return EErrorLevel.SUCCESS
        return EErrorLevel.FATAL


def validate_cmake() -> EErrorLevel:
    print('Validating cmake ...')
    if MyCmake.validate() is not EErrorLevel.SUCCESS:
        print(
            'Cmake not found. This is okay. But this implies that not all platforms will be supported or '
            'building the project without a solution.'
        )
    return EErrorLevel.SUCCESS


def route_to_subprogram(*args, **kwargs) -> EErrorLevel:
    print('Routing to subprogram ...')

    if '--UpdateSubmodules' in args:
        update_submodules()

    if '--ValidatePython' in args:
        error_level: EErrorLevel = validate_python()
        if error_level is not EErrorLevel.SUCCESS:
            return error_level

    if '--ValidateGo' in args:
        error_level: EErrorLevel = validate_go()
        if error_level is not EErrorLevel.SUCCESS:
            return error_level

    if '--ValidatePremake' in args:
        error_level: EErrorLevel = validate_premake(True if 'AllPremakePlatforms' in args else False)
        if error_level is not EErrorLevel.SUCCESS:
            return error_level

    if '--ValidateCmake' in args:
        error_level: EErrorLevel = validate_cmake()
        if error_level is not EErrorLevel.SUCCESS:
            return error_level

    return EErrorLevel.SUCCESS
