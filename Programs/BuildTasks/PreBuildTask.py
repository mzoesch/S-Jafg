# Copyright mzoesch. All rights reserved.

from ..Shared.SharedDefinitions import *
from ..Shared.CommonBuildStuff import *

def main(*, config: BuildConfiguration, platform: BuildPlatform) -> tuple[EErrorLevel, str]:
    print(f'PreBuildTask: {config=}, {platform=}')
    return EErrorLevel.SUCCESS, ''
