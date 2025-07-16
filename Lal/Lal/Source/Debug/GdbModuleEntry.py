# Copyright mzoesch. All rights reserved.

import gdb
import re
from Lal.Lal.Source.Debug.Containers.JafgArray import (
    JafgTArray_Printer
    )
from Lal.Lal.Source.Debug.Containers.JafgString import (
    JafgTStringBaseStrong_Printer,
    JafgTStringBaseWeak_Printer,
    )


def lal_pretty_lookup(val: any) -> any:
    type_str: str = str(val.type.strip_typedefs())

    pattern_array = re.compile(
        r'Jafg::TArrayBase<\s*[^>]+\s*>'
        )
    pattern_strong_string = re.compile(
        r'Jafg::_TStringBase<\s*Jafg::TStringTraits<[^>]+>,\s*Jafg::TArrayBase<\s*(Jafg::TArrayAllocator<[^>]+>)\s*>\s*>'
        )
    pattern_weak_string = re.compile(
        r'Jafg::_TStringBase<\s*Jafg::TStringTraits<[^>]+>,\s*Jafg::TArrayBase<\s*(Jafg::TArrayViewAllocator<[^>]+>)\s*>\s*>'
        )
    pattern_path_base = re.compile(
        r'Jafg::TPathBase<\s*Jafg::TStringTraits<[^>]+>,\s*Jafg::TArrayBase<\s*(Jafg::TArrayAllocator<[^>]+>)\s*>\s*>'
        )

    if pattern_array.match(type_str):
        return JafgTArray_Printer(val)
    if pattern_strong_string.match(type_str):
        return JafgTStringBaseStrong_Printer(val)
    if pattern_weak_string.match(type_str):
        return JafgTStringBaseWeak_Printer(val)
    if pattern_path_base.match(type_str):
        return JafgTStringBaseStrong_Printer(val)

    return None


def on_gdb_launch() -> None:
    print('Adding Lal pretty printers to gdb...')

    gdb.pretty_printers.append(lal_pretty_lookup)

    return None
