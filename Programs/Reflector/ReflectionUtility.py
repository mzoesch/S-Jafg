# Copyright mzoesch. All rights reserved.

import os
from typing import List

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

def is_file(absolute_path: str) -> bool:
    return os.path.isfile(absolute_path)

def is_directory(absolute_path: str) -> bool:
    return os.path.isdir(absolute_path)

def is_path_valid(absolute_path: str) -> bool:
    return os.path.exists(absolute_path)
