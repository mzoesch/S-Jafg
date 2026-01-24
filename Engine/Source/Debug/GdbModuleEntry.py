# Copyright mzoesch. All rights reserved.
from re import Pattern

import gdb
import re


class JafgTSimpleString_Printer:
    """
    Pretty printer for:
        Jafg::TSimpleString<TChar, TTraits, TAllocator>
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


def jafg_pretty_lookup(val: any) -> any:
    type_str: str = str(val.type.strip_typedefs())

    pattern_jafg_simple_string = re.compile(
        r'.*Jafg::TSimpleString<\s*[^>]+\s*>'
        )

    if pattern_jafg_simple_string.match(type_str):
        return JafgTSimpleString_Printer(val)

    return None


def on_gdb_launch() -> None:
    print('Adding Jafg pretty printers to gdb...')

    gdb.pretty_printers.append(jafg_pretty_lookup)

    return None
