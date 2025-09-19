# Copyright mzoesch. All rights reserved.

import gdb
import re
from Lal.Lal.Source.Debug.Containers.LalString import (
    LalTStringBase_Printer
    )
from Lal.Lal.Source.Debug.Containers.LalPath import (
    LalTPathBase_Printer
    )
from Lal.Lal.Source.Debug.Containers.JafgArray import (
    JafgTArray_Printer
    )
from Lal.Lal.Source.Debug.Containers.JafgString import (
    JafgTStringBaseStrong_Printer,
    JafgTStringBaseWeak_Printer,
    )
from Lal.Lal.Source.Debug.Containers.LalArray import (
    LalTArrayBase_Printer
    )


def lal_pretty_lookup(val: any) -> any:
    type_str: str = str(val.type.strip_typedefs())

    pattern_lal_array = re.compile(
        r'Lal::TArrayBase<\s*[^>]+\s*>'
        )
    pattern_lal_string = re.compile(
        r'Lal::TStringBase<\s*[^>]+\s*>'
        )
    pattern_lal_path = re.compile(
        r'Lal::TPathBase<\s*[^>]+\s*>'
    )

    # Legacy
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
    # ~Legacy

    if pattern_lal_array.match(type_str):
        return LalTArrayBase_Printer(val)
    if pattern_lal_string.match(type_str):
        return LalTStringBase_Printer(val)
    if pattern_lal_path.match(type_str):
        return LalTPathBase_Printer(val)

    # Legacy
    if pattern_array.match(type_str):
        return JafgTArray_Printer(val)
    if pattern_strong_string.match(type_str):
        return JafgTStringBaseStrong_Printer(val)
    if pattern_weak_string.match(type_str):
        return JafgTStringBaseWeak_Printer(val)
    if pattern_path_base.match(type_str):
        return JafgTStringBaseStrong_Printer(val)
    # ~Legacy

    return None


def on_gdb_launch() -> None:
    print('Adding Lal pretty printers to gdb...')

    gdb.pretty_printers.append(lal_pretty_lookup)

    return None
