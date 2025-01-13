# Copyright mzoesch. All rights reserved.

import os
import sys
import importlib.util as importlib_util
from typing import List
from pathlib import Path


def get_all_entries_in_directory(absolute_directory: str, concat: bool = False) -> List[str]:
    return [
        (os.path.join(absolute_directory, f) if concat else f)
        for f in os.listdir(absolute_directory)
    ]


def get_all_files_in_directory(absolute_directory: str, concat: bool = False) -> List[str]:
    return [
        (os.path.join(absolute_directory, f) if concat else f)
        for f in os.listdir(absolute_directory) if os.path.isfile(os.path.join(absolute_directory, f))
    ]


def get_all_dirs_in_directory(absolute_directory: str, concat: bool = False) -> List[str]:
    return [
        (os.path.join(absolute_directory, f) if concat else f)
        for f in os.listdir(absolute_directory) if os.path.isdir(os.path.join(absolute_directory, f))
    ]


def get_base(absolute_path: str) -> str:
    return os.path.basename(absolute_path)


def get_dir(path: str) -> str:
    """A/B/C.D -> A/B"""
    return os.path.dirname(path)


def abs_to_rel_fancy(absolute_path: str) -> str:
    return '~' + normalize_path(os.path.relpath(absolute_path))


def abs_to_rel(absolute_path: str) -> str:
    return normalize_path(os.path.relpath(absolute_path))


def normalize_path(path: str) -> str:
    return os.path.normpath(path).replace('\\', '/')


def is_file(absolute_path: str) -> bool:
    return os.path.isfile(absolute_path)


def is_directory(absolute_path: str) -> bool:
    return os.path.isdir(absolute_path)


def is_path_valid(absolute_path: str) -> bool:
    return os.path.exists(absolute_path)


def exec_function(absolute_path: str, function_name: str, *args, **kwargs) -> any:
    p = Path(absolute_path).resolve()
    module_name = p.stem

    spec = importlib_util.spec_from_file_location(module_name, str(p))
    module = importlib_util.module_from_spec(spec)
    sys.modules[module_name] = module
    spec.loader.exec_module(module)

    if hasattr(module, function_name):
        func = getattr(module, function_name)
        try:
            return func(*args, **kwargs)
        except TypeError as e:
            print(f'\033[91mFailed to add target in file: [{absolute_path}].\033[0m')
            raise TypeError(e)
        except AttributeError as e:
            print(f'\033[91mFailed to add target in file: [{absolute_path}].\033[0m')
            raise AttributeError(e)

    raise AttributeError(f'Function "{function_name}" not found in {absolute_path}')


def try_exec_function(absolute_path: str, function_name: str, *args, **kwargs) -> any:
    p = Path(absolute_path).resolve()
    module_name = p.stem

    spec = importlib_util.spec_from_file_location(module_name, str(p))
    module = importlib_util.module_from_spec(spec)
    sys.modules[module_name] = module
    spec.loader.exec_module(module)

    if hasattr(module, function_name):
        func = getattr(module, function_name)
        try:
            return func(*args, **kwargs)
        except TypeError as e:
            print(f'\033[91mFailed to add target in file: [{absolute_path}].\033[0m')
            raise TypeError(e)
        except AttributeError as e:
            print(f'\033[91mFailed to add target in file: [{absolute_path}].\033[0m')
            raise AttributeError(e)

    return None
