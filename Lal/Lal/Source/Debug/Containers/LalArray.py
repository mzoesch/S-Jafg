# Copyright mzoesch. All rights reserved.

import gdb


class LalTArrayBase_Printer:
    """
    Pretty printer for:
        Lal::TArrayBase<Alloc>
    """

    def __init__(self, val):
        self.val = val
        self.data = val['Impl']['Data']
        self.slack = val['Impl']['Slack']
        self.end = val['Impl']['End']

        self.size = int(self.slack - self.data)
        self.capacity = int(self.end - self.data)

        return

    def to_string(self):
        return f'{{length {self.size}, capacity {self.capacity}}}'

    def children(self):
        elem_t = self.data.type.strip_typedefs().target()

        is_const: bool = False
        if elem_t.__str__().startswith('const '):
            is_const = True

        # Do not use .strip_typedefs() here, because we want to keep typedefs.
        # Only strip the T typename from Alloc::T.
        if elem_t.code == gdb.TYPE_CODE_TYPEDEF:
            elem_t = elem_t.target()

        if is_const:
            elem_t = elem_t.const()

        for i in range(self.size):
            elem = (self.data + i).dereference()
            elem = elem.cast(elem_t)

            yield f'[{i}]', elem

    def display_hint(self):
        return 'array'
