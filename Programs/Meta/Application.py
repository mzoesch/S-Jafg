# Copyright mzoesch. All rights reserved.

import json
import os.path
import re
from typing import List
from Programs.Shared import *
from .Solution import Solution
from .Module import Module
from .Target import Target
from .Workspace import Workspace


class GApp:
    workspace: Workspace = Workspace()
    targets: List[Target] = []
    modules: List[Module] = []
    solutions: List[Solution] = []

    def __init__(self):  # deleted
        pass

    @classmethod
    def add_target(cls, target: Target) -> None:
        if target._name == '':
            raise ValueError('Target name cannot be empty.')
        if bool(re.fullmatch(r'[A-Za-z]+', target._name)) is False:
            raise ValueError(f'Target name may only contain latin letters. Faulty target: {target._name}.')
        if target in cls.targets:
            raise ValueError(f'Target already defined. Faulty target: {target._name}.')
        for tar in cls.targets:
            if tar._name == target._name:
                raise ValueError(f'Target with the name {target._name} is already defined.')

        cls.targets.append(target)
        print(f'Added target {target._name}.')
        return None

    @classmethod
    def add_module(cls, module: Module) -> None:
        if module._name == '':
            raise ValueError('Module name cannot be empty.')
        if bool(re.fullmatch(r"[A-Za-z]+", module._name)) is False:
            raise ValueError(f'Module name may only contain latin letters. Faulty module: {module._name}.')
        if module in cls.modules:
            raise ValueError(f'Module already defined. Faulty module: {module._name}.')
        for mod in cls.modules:
            if mod._name == module._name:
                raise ValueError(f'Module with the name {module._name} is already defined.')

        cls.modules.append(module)
        print(f'Added module {module._name}.')
        return None

    @classmethod
    def add_solution(cls, solution: Solution) -> None:
        solution.validate()

        if solution in cls.solutions:
            raise ValueError(f'Solution already defined. Faulty solution: {solution._name}.')
        for sln in cls.solutions:
            if sln._name == solution._name:
                raise ValueError(f'Solution with the name {solution._name} is already defined.')

        cls.solutions.append(solution)
        print(f'Added solution {solution._name}.')
        return None

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
