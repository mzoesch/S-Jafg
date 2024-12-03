# Copyright mzoesch. All rights reserved.

import sys
import subprocess
import threading

def stream_output(stream, output_func):
    counter: int = 0
    for line in iter(stream.readline, ''):
        output_func(line)
        sys.stdout.flush()

    stream.close()

def run_subprocess(*args) -> None:
    """
    Emits live output of the stdout / stderr.
    """

    with subprocess.Popen(
        args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    ) as p:

        stdout_thread = threading.Thread(target=stream_output, args=(p.stdout, lambda line: print(line, end='')))
        stderr_thread = threading.Thread(target=stream_output, args=(p.stderr, lambda line: print(line, file=sys.stderr, end='')))

        stdout_thread.start()
        stderr_thread.start()
        p.wait()
        stdout_thread.join()
        stderr_thread.join()

        if p.returncode != 0:
            print(f'Subprocess failed with return code {p.returncode=}.')
            sys.exit(p.returncode)

    return None

if __name__ == '__main__':
    import sys
    run_subprocess(*sys.argv[1:])
    sys.exit(0)
