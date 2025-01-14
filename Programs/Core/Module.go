// Copyright mzoesch. All rights reserved.

package Core

import (
    "Jafg/Shared"
)

type Module struct {
    Name                string
    RelativeDir         string
    PchUsage            PchUsage
    PchContent          string
    Kind                ModuleKind
    PublicDependencies  []Dependency
    PrivateDependencies []Dependency
}

func (mod *Module) GetUniqueName() string {
    return mod.RelativeDir
}

func (mod *Module) IsEqual(module *Module) bool {
    if mod.Name != module.Name {
        return false
    }
    return mod.RelativeDir == module.RelativeDir
}

func (mod *Module) IsDependency(d *Dependency) bool {
    return mod.Name == d.Name
}

func (mod *Module) GetRelativeTopLevelFilter() string {
    var relDir string = mod.GetFunctionalRelativeDir()
    var lastSlash int = -1
    for i := len(relDir) - 1; i >= 0; i-- {
        if relDir[i] == '/' {
            lastSlash = i
            break
        }
    }
    if lastSlash == -1 {
        return relDir
    }
    return relDir[:lastSlash]
}

func (mod *Module) GetFunctionalRelativeDir() string {
    if len(mod.RelativeDir) == 0 {
        panic("RelativeDir is empty")
    }
    if mod.RelativeDir[0] != '~' {
        panic("RelativeDir must start with ~")
    }

    return mod.RelativeDir[1:]
}

func (mod *Module) GetTransitivePublicDependencies(target *Target, out *[]*Module) {
    if Shared.ContainsByPredicate(*out, func(m *Module) bool {
        return mod.IsEqual(m)
    }) {
        panic("Circular dependency detected.")
    }

    *out = append(*out, mod)

    for _, d := range mod.PublicDependencies {
        var dAsM *Module = Shared.GetByPredicate(target.Modules, func(m Module) bool {
            return m.IsDependency(&d)
        })
        if dAsM == nil {
            panic("Public dependency not found.")
        }
        if !Shared.ContainsByPredicate(*out, func(m *Module) bool {
            return m.IsEqual(dAsM)
        }) {
            dAsM.GetTransitivePublicDependencies(target, out)
        }
    }

    return
}

func (mod *Module) GetTransitiveAllDependencies(target *Target, out *[]*Module) {
    mod.GetTransitivePublicDependencies(target, out)

    for _, d := range mod.PrivateDependencies {
        var dAsM *Module = Shared.GetByPredicate(target.Modules, func(m Module) bool {
            return m.IsDependency(&d)
        })
        if dAsM == nil {
            panic("Private dependency not found.")
        }
        if !Shared.ContainsByPredicate(*out, func(m *Module) bool {
            return m.IsEqual(dAsM)
        }) {
            dAsM.GetTransitivePublicDependencies(target, out)
        }
    }

    return
}
