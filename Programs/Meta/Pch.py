# Copyright mzoesch. All rights reserved.

from enum import Enum


class PchUsage(Enum):
    ALLOW = 1
    PROHIBIT = 0


def pch_usage_to_string(pch_usage: PchUsage) -> str:
    if pch_usage == PchUsage.ALLOW:
        return 'ALLOW'

    if pch_usage == PchUsage.PROHIBIT:
        return 'PROHIBIT'

    return 'Unknown'

def volatile_pch_usage_to_string(pch_usage: PchUsage) -> str:
    if pch_usage == PchUsage.ALLOW:
        return 'ALLOW'

    if pch_usage == PchUsage.PROHIBIT:
        return 'PROHIBIT'

    raise ValueError('Unknown pch_usage in volatile_pch_usage_to_string.')
