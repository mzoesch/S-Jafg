// Copyright mzoesch. All rights reserved.

package Core

import (
    "Jafg/Shared"
    "fmt"
)

type Target struct {
    Name     string
    Defines  []string
    Runtime  string
    Symbols  bool
    Optimize bool
    Modules  []Module
}

func (t Target) GetFunctionalRelativeDirFromModule(d Dependency) string {
    for _, m := range t.Modules {
        if m.IsDependency(&d) {
            return m.GetFunctionalRelativeDir()
        }
    }
    panic(fmt.Sprintf("Module [%s] not found.\n", d.Name))
}

func (t Target) GetModuleFromName(name string) *Module {
    return Shared.GetByPredicate(t.Modules, func(m Module) bool {
        return m.Name == name
    })
}

func (t Target) GetModuleFromNameChecked(name string) *Module {
    var m *Module = t.GetModuleFromName(name)
    if m == nil {
        panic(fmt.Sprintf("Module [%s] not found.\n", name))
    }
    return m
}
