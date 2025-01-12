# Copyright mzoesch. All rights reserved.

from enum import Enum


class BuildConfiguration(Enum):
    UNKNOWN = 0
    DEBUG = 1
    DEVELOPMENT = 2
    SHIPPING = 3


def build_configuration_to_string(config: BuildConfiguration) -> str:
    if config == BuildConfiguration.DEBUG:
        return 'Debug'

    if config == BuildConfiguration.DEVELOPMENT:
        return 'Development'

    if config == BuildConfiguration.SHIPPING:
        return 'Shipping'

    return 'Unknown'


def build_configuration_to_define(config: BuildConfiguration) -> str:
    if config == BuildConfiguration.DEBUG:
        return 'IN_DEBUG'

    if config == BuildConfiguration.DEVELOPMENT:
        return 'IN_DEVELOPMENT'

    if config == BuildConfiguration.SHIPPING:
        return 'IN_SHIPPING'

    raise Exception('Unknown build configuration')
