// Copyright 2024 mzoesch. All rights reserved.

package Shared

// Module is a module of a project. Always a child of a project and
// is statically compiled into its parent.
// A project must always have at least one module. That is the primary module.
type Module interface {
    // GetName is the internal name / identifier.
    GetName() string

    // GetFriendlyName is what a user might see.
    GetFriendlyName() string

    // GetRelDirPath is the path to the module root dir.
    // This might include a .jafgmod file if the module is part of a poly module project.
    GetRelDirPath() string

    // IsPrimary is true if this is the primary module of a project.
    IsPrimary() bool
    SetIsPrimary(bPrimary bool)

    // GetParent is the parent project of this module.
    GetParent() *Project

    // IsPoly is true if this module is part of a poly module project.
    IsPoly() bool

    // ToFancyString returns a string representation of the module.
    ToFancyString(indent int) string

    GetPreProcInt() int64
    SetPreProcInt(i int64)

    // GetPublicDependencies are the public transitive dependencies.
    GetPublicDependencies() []string
}
