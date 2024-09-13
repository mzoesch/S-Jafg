# Copyright mzoesch. All rights reserved.

import subprocess

def run_subprocess(*args) -> None:
    p : subprocess = subprocess.run(
        args,
        check=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )

    print(p.stdout.decode('utf-8'), end='')
    print(p.stderr.decode('utf-8'), end='')

    return None

if __name__ == '__main__':
    import sys
    run_subprocess(*sys.argv[1:])
    sys.exit(0)
