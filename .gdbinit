# Copyright mzoesch. All rights reserved.

# Please see Documentation/GdbDebug.md for details on how to load this file.

python

import sys
import importlib.util
from pathlib import Path

def jafg_load_master_gdb_init() -> None:
    master_path = Path('Programs/GdbMasterEntry.py').resolve()
    module_name = f'PRIVATE_JAFG_{master_path.stem}'

    spec = importlib.util.spec_from_file_location(module_name, master_path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[module_name] = module
    spec.loader.exec_module(module)

    module.master_entry([
        # Add your own pretty printers here.
        # For security reasons jafg will no longer detect pretty printers in engine plugin modules.
        "Engine/pretty_gdb.py",
        ])

    return None

jafg_load_master_gdb_init()

end

set print pretty 1
