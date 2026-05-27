# Copyright mzoesch. All rights reserved.

import math


def get_srgb2linear() -> list[str]:
    result = []
    for i in range(256):
        c = i / 255
        if c > 0.04045:
            c = ((c + 0.055) / 1.055) ** 2.4
        else:
            c = c / 12.92
        result.append(f'{c:.32f}')
    return result


if __name__ == '__main__':
    print('{')
    for idx, repr in enumerate(get_srgb2linear()):
        if idx % 3 == 0:
            print(f'{repr},', end='\n    ')
        else:
            print(f'{repr},', end='')
    print('}')
