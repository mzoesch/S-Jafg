# Copyright mzoesch. All rights reserved.

import gdb


class JafgTStringBaseStrong_Printer:
    """
    Pretty printer when using a strong allocation strategy for:
        Jafg::TStringBase<Derived, Traits, Alloc>
        Jafg::_TStringBase<Traits, Alloc>
        Jafg::TPathBase<Traits, Alloc>
    """

    def __init__(self, val):
        self.val = val
        self.data = val['Impl']['Impl']['Data']
        self.slack = val['Impl']['Impl']['Slack']

        return

    def to_string(self):
        str_len = int(self.slack - self.data)

        if str_len <= 0:
            return '""'

        # Minus one, because the string is null-terminated.
        return f'"{self.data.string(length=str_len - 1)}"'

    def display_hint(self):
        return 'string'


class JafgTStringBaseWeak_Printer:
    """
    Pretty printer when using a weak allocation strategy for:
        Jafg::TStringBase<Derived, Traits, Alloc>
        Jafg::_TStringBase<Traits, Alloc>
        Jafg::TPathBase<Traits, Alloc>
    """

    def __init__(self, val):
        self.val = val
        self.data = val['Impl']['Impl']['Data']
        self.slack = val['Impl']['Impl']['Slack']

        return

    def to_string(self):
        str_len = int(self.slack - self.data)

        if self.data == 0 or str_len <= 0:
            return '""'

        return f'"{self.data.string(length=str_len)}"'

    def display_hint(self):
        return 'string'
