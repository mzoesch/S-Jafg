# Copyright mzoesch. All rights reserved.

def run(*args, **kwargs) -> None:
    """Will call the main function of the program lib."""
    from Programs import Router as router

    router.route_to_program(*args, **kwargs)

    return None

"""
Valid arguments:
    --install-deps      : Will install the dependencies for the program.
    --compile-scripts   : Will compile the scripts for the program.
    --generate-solution : Will generate the solution for the program.
    --fwd               : Will just forward args to go.

For more arguments used by other scripts, see Programs/Main.go.
"""
if __name__ == '__main__':
    import sys
    run(*sys.argv)
