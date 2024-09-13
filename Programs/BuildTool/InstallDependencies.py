# Copyright mzoesch. All rights reserved.

import pathlib
import subprocess
import importlib.util as importlib_util
from ..Shared.FileStuff import *
from ..Shared.SharedDefinitions import *
from ..Shared.RunSubprocess import *

class MyPython:
    """
    Validates Python for the build tool if the minimum version is met and
    if required packages are installed. If not, it will try to install
    the required packages.
    """

    @classmethod
    def validate(cls) -> EErrorLevel:
        if not cls.__validate_python():
            return EErrorLevel.FATAL # Cannot validate further

        for packageName in ["requests"]:
            if not cls.__validate_package(packageName):
                return EErrorLevel.FATAL # Cannot validate further

        return EErrorLevel.SUCCESS

    @classmethod
    def __validate_python(cls, versionMajor = 3, versionMinor = 3) -> EErrorLevel:
        if sys.version is None:
            print('Python version not detected.')
            return EErrorLevel.FATAL

        print('Python version {0:d}.{1:d}.{2:d} detected.'.format( \
            sys.version_info.major, sys.version_info.minor, sys.version_info.micro))

        if      sys.version_info.major < versionMajor \
            or (sys.version_info.major == versionMajor and sys.version_info.minor < versionMinor):
            print('Python version too low, expected version {0:d}.{1:d} or higher.'.format(versionMajor, versionMinor))
            return EErrorLevel.FATAL

        return EErrorLevel.SUCCESS

    @classmethod
    def __validate_package(cls, packageName) -> EErrorLevel:
        if importlib_util.find_spec(packageName) is None:
            return cls.__install_package(packageName)

        return EErrorLevel.SUCCESS

    @classmethod
    def __install_package(cls, packageName) -> EErrorLevel:
        permission_granted = False
        while not permission_granted:
            reply = str(input('Would you like to install Python package "{0:s}"? [Y/N]: '.format(packageName))).lower().strip()[:1]

            if reply == 'y' or reply == 'Y':
                permission_granted = True
                continue

            if reply == 'n' or reply == 'N':
                print('Package installation aborted.')
                return EErrorLevel.FATAL

            continue

        print(f"Installing {packageName} module ...")
        subprocess.check_call(['python', '-m', 'pip', 'install', packageName])

        return cls.__validate_package(packageName)

class Premake:
    """
    Validates Premake for the build tool if if the right version
    is installed. If not, it will try to install the binaries.
    """

    version: str      =  '5.0.0-beta1'
    zip_url: str      = f'https://github.com/premake/premake-core/releases/download/v{version}/premake-{version}-windows.zip'
    license_url: str  =  'https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt'
    dir_to_locate:str =  'Programs/Vendor/Premake/Bin'

    @classmethod
    def get_abs_install_path(cls) -> str:
        return pathlib.Path(f'{pathlib.Path(get_engine_root_dir()).joinpath(cls.dir_to_locate).as_posix()}/')

    @classmethod
    def get_abs_install_file(cls) -> str:
        return pathlib.Path(f'{pathlib.Path(get_engine_root_dir()).joinpath(cls.dir_to_locate).as_posix()}/premake5.exe')

    @classmethod
    def get_abs_install_zip_file(cls) -> str:
        return pathlib.Path(f'{pathlib.Path(get_engine_root_dir()).joinpath(cls.dir_to_locate).as_posix()}/premake5.zip')

    @classmethod
    def get_abs_license_file(cls) -> str:
        return pathlib.Path(f'{pathlib.Path(get_engine_root_dir()).joinpath(cls.dir_to_locate).as_posix()}/LICENSE')

    @classmethod
    def validate(cls) -> EErrorLevel:
        if not cls.is_installed():
            return cls.install()

        print('Premake is already installed. Skipping ...')
        return EErrorLevel.SUCCESS

    @classmethod
    def is_installed(cls) -> bool:
        if not cls.get_abs_install_file().exists():
            return False

        return True

    @classmethod
    def install(cls) -> EErrorLevel:
        permission_granted: bool = False

        while not permission_granted:
            reply = str(input('Premake not found. Would you like to download Premake {0:s}? [Y/N]: '.format(cls.version))).lower().strip()[:1]
            
            if reply == 'y' or reply == 'Y':
                permission_granted = True
                continue
            
            if reply == 'n' or reply == 'N':
                print('Premake installation aborted.')
                return EErrorLevel.FATAL

            continue

        path_file: str = cls.get_abs_install_file()
        zip_file: str  = cls.get_abs_install_zip_file()
        print(f'Downloading Premake from {cls.zip_url} to {zip_file} ...')
        download_file(cls.zip_url, zip_file)
        print('Extracting Premake ...')
        UnzipFile(zip_file, deleteZipFile=False)
        print(f'Premake {cls.version} has been downloaded to {cls.dir_to_locate}.')

        if cls.get_abs_license_file().exists():
            print('Premake License file already exists. Skipping ...')
        else:
            license_path_file = cls.get_abs_license_file()
            print(f'Downloading {cls.license_url} to {license_path_file}')
            download_file(cls.license_url, license_path_file)
            print(f"Premake License file has been downloaded to '{cls.dir_to_locate}'")

        return EErrorLevel.SUCCESS

def run() -> EErrorLevel:
    print('Updating submodules ...')
    run_subprocess('git', 'submodule', 'update', '--init', '--recursive')

    error_level: EErrorLevel = MyPython.validate()

    if error_level != EErrorLevel.SUCCESS:
        print(f'Failed to validate Python. Error level: {error_level}')
        return error_level

    error_level = Premake.validate()

    if error_level != EErrorLevel.SUCCESS:
        print(f'Failed to validate Premake. Error level: {error_level}')
        return error_level

    return EErrorLevel.SUCCESS
