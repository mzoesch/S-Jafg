# Copyright mzoesch. All rights reserved.

from typing import List

class Workspace:
    friendly_name: str = ''
    ignored_tl_dirs: List[str] = []
    ignored_dirs: List[str] = []

    def is_tl_ignored(self, base: str) -> bool:
        return self._is_ignored(self.ignored_tl_dirs, base)

    def is_ignored(self, base: str) -> bool:
        return self._is_ignored(self.ignored_dirs, base)

    @staticmethod
    def _is_ignored(my_list: List[str], base: str) -> bool:
        for ignore in my_list:
            if ignore == base:
                return True

            if ignore.__contains__('*'):
                idx: int = ignore.index('*')
                ignore_stub: str = ignore[:idx]
                if ignore_stub in base:
                    return True
            continue

        return False
