# Copyright mzoesch. All rights reserved.

from ..Shared.SharedDefinitions import *

def parse_args(*args: str) -> tuple[bool, bool]:
    do_setup: bool    = False
    do_generate: bool = False

    for arg in args:
        if 'install-deps' in arg:
            do_setup = True
            continue

        if 'generate-solution' in arg:
            do_generate = True
            continue

        continue

    return do_setup, do_generate


def main(*args, **kwargs) -> None:
    """Will call the main function of the program lib."""

    do_setup, do_generate = parse_args(*args)

    if 'install-deps' in kwargs:
        do_setup = True
    if 'generate-solution' in kwargs:
        do_generate = True

    if do_setup == False and do_generate == False:
        print(f'RunBuildTool: Invalid args provided {args=} with {kwargs=}.')
        return None

    print(f'RunBuildTool: Will run with {do_setup=}, {do_generate=}.')

    error_level: EErrorLevel = EErrorLevel.SUCCESS

    if do_setup:
        from Programs.BuildTool import InstallDependencies as Setup
        error_level = Setup.run()

    if error_level != EErrorLevel.SUCCESS:
        print(f'Quit with error level {error_level=}.')
        return None
    
    if do_generate:
        from Programs.BuildTool import GenerateSolutionFiles as Generate
        error_level = Generate.run()

    if error_level != EErrorLevel.SUCCESS:
        print(f'Quit with error level {error_level=}.')
        return None

    return None
