# Copyright mzoesch. All rights reserved.


api_prefix = 'JAFG'
detail_api_prefix = 'DETAIL_JAFG'


def generate_map_macros(N: int):
    for n in range(1, N + 1):
        args = ', '.join(['Transform'] + [f'\\\n    What{i}'if i % 10 == 1 else f'What{i}' for i in range(1, n + 1)])
        body = ' '.join(
            [f'\\\n    Transform(What{i})' if i % 10 == 1 else f'Transform(What{i})' for i in range(1, n + 1)]
            )
        print(f'#define {detail_api_prefix}_MAP_{n}({args}) {body}')
    print(f'')
    print(f'')
    print(f'#define {detail_api_prefix}_GET_MAP( ', end='')
    for n in range(1, N + 1):
        if n % 10 == 1:
            print(f'\\\n    ', end='')
        print(f'_{n}, ', end='')
    print('\\\n    NAME, ...) NAME')
    print(f'')
    print(f'')
    print(f'#define {api_prefix}_MAP(Transform, ...) \\')
    print(f'    {detail_api_prefix}_GET_MAP(__VA_ARGS__, ', end='')
    for n in range(N, 1, -1):
        if n % 10 == 0:
            print(f'\\\n    ', end='')
        print(f'{detail_api_prefix}_MAP_{n}, ', end='')
    print(f'{detail_api_prefix}_MAP_1) \\\n    (Transform, __VA_ARGS__)')


if __name__ == '__main__':
    generate_map_macros(512)
