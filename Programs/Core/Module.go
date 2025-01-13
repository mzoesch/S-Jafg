// Copyright mzoesch. All rights reserved.

package Core

type Module struct {
    Name                string
    RelativeDir         string
    PchUsage            PchUsage
    Kind                ModuleKind
    PublicDependencies  []Dependency
    PrivateDependencies []Dependency
}
