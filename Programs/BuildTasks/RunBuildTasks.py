# Copyright mzoesch. All rights reserved.

from ..Shared.SharedDefinitions import *
from ..Shared.CommonBuildStuff import *

def parse_args(*args: str) -> tuple[bool, bool, BuildPlatform, BuildConfiguration, ProjectType, str]:
    is_prebuild: bool  = False
    is_postbuild: bool = False
    platform: BuildPlatform = BuildPlatform.UNKNOWN
    configuration: BuildConfiguration = BuildConfiguration.UNKNOWN
    project_type: ProjectType = ProjectType.UNKNOWN
    project_name: str = ''

    for arg in args:
        if 'prebuild' in arg:
            is_prebuild = True
            continue

        if 'postbuild' in arg:
            is_postbuild = True
            continue

        if 'Client' in arg:
            platform = BuildPlatform.CLIENT
            continue

        if 'Server' in arg:
            platform = BuildPlatform.SERVER
            continue

        if 'Debug' in arg:
            configuration = BuildConfiguration.DEBUG
            continue

        if 'Development' in arg:
            configuration = BuildConfiguration.DEVELOPMENT
            continue

        if 'Shipping' in arg:
            configuration = BuildConfiguration.SHIPPING
            continue

        if 'Static-Core' in arg:
            project_type = ProjectType.STATIC_CORE
            continue

        if 'Runtime' in arg:
            project_type = ProjectType.RUNTIME
            continue

        # Currently we do not have plugins but in the future we will.
        # How can we detect this? We could parse the project location or use the target kind, meaning console app,
        # static library, etc. We will probably need the dynamic link library for plugins.

        continue

    return is_prebuild, is_postbuild, platform, configuration, project_type, project_name

def main(*args, **kwargs) -> None:
    is_prebuild, is_postbuild, platform, configuration, project_type, project_name = parse_args(*args)

    if is_prebuild is False and is_postbuild is False:
        raise ValueError('No build task specified. Please specify either prebuild or postbuild.')
        return None

    if is_prebuild and is_postbuild:
        raise ValueError('Cannot specify both prebuild and postbuild. Please specify only one.')
        return None

    if platform == BuildPlatform.UNKNOWN:
        raise ValueError('No platform specified. Please specify either client or server.')
        return None

    if configuration == BuildConfiguration.UNKNOWN:
        raise ValueError('No configuration specified. Please specify either debug, development, or shipping.')
        return None

    if project_type == ProjectType.UNKNOWN:
        raise ValueError('No project type specified. Please specify either static-core or runtime.')
        return None

    if is_prebuild:
        from .PreBuildTask import main as prebuild_main
        error_level, error_msg = prebuild_main(config=configuration, platform=platform)

        if error_level != EErrorLevel.SUCCESS:
            raise RuntimeError(f'PreBuildTask failed with error level {error_level} and message {error_msg}.')

        return None

    if is_postbuild:
        from .PostBuildTask import main as postbuild_main
        error_level, error_msg = postbuild_main(config=configuration, platform=platform, project_type=project_type, project_name=project_name)

        if error_level != EErrorLevel.SUCCESS:
            raise RuntimeError(f'PostBuildTask failed with error level {error_level} and message {error_msg}.')

        return None

    raise NotImplementedError('Enclosing block should never be entered.') 
    return None
