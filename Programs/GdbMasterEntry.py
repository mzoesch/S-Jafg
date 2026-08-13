# Copyright mzoesch. All rights reserved.

import os
import sys
import importlib.util
from pathlib import Path

def master_entry(scripts: list[str]) -> None:
    engine_root: str = os.path.dirname( Path(f'{__file__}').parent)
    sys.path.insert(0,engine_root)
    print(f'Added engine root [{engine_root}] to sys.path.')

    for script in scripts:
        module_name: str = str(script).replace('/', '_').replace('\\', '_').replace('.', '_')

        spec = importlib.util.spec_from_file_location(module_name, script)
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

    nlohmann_module_file: str = 'Vendor/nlohmannjson/tools/gdb_pretty_printer/nlohmann-json.py'
    nlohmann_module: str = str(nlohmann_module_file).replace('/', '_').replace('\\', '_').replace('.', '_')
    spec = importlib.util.spec_from_file_location(nlohmann_module, nlohmann_module_file)
    module = importlib.util.module_from_spec(spec)
    sys.modules[nlohmann_module] = module
    spec.loader.exec_module(module)
    print(f'Executed nlohmann-json.py for pretty printing nlohmann::json types.')

    return None
