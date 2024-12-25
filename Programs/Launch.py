# Copyright mzoesch. All rights reserved.

import os
from Programs.Shared import *


def launch(*args, **kwargs) -> None:
    """
    Arguments:
      - UpdateSubmodules: bool = False // Update submodules before launching.
      - ValidatePython:   bool = False // Validate python version before launching.
      - ValidateGo:       bool = False // Validate go version before launching.
      - ValidatePremake:  bool = False // Validate premake binaries before launching.

    @see Programs/Router.py for subprogram routing.
    """

    error_level: EErrorLevel = EErrorLevel.SUCCESS

    print('Launching reflection wrapper ...')
    last_wd: str = os.getcwd()
    os.chdir(get_abs_engine_root_dir())
    print(f'Changed working directory from [{last_wd}] to [{os.getcwd()}].')

    try:
        from .Router import route_to_subprogram
        error_level = route_to_subprogram(*args, **kwargs)
    except Exception as e:
        print(f'Error: {e}')
        print('Failed to route to subprogram.')
        import traceback
        traceback.print_exc()
        error_level = EErrorLevel.FATAL

    if error_level is None:
        error_level = EErrorLevel.SUCCESS

    os.chdir(last_wd)
    print(f'Popped working directory to [{last_wd}].')

    print(f'Exiting reflection wrapper with {error_level}.')
    exit(error_level.value)


if __name__ == '__main__':
    import sys
    launch(*sys.argv[:])
