# Copyright mzoesch. All rights reserved.

import os
import sys
import threading
import subprocess


def _stream_output(stream, output_func) -> None:
    for line in iter(stream.readline, ''):
        output_func(line)
        sys.stdout.flush()

    stream.close()
    return None


def _copy_and_print(line, copied_stdout) -> None:
    print(line, end='')
    if copied_stdout is not None:
        copied_stdout.append(line)

    return None


def run_any_task(*args, wd=None, shell=False) -> None:
    """
    Emits live output of the stdout / stderr.
    """

    cwd = None
    if wd is not None:
        cwd = os.getcwd()
        os.chdir(wd)
        print(f'Changed working directory from [{cwd}] to [{os.getcwd()}] to execute subprocess.')

    print(f'Running subprocess with args [{" ".join(args)}].')
    try:
        with subprocess.Popen(
                args,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                shell=shell
        ) as p:
            stdout_thread = threading.Thread(target=_stream_output,
                                            args=(p.stdout, lambda line: print(line, end='')))
            stderr_thread = threading.Thread(target=_stream_output,
                                            args=(p.stderr, lambda line: print(line, end='')))

            stdout_thread.start()
            stderr_thread.start()
            p.wait()
            stdout_thread.join()
            stderr_thread.join()

            if p.returncode != 0:
                print(f'Subprocess failed with {p.returncode=}.')
                if cwd is not None:
                    os.chdir(cwd)
                    cwd = None
                    print(f'Popped working directory to [{os.getcwd()}].')
                sys.exit(p.returncode)
    finally:
        if cwd is not None:
            os.chdir(cwd)
            print(f'Popped working directory to [{os.getcwd()}].')

    return None


def run_any_task_with_stdout(copied_stdout, *args) -> None:
    """
    Emits live output of the stdout / stderr.
    """

    print(f'Running subprocess with args [{" ".join(args)}].')
    with subprocess.Popen(
            args,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
    ) as p:

        stdout_thread = threading.Thread(target=_stream_output,
                                         args=(
                                             p.stdout,
                                             lambda line: _copy_and_print(line, copied_stdout)
                                         ))
        stderr_thread = threading.Thread(target=_stream_output,
                                         args=(
                                             p.stderr,
                                             lambda line: _copy_and_print(line, copied_stdout)
                                         ))

        stdout_thread.start()
        stderr_thread.start()
        p.wait()
        stdout_thread.join()
        stderr_thread.join()

        if p.returncode != 0:
            print(f'Subprocess failed with {p.returncode=}.')
            sys.exit(p.returncode)

    return None


def run_any_task_ok_to_fail(*args) -> int:
    """
    Emits live output of the stdout / stderr.
    """

    print(f'Running subprocess with args [{" ".join(args)}].')
    with subprocess.Popen(
            args,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            shell=True
    ) as p:
        stdout_thread = threading.Thread(target=_stream_output,
                                         args=(p.stdout, lambda line: print(line, end='')))
        stderr_thread = threading.Thread(target=_stream_output,
                                         args=(p.stderr, lambda line: print(line, end='')))

        stdout_thread.start()
        stderr_thread.start()
        p.wait()
        stdout_thread.join()
        stderr_thread.join()

        if p.returncode != 0:
            print(f'Subprocess failed with {p.returncode=}.')
            return p.returncode

    return 0


def run_any_task_with_stdout_ok_to_fail(copied_stdout, *args) -> int:
    """
    Emits live output of the stdout / stderr.
    """

    print(f'Running subprocess with args [{" ".join(args)}].')
    with subprocess.Popen(
            args,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
    ) as p:

        stdout_thread = threading.Thread(target=_stream_output,
                                         args=(
                                             p.stdout,
                                             lambda line: _copy_and_print(line, copied_stdout)
                                         ))
        stderr_thread = threading.Thread(target=_stream_output,
                                         args=(
                                             p.stderr,
                                             lambda line: _copy_and_print(line, copied_stdout)
                                         ))

        stdout_thread.start()
        stderr_thread.start()
        p.wait()
        stdout_thread.join()
        stderr_thread.join()

        if p.returncode != 0:
            print(f'Subprocess failed with {p.returncode=}.')
            return p.returncode

    return 0
