# Copyright mzoesch. All rights reserved.

from enum import Enum

class BuildPlatform(Enum):
    UNKNOWN = 0
    CLIENT  = 1
    SERVER  = 2

class BuildConfiguration(Enum):
    UNKNOWN     = 0
    DEBUG       = 1
    DEVELOPMENT = 2
    SHIPPING    = 3

class ProjectType(Enum):
    UNKNOWN     = 0
    STATIC_CORE = 1
    RUNTIME     = 2
    PLUGIN      = 3
    OTHER       = 4 # Currently ignored by this build system.

def build_platform_to_string(platform: BuildPlatform) -> str:
    if platform == BuildPlatform.CLIENT:
        return 'Client'

    if platform == BuildPlatform.SERVER:
        return 'Server'

    return 'Unknown'

def build_configuration_to_string(config: BuildConfiguration) -> str:
    if config == BuildConfiguration.DEBUG:
        return 'Debug'

    if config == BuildConfiguration.DEVELOPMENT:
        return 'Development'

    if config == BuildConfiguration.SHIPPING:
        return 'Shipping'

    return 'Unknown'
