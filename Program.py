# Copyright mzoesch. All rights reserved.

def run(*args, **kwargs) -> None:
    """Will call the main function of the program lib."""
    from Programs import Router as router

    router.route_to_program(*args, **kwargs)

    return None

if __name__ == '__main__':
    import sys
    run(*sys.argv)
