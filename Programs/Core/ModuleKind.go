// Copyright mzoesch. All rights reserved.

package Core

type ModuleKind int

const (
    MODULE_KIND_SHARED ModuleKind = iota
    MODULE_KIND_STATIC ModuleKind = iota
    MODULE_KIND_LAUNCH ModuleKind = iota
)

func (kind ModuleKind) IsShared() bool {
    return kind == MODULE_KIND_SHARED
}

func (kind ModuleKind) IsStatic() bool {
    return kind == MODULE_KIND_STATIC
}

func (kind ModuleKind) IsLaunch() bool {
    return kind == MODULE_KIND_LAUNCH
}

func (kind ModuleKind) ToString() string {
    switch kind {
    case MODULE_KIND_SHARED:
        return "Shared"
    case MODULE_KIND_STATIC:
        return "Static"
    case MODULE_KIND_LAUNCH:
        return "Launch"
    default:
        panic("Unknown ModuleKind value.")
    }
}

func (kind ModuleKind) ToLuaString() string {
    switch kind {
    case MODULE_KIND_SHARED:
        return "SharedLib"
    case MODULE_KIND_STATIC:
        return "StaticLib"
    case MODULE_KIND_LAUNCH:
        return "ConsoleApp"
    default:
        panic("Unknown ModuleKind value.")
    }
}

func ModuleKindFromString(str string) ModuleKind {
    switch str {
    case "Shared", "SHARED":
        return MODULE_KIND_SHARED
    case "Static", "STATIC":
        return MODULE_KIND_STATIC
    case "Launch", "LAUNCH":
        return MODULE_KIND_LAUNCH
    default:
        panic("Unknown ModuleKind value.")
    }
}
