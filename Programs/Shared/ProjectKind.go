// Copyright 2024 mzoesch. All rights reserved.

package Shared

type ProjectKind int

const (
    PKIND_INHERIT ProjectKind = iota
    PKIND_SHARED ProjectKind = iota
    PKIND_STATIC ProjectKind = iota
    PKIND_LAUNCH ProjectKind = iota
)

func (kind ProjectKind) ToString() string {
    switch kind {
    case PKIND_INHERIT:
        return "Inherit"
    case PKIND_SHARED:
        return "Shared"
    case PKIND_STATIC:
        return "Static"
    case PKIND_LAUNCH:
        return "Launch"
    default:
        panic("Unknown project kind")
    }
}

func (kind ProjectKind) ToLuaString(parentKind *ProjectKind) string {
    if kind == PKIND_INHERIT {
        if parentKind == nil {
            panic("Parent kind is nil. But kind should be inherited.")
        }

        return parentKind.ToLuaString(nil)
    }

    switch kind {
    case PKIND_SHARED:
        return "SharedLib"
    case PKIND_STATIC:
        return "StaticLib"
    case PKIND_LAUNCH:
        return "ConsoleApp"
    default:
        panic("Unknown project kind")
    }
}

func (kind ProjectKind) IsInherit() bool {
    return kind == PKIND_INHERIT
}

func (kind ProjectKind) IsShared() bool {
    return kind == PKIND_SHARED
}

func (kind ProjectKind) IsStatic() bool {
    return kind == PKIND_STATIC
}

func (kind ProjectKind) IsLaunch() bool {
    return kind == PKIND_LAUNCH
}

func IsValidProjectKind(kind string) bool {
    switch kind {
    case "Inherit":
        return true
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
    case "Inherit":
        return PKIND_INHERIT
    case "Shared", "SharedLib":
        return PKIND_SHARED
    case "Static", "StaticLib":
        return PKIND_STATIC
    case "Launch", "ConsoleApp":
        return PKIND_LAUNCH
    default:
        panic("Unknown project kind: " + kind)
    }
}
