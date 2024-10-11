// Copyright 2024 mzoesch. All rights reserved.

package Shared

type ProjectKind int
const (
    SHARED ProjectKind = iota
    STATIC ProjectKind = iota
    LAUNCH ProjectKind = iota
)

func (kind ProjectKind) ToString() string {
    switch kind {
    case SHARED:
        return "Shared"
    case STATIC:
        return "Static"
    case LAUNCH:
        return "Launch"
    default:
        panic("Unknown project kind")
    }
}

func (kind ProjectKind) ToLuaString() string {
    switch kind {
    case SHARED:
        return "SharedLib"
    case STATIC:
        return "StaticLib"
    case LAUNCH:
        return "ConsoleApp"
    default:
        panic("Unknown project kind")
    }
}

func IsValid(kind string) bool {
    switch kind {
    case "Shared":
        return true
    case "Static":
        return true
    case "Launch":
        return true
    default:
        return false
    }
}

func ProjectKindFromString(kind string) ProjectKind {
    switch kind {
    case "Shared":
        return SHARED
    case "Static":
        return STATIC
    case "Launch":
        return LAUNCH
    default:
        panic("Unknown project kind: " + kind)
    }
}
