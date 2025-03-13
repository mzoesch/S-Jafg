# Copyright mzoesch. All rights reserved.

from enum import Enum


class PchUsage(Enum):
    ALLOW = 1
    PROHIBIT = 0


def pch_usage_to_bool(pch_usage: PchUsage) -> bool:
    if pch_usage == PchUsage.ALLOW:
        return True

    if pch_usage == PchUsage.PROHIBIT:
        return False

    raise ValueError(f'Cannot convert [{pch_usage}] to bool.')


def pch_usage_to_string(pch_usage: PchUsage) -> str:
    if pch_usage == PchUsage.ALLOW:
        return 'Allow'

    if pch_usage == PchUsage.PROHIBIT:
        return 'Prohibit'

    return 'Unknown'


def volatile_pch_usage_to_string(pch_usage: PchUsage) -> str:
    if pch_usage == PchUsage.ALLOW:
        return 'Allow'

    if pch_usage == PchUsage.PROHIBIT:
        return 'Prohibit'

    raise ValueError('Unknown pch_usage in volatile_pch_usage_to_string.')
