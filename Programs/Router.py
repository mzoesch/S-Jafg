# Copyright mzoesch. All rights reserved.

def run_program(program: str, *args, **kwargs) -> bool:
    if program == 'BuildTool':
        from Programs.BuildTool import RunBuildTool as rbt
        rbt.main(*args, **kwargs)
        return True

    if program == 'BuildTask':
        from Programs.BuildTasks import RunBuildTasks as rbt
        rbt.main(*args, **kwargs)
        return True

    return False

def parse_args_to_kwargs(*args: list[str]) -> dict[str, str]: 
    """Will convert a list of arguments to a dictionary of keyword arguments."""

    kwargs = {}

    for arg in args:
        if '=' in arg:
            key, value = arg.split('=', 1)
            kwargs[key] = value

    return kwargs


def route_to_program(*args, **kwargs) -> None:
    """Will call the main function of the program to run in this library."""

    print(f'Try running program with: {args=}.')

    if 'run' in kwargs:
        program = kwargs['run']

        if run_program(program, *args, **kwargs):
            return None

    parsed_kwargs = parse_args_to_kwargs(*args)
    if not 'run' in parsed_kwargs:
        print(f'No program to run {args=} with {parsed_kwargs=}.')
        return None

    if not run_program(parsed_kwargs['run'], *args, **kwargs):
        print(f'Could not run or find program {parsed_kwargs["run"]=}.')

    return None

if __name__ == '__name__':
    import sys
    route_to_program(*sys.argv)
