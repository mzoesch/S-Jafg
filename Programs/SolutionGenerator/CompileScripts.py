# Copyright mzoesch. All rights reserved.

from ..Shared.RunSubprocess import *
from ..Shared.SharedDefinitions import *

def compile_scripts() -> None:
    """Will compile the scripts for the program."""

    print('Compiling scripts for the program.')
    compiled_working_dir: str = get_engine_root_dir() + '/Programs/'
    subprocess.check_call(f'go build Jafg', shell=True, cwd=compiled_working_dir)
    print('Scripts compiled successfully.')

    return None
