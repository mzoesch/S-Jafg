# Copyright mzoesch. All rights reserved.

import os
import sys
from Programs.Shared import *
from Programs.Meta.Application import GApp
from Programs.Reflector import ReflectionUtility as ru

def reflect_all_targets_and_modules() -> None:
    cwd = os.getcwd()
    for entry in ru.get_all_entries_in_directory(cwd, True):
        if ru.is_directory(entry):
            if (GApp.workspace.is_tl_ignored(ru.get_base(entry))
                    or GApp.workspace.is_ignored(ru.get_base(entry))):
                continue
            scan_dir_for_targets_and_modules(entry)

    return None

def scan_dir_for_targets_and_modules(absolute_path: str) -> None:
    print(f'Scanning: {absolute_path}')
    for entry in ru.get_all_entries_in_directory(absolute_path, True):
        if ru.is_directory(entry):
            if GApp.workspace.is_ignored(ru.get_base(entry)):
                continue
            scan_dir_for_targets_and_modules(entry)

    return None
