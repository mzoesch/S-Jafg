# Copyright mzoesch. All rights reserved.

import json
import os.path
from typing import List
from Programs.Shared import *
from .Module import Module
from .Target import Target
from .Workspace import Workspace

class GApp:
    workspace: Workspace = Workspace()
    targets: List[Target] = []
    modules: List[Module] = []

    def __init__(self): # deleted
        pass

    @classmethod
    def _pull_cache(cls) -> None:
        cls._read_workspace_file()
        return None

    @classmethod
    def _push_cache(cls) -> None:
        ...

    @classmethod
    def _read_workspace_file(cls) -> None:
        p: str = os.path.join(get_abs_engine_root_dir(), 'jafg.jafgworkspace')
        with open(p, 'r') as f:
            data = json.load(f)
            if data.get('FriendlyName') is None:
                raise ValueError('Could not find "FriendlyName" in jafg workspace file.')
            if data.get('TpIgnores') is None:
                raise ValueError('Could not find "TpIgnores" in jafg workspace file.')
            if data.get('Ignores') is None:
                raise ValueError('Could not find "Ignores" in jafg')

            friendly_name = data.get('FriendlyName')
            if type(friendly_name) is not str:
                raise ValueError(f'Found non string value for "FriendlyName".')
            GApp.workspace.friendly_name = friendly_name
            if GApp.workspace.friendly_name == '':
                raise ValueError('Friendly name cannot be empty.')

            tp_ignores = data.get('TpIgnores')
            for tp_ignore in tp_ignores:
                if type(tp_ignore) is not str:
                    raise ValueError(f'Found not string value in TpIgnores: {tp_ignore}.')
                if tp_ignore == '':
                    raise ValueError('TpIgnores cannot be empty.')
                cls.workspace.ignored_tl_dirs.append(tp_ignore)
                continue

            ignores = data.get('Ignores')
            for ignore in ignores:
                if type(ignore) is not str:
                    raise ValueError(f'Found non string value in Ignores: {ignore}.')
                if ignore == '':
                    raise ValueError('Ignores cannot be empty.')
                cls.workspace.ignored_dirs.append(ignore)
                continue

        print(f'Finished loading workspace {GApp.workspace.friendly_name}.')
        return None

del GApp.__init__
