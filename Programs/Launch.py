# Copyright mzoesch. All rights reserved.

"""
"""
def Launch(*args, **kwargs) -> None:
    print("Hello from python.")
    return None

if __name__ == '__main__':
    import sys
    Launch(*sys.argv[:])
