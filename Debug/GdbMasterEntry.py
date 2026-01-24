# Copyright mzoesch. All rights reserved.

import os
import sys
import importlib.util
from pathlib import Path

def master_entry() -> None:
    """
    The master entry point for this Gdb jafg customization script form the engine root directory of the .gdbinit file.
    """

    engine_root: str = os.path.dirname( Path(f'{__file__}').parent)
    sys.path.insert(0,engine_root)
    print(f'Added engine root [{engine_root}] to sys.path.')

    lookups = [
        'Engine',
        'Runtime',

        # Third party libraries last.
        'Plugins',
        ]

    for lookup in lookups:
        for root, _, fs in os.walk(lookup):
            for f in fs:
                p = os.path.join(root, f)
                if not p.endswith('Source/Debug/GdbModuleEntry.py'):
                    continue

                module_name: str = str(p).replace('/', '_').replace('\\', '_').replace('.', '_')

                spec = importlib.util.spec_from_file_location(module_name, p)
                module = importlib.util.module_from_spec(spec)
                sys.modules[module_name] = module
                spec.loader.exec_module(module)

                print(f'Executing on_gdb_launch for [{module_name}]...')

                try:
                    module.on_gdb_launch()
                except AttributeError as e:
                    print(f'Error while executing on_gdb_launch for [{module_name}]: [{e}] - skipping this module.')
                    continue

                continue
            continue
        continue

    return None
