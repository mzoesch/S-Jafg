# Copyright mzoesch. All rights reserved.

"""
@see Programs.Launch.py for launch arguments.
"""
if __name__ == '__main__':
    import sys
    from Programs import Launch as launch
    launch.Launch(*sys.argv[:])
