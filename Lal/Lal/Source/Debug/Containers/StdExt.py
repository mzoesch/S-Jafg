# Copyright mzoesch. All rights reserved.

import gdb


class LalTSimpleString_Printer:
    """
    Pretty printer for:
        Lal::TSimpleString<TChar, TTraits, TAllocator>
    """

    def __init__(self, val):
        self.val = val
        return

    def to_string(self):
        std_string_type = gdb.lookup_type("std::string")
        base_val = self.val.cast(std_string_type)

        pp = gdb.default_visualizer(base_val)
        if pp is not None:
            return pp.to_string()

        return str(base_val)

    def display_hint(self):
        return 'string'


# class LalTSimpleArray_Printer:
#     def __init__(self, val):
#         self.val = val
#
#     def to_string(self):
#         result = gdb.parse_and_eval(f'({self.val.address}).size()')
#         return str(result)
#
#     def display_hint(self):
#         return 'array'
