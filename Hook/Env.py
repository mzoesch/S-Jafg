# Copyright mzoesch. All rights reserved.

import os

# Secret transmitted by the GitHub webhook imported from the local environment.
secret_x_hub_signature = os.getenv('SECRET_X_HUB_SIGNATURE')
if secret_x_hub_signature is None:
    raise ValueError('Environment variable SECRET_X_HUB_SIGNATURE is not set')
if secret_x_hub_signature == b'':
    raise ValueError('Environment variable SECRET_X_HUB_SIGNATURE is empty')
secret_x_hub_signature = secret_x_hub_signature.encode('utf-8')

# Local path to the directory where the static motor binaries are cached between runs.
volume_cache = os.getenv('VOLUME_CACHE')
if volume_cache is None:
    raise ValueError('Environment variable VOLUME_CACHE is not set')
if volume_cache == '':
    raise ValueError('Environment variable VOLUME_CACHE is empty')

# Local path to the vcs repository that is cloned and updated and contains the Motor source code.
vcs_local_remote_clone = os.getenv('VCS_LOCAL_REMOTE_CLONE')
if vcs_local_remote_clone is None:
    raise ValueError('Environment variable VCS_LOCAL_REMOTE_CLONE is not set')
if vcs_local_remote_clone == '':
    raise ValueError('Environment variable VCS_LOCAL_REMOTE_CLONE is empty')
vcs_local_remote_clone = f'{volume_cache}/{vcs_local_remote_clone}'

# Remote URL of the vcs repository that is cloned and updated and contains the Motor source code.
vcs_remote_url = os.getenv('VCS_REMOTE_URL')
if vcs_remote_url is None:
    raise ValueError('Environment variable VCS_REMOTE_URL is not set')
if vcs_remote_url == '':
    raise ValueError('Environment variable VCS_REMOTE_URL is empty')
