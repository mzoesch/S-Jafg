# Copyright mzoesch. All rights reserved.

import os
import platform
from enum import Enum
from pathlib import Path


_abs_engine_root_dir: Path = None


def get_abs_engine_root_dir() -> Path:
    global _abs_engine_root_dir

    if _abs_engine_root_dir is not None:
        return _abs_engine_root_dir

    dir_name: str = os.path.dirname(os.path.abspath(__file__)).replace('\\', '/')
    dir_name = '/'.join(dir_name.split('/')[:-1])
    dir_name = '/'.join(dir_name.split('/')[:-1])
    dir_path: Path = Path(dir_name)
    _abs_engine_root_dir = dir_path
    return get_abs_engine_root_dir()


class EErrorLevel(Enum):
    """
    Error level enumeration.
    """

    SUCCESS = 0
    FATAL = 1


class HostPlatform(Enum):
    """The platform of the native operating system that runs this script not the target build platform."""

    WINDOWS = 1
    OSX = 2
    LINUX = 3

    def is_this_windows(self) -> bool:
        return self.value == HostPlatform.WINDOWS.value

    def is_this_linux(self) -> bool:
        return self.value == HostPlatform.LINUX.value

    def is_this_osx(self) -> bool:
        return self.value == HostPlatform.OSX.value

    @staticmethod
    def is_windows() -> bool:
        return HostPlatform.get_current_platform() == HostPlatform.WINDOWS

    @staticmethod
    def is_osx() -> bool:
        return HostPlatform.get_current_platform() == HostPlatform.OSX

    @staticmethod
    def is_linux() -> bool:
        return HostPlatform.get_current_platform() == HostPlatform.LINUX

    @staticmethod
    def get_current_platform() -> 'HostPlatform':
        if platform.system() == 'Windows':
            return HostPlatform.WINDOWS

        if platform.system() == 'Darwin':
            return HostPlatform.OSX

        if platform.system() == 'Linux':
            return HostPlatform.LINUX

        raise NotImplementedError(f'Unsupported platform: {platform.system()}.')
