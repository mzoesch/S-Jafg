# Copyright mzoesch. All rights reserved.

import shutil
from ..Shared.SharedDefinitions import *
from ..Shared.CommonBuildStuff import *

def copy_content_file_to_bin_dir(config: BuildConfiguration, platform: BuildPlatform) -> bool:
    target_content_dir: str = f'{get_bin_dir(config, platform)}/Content'



    print(f'Copying content files to bin directory...')
    print(f'Source: {get_content_dir()}')
    print(f'Destination: {get_bin_dir(config, platform)}/Content')
    shutil.copytree(get_content_dir(), f'{get_bin_dir(config, platform)}/Content', dirs_exist_ok=True)

    return True

def main(*, config: BuildConfiguration, platform: BuildPlatform, project_type: ProjectType, project_name: str) -> tuple[EErrorLevel, str]:
    print(f'PostBuildTask: {config=}, {platform=}, {project_type=}, {project_name=}')

    if project_type != ProjectType.RUNTIME:
        # Do nothing. The content dir will be copied to the bin dir for the runtime project.
        return EErrorLevel.SUCCESS, ''

    if copy_content_file_to_bin_dir(config, platform) is False:
        return EErrorLevel.ERROR, 'Failed to copy content file to bin directory.'

    return EErrorLevel.SUCCESS, ''
