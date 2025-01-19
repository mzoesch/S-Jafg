// Copyright mzoesch. All rights reserved.

package Core

type Dependency struct {
    Name string
}

func DependencyFromString(name string) Dependency {
    return Dependency{Name: name}
}
