# Copyright mzoesch. All rights reserved.

import pathlib
from pathlib import Path
import platform
from enum import Enum
from .CommonBuildStuff import *

def get_engine_root_dir():
    """Gets the root directory of the engine."""
    # Hardcoded here, but we should probably get a better solution for this?
    path = pathlib.Path(__file__).parent.parent.parent.expanduser().resolve().as_posix()
    return path

def get_bin_dir(config: BuildConfiguration, platform: BuildPlatform, *, check_result=True) -> str:
    """Gets the compiled bin dir for the specified configuration and platform."""
    bin_dir = f'{get_engine_root_dir()}/Binaries'

    if Path(bin_dir).exists() is False:
        raise FileNotFoundError(f'Binaries directory not found at {bin_dir}.')

    dirs = [d for d in pathlib.Path(bin_dir).iterdir() if d.is_dir()]

    if len(dirs) == 0:
        raise FileNotFoundError(f'No directories found in {bin_dir}.')

    resolved_platform: bool = False
    if Platform.is_windows():
        for d in dirs:
            if 'windows' in d.name.lower():
                bin_dir = d
                resolved_platform = True
                break

    if resolved_platform is False:
        raise FileNotFoundError(f'No platform directory found in {bin_dir}.')

    bin_dir = f'{bin_dir}/{build_configuration_to_string(config)}/Runtime'
    bin_dir = pathlib.Path(bin_dir).expanduser().resolve().as_posix()

    if check_result:
        if Path(bin_dir).exists() is False:
            raise FileNotFoundError(f'Binaries directory not found at {bin_dir}.')

    return bin_dir

def get_content_dir() -> str:
    """Gets the content directory of the engine."""
    return Path(f'{get_engine_root_dir()}/Content').expanduser().resolve().as_posix()

class EErrorLevel(Enum):
    """
    Error level enumeration.
    """

    SUCCESS = 0
    FATAL   = 1

class Platform(Enum):
    WINDOWS = 1
    OSX     = 2
    LINUX   = 3

    @staticmethod
    def is_windows() -> bool:
        return Platform.get_current_platform() == Platform.WINDOWS

    @staticmethod
    def is_osx() -> bool:
        return Platform.get_current_platform() == Platform.OSX

    @staticmethod
    def is_linux() -> bool:
        return Platform.get_current_platform() == Platform.LINUX

    @staticmethod
    def get_current_platform() -> 'Platform':
        if platform.system() == 'Windows':
            return Platform.WINDOWS

        # We will just raise an error here, as the other scripts
        # currently only support Windows.
        raise NotImplementedError(f'Unsupported platform: {platform.system()}.')
