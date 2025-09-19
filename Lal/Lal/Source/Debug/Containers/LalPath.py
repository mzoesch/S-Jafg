# Copyright mzoesch. All rights reserved.

import gdb


class LalTPathBase_Printer:
    """
    Pretty printer for:
        Lal::TPathBase<Alloc>
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

        if self.data[str_len - 1] == 0:
            str_len -= 1

        return f'"{self.data.string(length=str_len)}"'

    def display_hint(self):
        return 'string'
