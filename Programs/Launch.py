# Copyright mzoesch. All rights reserved.

import argparse
import os
import time
from Programs.Shared import *
from Programs.Meta.Application import GApp

def launch(*args, **kwargs) -> None:
    error_level: EErrorLevel = EErrorLevel.SUCCESS

    start: float = time.time() 

    print('Launching reflection wrapper ...')
    last_wd: str = os.getcwd()
    os.chdir(get_abs_engine_root_dir())
    print(f'Changed working directory from [{last_wd}] to [{os.getcwd()}].')

    GApp._pull_cache()

    try:
        parser = argparse.ArgumentParser(description='Wrapper for reflection and other non core engine features.')
        parser.add_argument('-UpdateSubmodules',   action='store_true',      help='Update git submodules before launching.')
        parser.add_argument('-ValidatePython',     action='store_true',      help='Validate python version before launching and install packages into this venv.')
        parser.add_argument('-ValidateRust',       action='store_true',      help='Validate rust version before launching.')
        parser.add_argument('-ValidatePremake',    action='store_true',      help='Validate premake binaries before launching.')
        parser.add_argument('-ValidateCmake',      action='store_true',      help='Validate cmake binaries before launching.')
        parser.add_argument('-UpdateCachedData',   action='store_true',      help='Updates target- and module-infos.')
        parser.add_argument('-DevRun',             action='store_true',      help='Does nothing. Relevant only for development.')
        parser.add_argument('-INVOKE',             action='store_true',      help='Invokes the reflection and meta program. All args after this argument are forwarded. Use with "--".')
        parser.add_argument('InvokeArgs',          nargs=argparse.REMAINDER, help='All args that are after this argument are forwarded. Preceding arguments will be discarded.')
        args = parser.parse_args(args=args)

        if (args.UpdateSubmodules is False and args.ValidatePython   is False and
            args.ValidateRust     is False and args.ValidatePremake  is False and
            args.ValidateCmake    is False and args.UpdateCachedData is False and
            args.DevRun           is False and args.INVOKE           is False):
            raise RuntimeError('No action specified.')

        from .Router import route_to_subprogram
        error_level = route_to_subprogram(args)
    except Exception as e:
        print(f'Error: {e}')
        print('Failed to route to subprogram.')
        import traceback
        traceback.print_exc()
        error_level = EErrorLevel.FATAL
    finally:
        GApp._push_cache()
        print(f'Program exited after {time.time() - start}s.')

    if error_level is None:
        error_level = EErrorLevel.SUCCESS

    os.chdir(last_wd)
    print(f'Popped working directory to [{last_wd}].')

    print(f'Exiting reflection wrapper with {error_level}.')
    exit(error_level.value)


if __name__ == '__main__':
    import sys
    launch(*sys.argv[:])
