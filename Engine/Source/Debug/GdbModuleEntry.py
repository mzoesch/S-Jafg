# Copyright mzoesch. All rights reserved.

import gdb
import re


class JafgPrettyPrinter:
    def __init__(self, val, str_repr):
        self.val = val
        self.str_repr = str_repr
        return

    def is_applicable(self) -> bool:
        for pattern in self.patterns:
            match = re.search(pattern, self.str_repr)
            if match:
                self.on_applicable(match)
                return True
        return False

    def on_applicable(self, match):
        pass


class JafgPrettyStringPrinter(JafgPrettyPrinter):
    def display_hint(self):
        return 'string'


class GlmVec_Printer(JafgPrettyStringPrinter):
    def __init__(self, val, str_repr):
        super().__init__(val, str_repr)
        self.domain = None
        self.patterns = ['^glm::vec<(?P<domain>(\d)),\s[\w\s]+,\s\(glm::qualifier\)\d>$',]
        return

    def on_applicable(self, match):
        self.domain = match.group('domain')

    def to_string(self):
        if self.domain is None:
            return 'Invalid vector type'
        if self.domain == '2':
            return 'vec2({}, {})'.format(self.val['x'], self.val['y'])
        elif self.domain == '3':
            return 'vec3({}, {}, {})'.format(self.val['x'], self.val['y'], self.val['z'])
        elif self.domain == '4':
            return 'vec4({}, {}, {}, {})'.format(self.val['x'], self.val['y'], self.val['z'], self.val['w'])
        else:
            return None


class Color_Printer(JafgPrettyStringPrinter):
    def __init__(self, val, str_repr):
        super().__init__(val, str_repr)
        self.patterns = ['^LColor$',]
        return

    def to_string(self):
        return 'RGBA: 0x{:02X}{:02X}{:02X}{:02X}'.format(
            int(self.val['R'].cast(gdb.lookup_type('unsigned int'))),
            int(self.val['G'].cast(gdb.lookup_type('unsigned int'))),
            int(self.val['B'].cast(gdb.lookup_type('unsigned int'))),
            int(self.val['A'].cast(gdb.lookup_type('unsigned int')))
            )


class LinearColor_Printer(JafgPrettyStringPrinter):
    def __init__(self, val, str_repr):
        super().__init__(val, str_repr)
        self.patterns = ['^LLinearColor$',]
        return

    def to_string(self):
        return 'RGBA: {:.3f}, {:.3f}, {:.3f}, {:.3f}'.format(
            float(self.val['R']), float(self.val['G']), float(self.val['B']), float(self.val['A'])
            )


class JafgWhitespace_Printer(JafgPrettyStringPrinter):
    def __init__(self, val, str_repr):
        super().__init__(val, str_repr)
        self.patterns = ['^Jafg::LWhitespace$',]
        return

    def to_string(self):
        return '{} "vec4({}, {}, {}, {})"'.format(
            self.val['Type'],
            self.val['Left'], self.val['Top'], self.val['Right'], self.val['Bottom']
            )


class JafgWidgetSize_Printer(JafgPrettyStringPrinter):
    def __init__(self, val, str_repr):
        super().__init__(val, str_repr)
        self.domain = None
        self.patterns = ['^Jafg::LWidgetSize(?P<domain>\d)$',]
        return

    def on_applicable(self, match):
        self.domain = match.group('domain')

    def to_string(self):
        if self.domain is None:
            return 'Invalid widget size type'
        return '{} {}'.format(self.val['Type'], self.val['Size'])


def jafg_pretty_lookup(val: any) -> any:
    type_str: str = str(val.type.strip_typedefs())
    print(f'WHAT: {type_str}')

    printers = [
        GlmVec_Printer,
        Color_Printer,
        LinearColor_Printer,
        JafgWhitespace_Printer,
        JafgWidgetSize_Printer,
        ]

    for printer in printers:
        instance = printer(val, type_str)
        if instance.is_applicable():
            return instance
        continue
    return None


def on_gdb_launch() -> None:
    print('Adding Jafg pretty printers to gdb...')
    gdb.pretty_printers.append(jafg_pretty_lookup)
    return None
