# Copyright mzoesch. All rights reserved.

if __name__ == '__main__':
    """
    @see Programs/Launch.py for launch arguments.
    """
    import sys
    from Programs import Launch as launch
    launch.launch(*sys.argv[1:])
