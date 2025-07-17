# Copyright mzoesch. All rights reserved.

import os
import subprocess
from fastapi import HTTPException
from Env import vcs_local_remote_clone, vcs_remote_url

def update_static_motor_impl() -> None:
    if not os.path.exists(vcs_local_remote_clone):
        popen = subprocess.Popen(
            ['git', 'clone', vcs_remote_url, vcs_local_remote_clone],
            )
        popen.wait()
        if popen.returncode != 0:
            raise HTTPException(status_code=500)

    if not os.path.exists(vcs_local_remote_clone):
        raise HTTPException(status_code=500)

    cwd = os.getcwd()
    try:
        os.chdir(vcs_local_remote_clone)

        popen = subprocess.Popen(
            ['git', 'pull']
            )
        popen.wait()
        if popen.returncode != 0:
            raise HTTPException(status_code=500, detail='Failed to pull from remote')

        if not os.path.exists('Motor'):
            raise HTTPException(status_code=500)

    finally:
        os.chdir(cwd)

    commands = [
        ['cargo', 'build', '--release', '--target', 'x86_64-pc-windows-gnu'],
        ['cargo', 'build', '--release', '--target', 'x86_64-unknown-linux-gnu']
        ]

    try:
        for command in commands:
            result = subprocess.Popen(
                command,
                cwd=f'{vcs_local_remote_clone}/Motor',
                )
            result.wait()
            if result.returncode != 0:
                raise HTTPException(status_code=500, detail=f'Build failed for target [{command[len(command) - 1]}.')

            continue

    except Exception:
        raise HTTPException(status_code=500)

    return None


def update_static_motor() -> None:
    print('Updating static motor bins...')

    try:
        update_static_motor_impl()
    except Exception as e:
        if isinstance(e, HTTPException):
            raise e
        raise HTTPException(status_code=500)

    return None


if __name__ == '__main__':
    """
    Development entry point.
    """
    update_static_motor()
